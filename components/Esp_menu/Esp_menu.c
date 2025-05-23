/**
 * @file Esp_menu.c
 * @brief Implementation of the ESP Menu component for initializing the menu system with SSD1306 display and rotary encoders.
 */

#include "Esp_menu.h"
#include "menu_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"
#include "esp_lcd_panel_io.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "iot_knob.h"

/** @brief Logging tag for ESP Menu component. */
#define TAG "Esp_menu"

/** @brief Macro to check ESP error codes and return on failure with logging. */
#define BSP_ERROR_CHECK_RETURN_ERR(x)                                                \
    do                                                                               \
    {                                                                                \
        esp_err_t err_rc = (x);                                                      \
        if (err_rc != ESP_OK)                                                        \
        {                                                                            \
            ESP_LOGE(TAG, "Error %s at line %d", esp_err_to_name(err_rc), __LINE__); \
            return err_rc;                                                           \
        }                                                                            \
    } while (0)

/** @brief Handle for the LCD panel. */
static esp_lcd_panel_handle_t lcd_handle = NULL;

/**
 * @brief Initializes the ESP Menu system, including I2C, SSD1306 display, rotary encoders, and LVGL.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_menu_init(void)
{
    ESP_LOGI(TAG, "Starting menu system initialization");

    // Initialize I2C master bus for SSD1306
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_NUM_1,
        .sda_io_num = CONFIG_ESPMENU_SSD1306_I2C_SDA,
        .scl_io_num = CONFIG_ESPMENU_SSD1306_I2C_SCL,
        .flags.enable_internal_pullup = true,
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&bus_config, &i2c_bus));

    // Initialize LCD panel I/O
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x3C,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6};
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_bus, &io_config, &io_handle));

    // Initialize SSD1306 panel
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .bits_per_pixel = 1};
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_panel_reset(panel_handle));
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_panel_init(panel_handle));
    lcd_handle = panel_handle;

    // Configure GPIO for rotary encoders
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask =
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_A) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_B) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_2_A) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_2_B) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_2_BUTTON) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_3_A) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_3_B) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_3_BUTTON) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_4_A) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_4_B) |
            (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_4_BUTTON),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE};
    BSP_ERROR_CHECK_RETURN_ERR(gpio_config(&io_conf));

    // Initialize rotary encoders and buttons
    button_handle_t encoder_btn_handles[4] = {NULL};
    knob_handle_t encoder_knob_handles[4] = {NULL};
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = 0,
            .active_level = 0}};
    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = 0,
        .gpio_encoder_b = 0};
    const int encoder_pins[4][3] = {
        {CONFIG_ESPMENU_ROTARY_ENCODER_1_A, CONFIG_ESPMENU_ROTARY_ENCODER_1_B, CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON},
        {CONFIG_ESPMENU_ROTARY_ENCODER_2_A, CONFIG_ESPMENU_ROTARY_ENCODER_2_B, CONFIG_ESPMENU_ROTARY_ENCODER_2_BUTTON},
        {CONFIG_ESPMENU_ROTARY_ENCODER_3_A, CONFIG_ESPMENU_ROTARY_ENCODER_3_B, CONFIG_ESPMENU_ROTARY_ENCODER_3_BUTTON},
        {CONFIG_ESPMENU_ROTARY_ENCODER_4_A, CONFIG_ESPMENU_ROTARY_ENCODER_4_B, CONFIG_ESPMENU_ROTARY_ENCODER_4_BUTTON}};
    for (int i = 0; i < 4; i++)
    {
        btn_cfg.gpio_button_config.gpio_num = encoder_pins[i][2];
        BSP_ERROR_CHECK_RETURN_ERR(iot_button_create(&btn_cfg, &encoder_btn_handles[i]));
        knob_cfg.gpio_encoder_a = encoder_pins[i][0];
        knob_cfg.gpio_encoder_b = encoder_pins[i][1];
        BSP_ERROR_CHECK_RETURN_ERR(iot_knob_create(&knob_cfg, &encoder_knob_handles[i]));
    }

    // Initialize LVGL
    lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 5,
        .task_stack = 4096,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5};
    BSP_ERROR_CHECK_RETURN_ERR(lvgl_port_init(&lvgl_cfg));

    // Add display to LVGL
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = 128 * 64,
        .double_buffer = true,
        .hres = 128,
        .vres = 64,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false}};
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
    if (!disp)
    {
        ESP_LOGE(TAG, "Failed to add display to LVGL");
        return ESP_FAIL;
    }

    // Add encoders to LVGL
    const char *encoder_names[4] = {"encoder1", "encoder2", "encoder3", "encoder4"};
    for (int i = 0; i < 4; i++)
    {
        lvgl_port_encoder_cfg_t encoder_cfg = {
            .disp = disp,
            .encoder_a_b = encoder_knob_handles[i],
            .encoder_enter = encoder_btn_handles[i]};
        lv_indev_t *indev = lvgl_port_add_encoder(&encoder_cfg);
        if (!indev)
        {
            ESP_LOGE(TAG, "Failed to add encoder %d to LVGL", i + 1);
            return ESP_FAIL;
        }
        lv_group_t *group = lv_group_create();
        lv_indev_set_group(indev, group);
        lv_obj_set_user_data(indev, (void *)encoder_names[i]);
    }

    // Initialize menu widgets
    ESP_LOGI(TAG, "Initializing LVGL menu widgets");
    menu_init();

    ESP_LOGI(TAG, "Menu system fully initialized");
    return ESP_OK;
}