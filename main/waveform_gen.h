/**
 * @file waveform_gen.h
 * @brief Header file for waveform generation functions used in the oscillator module.
 */

#ifndef WAVEFORM_GEN_H
#define WAVEFORM_GEN_H

#include <stdint.h>
#include "synth_constants.h"

/**
 * @brief Initializes the waveform generator with the specified sample rate.
 * @param sample_rate The audio sample rate in Hz (e.g., 44100).
 */
void waveform_init(uint32_t sample_rate);

/**
 * @brief Sets the parameters for waveform generation.
 * @param freq_pitch MIDI note number for frequency (0–127).
 * @param freq_fine Fine frequency adjustment in cents (-100 to 100).
 * @param waveform Waveform type (sine, triangle, saw, square, pulse).
 * @param level Output level (0–65535).
 * @param pw Pulse width for pulse wave (0–65535).
 * @param amp_slot Amplitude modulation slot (0–15 or 0xFF for none).
 * @param freq_slot Frequency modulation slot (0–15 or 0xFF for none).
 * @param sync Sync source slot (0–15 or 0xFF for none).
 */
void waveform_set_params(uint8_t freq_pitch, int16_t freq_fine, OscWaveform_t waveform, uint16_t level, uint16_t pw, uint8_t amp_slot, uint8_t freq_slot, uint8_t sync);

/**
 * @brief Generates a buffer of waveform samples.
 * @param buffer Pointer to the output buffer for 16-bit samples.
 * @param num_samples Number of samples to generate.
 */
void waveform_generate(int16_t *buffer, uint32_t num_samples);

#endif