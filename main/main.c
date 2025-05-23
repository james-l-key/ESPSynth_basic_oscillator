/**
 * @file main.c
 * @brief Main application file for the oscillator module, integrating I2C, I2S, NVS, and menu system.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include "nvs_flash.h"
#include "common.h"
#include "module_i2c_proto.h"
#include "waveform_gen.h"
#include "Esp_menu.h"
#include "user_actions.h"

/** @brief Enable debug mode with PCM5102A output (GPIO 4–6). */
#define DEBUG_MODE 1

/** @brief I2C slave address for the oscillator module. */
#define I2C_SLAVE_ADDR 0x50

/** @brief I2C port number. */
#define I2C_PORT I2C_NUM_0

/** @brief I2S port number. */
#define I2S_PORT I2S_NUM_0

/** @brief Audio sample rate (Hz). */
#define SAMPLE_RATE 44100

/** @brief TCA9548A channel for I2C communication. */
#define TCA9548A_CHANNEL 0

/** @brief Array of parameter values for the oscillator module. */
static ParamValue_t params[] = {
    [PARAM_OSC_WAVEFORM - PARAM_RANGE_OSC] = {.u8[0] = OSC_WAVE_SINE},
    [PARAM_OSC_FREQUENCY_PITCH -
        PARAM_RANGE_OSC] = {.u8[0] = 69},
    [PARAM_OSC_FREQUENCY_FINE -
        PARAM_RANGE_OSC] = {.s16[0] = 0},
    [PARAM_OSC_LEVEL -
        PARAM_RANGE_OSC] = {.u16[0] = 65535},
    [PARAM_OSC_AMP_MOD_SLOT -
        PARAM_RANGE_OSC] = {.u8[0] = 0xFF},
    [PARAM_OSC_FREQ_MOD_SLOT -
        PARAM_RANGE_OSC] = {.u8[0] = 0xFF},
    [PARAM_OSC_PW -
        PARAM_RANGE_OSC] = {.u16[0] = 32768},
    [PARAM_OSC_SYNC_SOURCE_SLOT -
        PARAM_RANGE_OSC] = {.u8[0] = 0xFF}};

/** @brief I2S configuration for the oscillator module. */
static I2sConfig_t i2s_config = {0, 0x0001};

#ifdef CONFIG_ESPMENU_ENABLE_NVS
/** @brief Flag indicating if parameters have changed (for NVS saving). */
bool param_changed = false;

/** @brief Timestamp of the last parameter change. */
TickType_t last_param_change = 0;
#endif

/**
 * @brief Initializes the I2C slave interface for communication with the central controller.
 */
void init_i2c_slave()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = 8,
        .scl_io_num = 9,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDR,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 1024, 1024, 0));
}

/**
 * @brief Initializes the I2S interface for audio output.
 */
void init_i2s()
{
    i2s_config_t i2s_conf = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
    };
    ESP_ERROR_CHECK(i2s_driver_install(I2S_PORT, &i2s_conf, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_pin(I2S_PORT, &(i2s_pin_config_t){
                                              .bck_io_num = 4,
                                              .ws_io_num = 5,
                                              .data_out_num = 6,
                                              .data_in_num = I2S_PIN_NO_CHANGE}));
}

/**
 * @brief Task to handle I2C slave communication, processing commands from the central controller.
 * @param arg Unused task argument.
 */
