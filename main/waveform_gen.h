#ifndef WAVEFORM_GEN_H
#define WAVEFORM_GEN_H

#include "module_i2c_proto.h"

void waveform_init(uint32_t sample_rate);
void waveform_set_params(uint8_t freq_pitch, int16_t freq_fine, OscWaveform_t waveform, uint16_t level, uint16_t pulse_width, uint8_t amp_mod_slot, uint8_t freq_mod_slot, uint8_t sync_slot);
void waveform_generate(int16_t *buffer, uint32_t num_samples);

#endif // WAVEFORM_GEN_H