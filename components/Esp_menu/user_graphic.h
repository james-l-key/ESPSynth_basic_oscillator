#ifndef USER_GRAPHICS_H
#define USER_GRAPHICS_H
#include "lvgl.h"
// Icon for waveform selection
static const uint8_t waveform_icon_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x20, 0x04,
    0x40, 0x02, 0x80, 0x01, 0x80, 0x01, 0x40, 0x02,
    0x20, 0x04, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00
};
const lv_image_dsc_t waveform_icon = {
    .header = {.magic = LV_IMAGE_HEADER_MAGIC, .w = 16, .h = 12, .cf = LV_COLOR_FORMAT_MONO1},
    .data = waveform_icon_data,
    .data_size = sizeof(waveform_icon_data)
};
#endif