void i2c_slave_task(void *arg)
{
    uint8_t data[8];
    while (1)
    {
        int len = i2c_slave_read_buffer(I2C_PORT, data, sizeof(data), pdMS_TO_TICKS(I2C_TIMEOUT_MS));
        if (len > 0)
        {
            ESP_LOGD("I2C_SLAVE", "Received %d bytes: cmd=0x%02X", len, data[0]);
            if (data[0] == REG_COMMON_SET_PARAM && len >= 7)
            {
                ParamId_t param_id;
                ParamValue_t param_value;
                if (i2c_proto_unpack_set_param_payload(data + 1, len - 1, &param_id, &param_value))
                {
                    if (param_id >= PARAM_RANGE_OSC && param_id < PARAM_RANGE_OSC + sizeof(params) / sizeof(params[0]))
                    {
                        params[param_id - PARAM_RANGE_OSC] = param_value;
                        switch (param_id)
                        {
                        case PARAM_OSC_FREQUENCY_PITCH:
                            menu_params.frequency_pitch = param_value.u8[0];
                            break;
                        case PARAM_OSC_FREQUENCY_FINE:
                            menu_params.frequency_fine = param_value.s16[0];
                            break;
                        case PARAM_OSC_WAVEFORM:
                            menu_params.waveform = param_value.u8[0];
                            break;
                        case PARAM_OSC_LEVEL:
                            menu_params.level = param_value.u16[0];
                            break;
                        case PARAM_OSC_PW:
                            menu_params.pulse_width = param_value.u16[0];
                            break;
                        case PARAM_OSC_AMP_MOD_SLOT:
                            menu_params.amp_mod_slot = param_value.u8[0];
                            break;
                        case PARAM_OSC_FREQ_MOD_SLOT:
                            menu_params.freq_mod_slot = param_value.u8[0];
                            break;
                        case PARAM_OSC_SYNC_SOURCE_SLOT:
                            menu_params.sync_source_slot = param_value.u8[0];
                            break;
                        }
                        save_to_nvs();
                        user_update_display();
                    }
                }
            }
            else if (data[0] == REG_COMMON_I2S_CONFIG && len >= 5)
            {
                i2c_proto_unpack_i2s_config_packet(data + 1, len - 1, &i2s_config);
            }
            else if (data[0] == CMD_COMMON_RESET)
            {
                menu_params = (MenuParams_t){69, 0, OSC_WAVE_SINE, 65535, 32768, 0xFF, 0xFF, 0xFF};
                params[PARAM_OSC_WAVEFORM - PARAM_RANGE_OSC] = (ParamValue_t){.u8[0] = OSC_WAVE_SINE};
                params[PARAM_OSC_FREQUENCY_PITCH - PARAM_RANGE_OSC] = (ParamValue_t){.u8[0] = 69};
                params[PARAM_OSC_FREQUENCY_FINE - PARAM_RANGE_OSC] = (ParamValue_t){.s16[0] = 0};
                params[PARAM_OSC_LEVEL - PARAM_RANGE_OSC] = (ParamValue_t){.u16[0] = 65535};
                params[PARAM_OSC_PW - PARAM_RANGE_OSC] = (ParamValue_t){.u16[0] = 32768};
                params[PARAM_OSC_AMP_MOD_SLOT - PARAM_RANGE_OSC] = (ParamValue_t){.u8[0] = 0xFF};
                params[PARAM_OSC_FREQ_MOD_SLOT - PARAM_RANGE_OSC] = (ParamValue_t){.u8[0] = 0xFF};
                params[PARAM_OSC_SYNC_SOURCE_SLOT - PARAM_RANGE_OSC] = (ParamValue_t){.u8[0] = 0xFF};
                save_to_nvs();
                user_update_display();
            }
            else if (data[0] == CMD_COMMON_SAVE_SETTINGS)
            {
                save_to_nvs();
            }
        }
    }
}

/**
 * @brief Task to generate and output audio waveforms via I2S.
 * @param arg Unused task argument.
 */
void audio_task(void *arg)
{
    int16_t buffer[64];
    waveform_init(SAMPLE_RATE);
    while (1)
    {
        waveform_set_params(
            menu_params.frequency_pitch,
            menu_params.frequency_fine,
            menu_params.waveform,
            menu_params.level,
            menu_params.pulse_width,
            menu_params.amp_mod_slot,
            menu_params.freq_mod_slot,
            menu_params.sync_source_slot);
        waveform_generate(buffer, 64);
        size_t bytes_written;
        i2s_write(I2S_PORT, buffer, 64 * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    }
}

#ifdef CONFIG_ESPMENU_ENABLE_NVS
/**
 * @brief Task to periodically save changed parameters to NVS.
 * @param arg Unused task argument.
 */
void nvs_task(void *arg)
{
    while (1)
    {
        if (param_changed && (xTaskGetTickCount() - last_param_change > pdMS_TO_TICKS(1000)))
        {
            save_to_nvs();
            param_changed = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif

/**
 * @brief Main application entry point, initializing NVS, I2C, I2S, and menu system.
 */
void app_main(void)
{
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
#endif

    init_i2c_slave();
    init_i2s();
    user_init();
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 4096, NULL, 5, NULL);
    xTaskCreate(audio_task, "audio_task", 4096, NULL, 5, NULL);
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    xTaskCreate(nvs_task, "nvs_task", 2048, NULL, 4, NULL);
#endif

    esp_err_t err = esp_menu_init();
    if (err != ESP_OK)
    {
        printf("Failed to initialize menu system: %s\n", esp_err_to_name(err));
        return;
    }
}