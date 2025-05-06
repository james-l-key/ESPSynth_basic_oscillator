#include "Esp_menu.h"
#include "nvs_flash.h"
#include "module_i2c_proto.h"
#include <string.h>

#define NUM_FAVORITE_SLOTS 4

typedef struct {
    uint8_t frequency_pitch;
    int16_t frequency_fine;
    OscWaveform_t waveform;
    uint16_t level;
    uint16_t pulse_width;
    uint8_t amp_mod_slot;
    uint8_t freq_mod_slot;
    uint8_t sync_source_slot;
} MenuParams_t;

extern MenuParams_t menu_params;
static uint8_t current_slot = 0;
static bool param_changed = false;
static TickType_t last_param_change = 0;

void save_to_nvs(void) {
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READWRITE, &nvs);
    if (err != ESP_OK) return;
    nvs_set_u8(nvs, "freq_pitch", menu_params.frequency_pitch);
    nvs_set_i16(nvs, "freq_fine", menu_params.frequency_fine);
    nvs_set_u8(nvs, "waveform", menu_params.waveform);
    nvs_set_u16(nvs, "level", menu_params.level);
    nvs_set_u16(nvs, "pulse_width", menu_params.pulse_width);
    nvs_set_u8(nvs, "amp_mod_slot", menu_params.amp_mod_slot);
    nvs_set_u8(nvs, "freq_mod_slot", menu_params.freq_mod_slot);
    nvs_set_u8(nvs, "sync_slot", menu_params.sync_source_slot);
    nvs_commit(nvs);
    nvs_close(nvs);
}

void load_from_nvs(void) {
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READONLY, &nvs);
    if (err != ESP_OK) return;
    nvs_get_u8(nvs, "freq_pitch", &menu_params.frequency_pitch);
    nvs_get_i16(nvs, "freq_fine", &menu_params.frequency_fine);
    nvs_get_u8(nvs, "waveform", (uint8_t*)&menu_params.waveform);
    nvs_get_u16(nvs, "level", &menu_params.level);
    nvs_get_u16(nvs, "pulse_width", &menu_params.pulse_width);
    nvs_get_u8(nvs, "amp_mod_slot", &menu_params.amp_mod_slot);
    nvs_get_u8(nvs, "freq_mod_slot", &menu_params.freq_mod_slot);
    nvs_get_u8(nvs, "sync_slot", &menu_params.sync_source_slot);
    nvs_close(nvs);
}

void save_favorite(uint8_t slot) {
    if (slot >= NUM_FAVORITE_SLOTS) return;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READWRITE, &nvs);
    if (err != ESP_OK) return;
    char key[16];
    snprintf(key, sizeof(key), "fav_slot_%d", slot);
    nvs_set_blob(nvs, key, &menu_params, sizeof(MenuParams_t));
    nvs_commit(nvs);
    nvs_close(nvs);
}

void load_favorite(uint8_t slot) {
    if (slot >= NUM_FAVORITE_SLOTS) return;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READONLY, &nvs);
    if (err != ESP_OK) return;
    char key[16];
    snprintf(key, sizeof(key), "fav_slot_%d", slot);
    size_t size = sizeof(MenuParams_t);
    nvs_get_blob(nvs, key, &menu_params, &size);
    nvs_close(nvs);
    save_to_nvs();
}

void clear_favorite(uint8_t slot) {
    if (slot >= NUM_FAVORITE_SLOTS) return;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READWRITE, &nvs);
    if (err != ESP_OK) return;
    char key[16];
    snprintf(key, sizeof(key), "fav_slot_%d", slot);
    nvs_erase_key(nvs, key);
    nvs_commit(nvs);
    nvs_close(nvs);
}

void pitch_up(void) {
    menu_params.frequency_pitch = menu_params.frequency_pitch < 127 ? menu_params.frequency_pitch + 1 : 127;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void pitch_down(void) {
    menu_params.frequency_pitch = menu_params.frequency_pitch > 0 ? menu_params.frequency_pitch - 1 : 0;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void waveform_next(void) {
    menu_params.waveform = (menu_params.waveform + 1) % 5;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void waveform_prev(void) {
    menu_params.waveform = (menu_params.waveform + 4) % 5;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void level_up(void) {
    menu_params.level = menu_params.level < 65535 - 655 ? menu_params.level + 655 : 65535;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void level_down(void) {
    menu_params.level = menu_params.level > 655 ? menu_params.level - 655 : 0;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void fine_tune_up(void) {
    menu_params.frequency_fine = menu_params.frequency_fine < 100 ? menu_params.frequency_fine + 1 : 100;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void fine_tune_down(void) {
    menu_params.frequency_fine = menu_params.frequency_fine > -100 ? menu_params.frequency_fine - 1 : -100;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void pulse_width_up(void) {
    menu_params.pulse_width = menu_params.pulse_width < 65535 - 655 ? menu_params.pulse_width + 655 : 65535;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void pulse_width_down(void) {
    menu_params.pulse_width = menu_params.pulse_width > 655 ? menu_params.pulse_width - 655 : 0;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void amp_mod_slot_next(void) {
    menu_params.amp_mod_slot = menu_params.amp_mod_slot == 0xFF ? 0 : (menu_params.amp_mod_slot < 15 ? menu_params.amp_mod_slot + 1 : 0xFF);
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void amp_mod_slot_prev(void) {
    menu_params.amp_mod_slot = menu_params.amp_mod_slot == 0xFF ? 15 : (menu_params.amp_mod_slot > 0 ? menu_params.amp_mod_slot - 1 : 0xFF);
    param_changed = true;
    last_param_change = xTaskGetTickCount();
}

void select_favorite_slot_next(void) {
    current_slot = (current_slot + 1) % NUM_FAVORITE_SLOTS;
}

void select_favorite_slot_prev(void) {
    current_slot = (current_slot + NUM_FAVORITE_SLOTS - 1) % NUM_FAVORITE_SLOTS;
}

void save_favorite_action(void) {
    save_favorite(current_slot);
}

void load_favorite_action(void) {
    load_favorite(current_slot);
}

void clear_favorite_action(void) {
    clear_favorite(current_slot);
}