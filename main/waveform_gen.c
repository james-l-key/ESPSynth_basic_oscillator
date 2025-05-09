/**
 * @file waveform_gen.c
 * @brief Implementation of waveform generation for the oscillator module, supporting sine, triangle, saw, square, and pulse waves.
 */

#include "waveform_gen.h"
#include <math.h>

/** @brief Audio sample rate (Hz). */
#define SAMPLE_RATE 44100

/** @brief Size of the sine wave lookup table. */
#define TABLE_SIZE 1024

/** @brief MIDI note number for A4 (440 Hz). */
#define MIDI_A4 69

/** @brief Reference frequency for A4 (Hz). */
#define A4_FREQ 440.0f

/** @brief Number of cents per octave. */
#define CENTS_PER_OCTAVE 1200.0f

/** @brief Current frequency pitch (MIDI note number). */
static uint8_t base_freq_pitch = MIDI_A4;

/** @brief Fine frequency adjustment (cents). */
static int16_t base_freq_fine = 0;

/** @brief Current waveform type. */
static OscWaveform_t waveform_type = OSC_WAVE_SINE;

/** @brief Output level (0–65535). */
static uint16_t level = 65535;

/** @brief Pulse width for pulse wave (0–65535). */
static uint16_t pulse_width = 32768;

/** @brief Amplitude modulation slot (0–15 or 0xFF). */
static uint8_t amp_mod_slot = 0xFF;

/** @brief Frequency modulation slot (0–15 or 0xFF). */
static uint8_t freq_mod_slot = 0xFF;

/** @brief Sync source slot (0–15 or 0xFF). */
static uint8_t sync_slot = 0xFF;

/** @brief Lookup table for sine wave. */
static int16_t sine_table[TABLE_SIZE];

/** @brief Current phase of the waveform (radians). */
static float phase = 0.0f;

/**
 * @brief Reads a modulation value from a TDM slot.
 * @param slot The TDM slot number (0–15).
 * @return float The modulation value (placeholder implementation).
 * @note Requires implementation for I2C or shared memory access.
 */
static float read_tdm_slot(uint8_t slot)
{
    // TODO: Implement TDM buffer access via I2C or shared memory
    return 0.0f;
}

/**
 * @brief Initializes the waveform generator with the specified sample rate.
 * @param sample_rate The audio sample rate in Hz (e.g., 44100).
 */
void waveform_init(uint32_t sample_rate)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        sine_table[i] = (int16_t)(32767.0f * sinf(2.0f * M_PI * i / TABLE_SIZE));
    }
}

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
void waveform_set_params(uint8_t freq_pitch, int16_t freq_fine, OscWaveform_t waveform, uint16_t lvl, uint16_t pw, uint8_t amp_slot, uint8_t freq_slot, uint8_t sync)
{
    base_freq_pitch = freq_pitch > 127 ? 127 : freq_pitch;
    base_freq_fine = freq_fine > 100 ? 100 : (freq_fine < -100 ? -100 : freq_fine);
    waveform_type = waveform;
    level = lvl;
    pulse_width = pw;
    amp_mod_slot = amp_slot;
    freq_mod_slot = freq_slot;
    sync_slot = sync;
}

/**
 * @brief Generates a buffer of waveform samples.
 * @param buffer Pointer to the output buffer for 16-bit samples.
 * @param num_samples Number of samples to generate.
 */
void waveform_generate(int16_t *buffer, uint32_t num_samples)
{
    float semitones = (float)(base_freq_pitch - MIDI_A4) + (float)base_freq_fine / CENTS_PER_OCTAVE * 12.0f;
    float base_frequency = A4_FREQ * powf(2.0f, semitones / 12.0f);
    float phase_inc = 2.0f * M_PI * base_frequency / SAMPLE_RATE;
    float pw_ratio = (float)pulse_width / 65535.0f;
    float amp_mod = (amp_mod_slot != 0xFF) ? read_tdm_slot(amp_mod_slot) : 1.0f;
    float freq_mod = (freq_mod_slot != 0xFF) ? read_tdm_slot(freq_mod_slot) : 0.0f;

    for (uint32_t i = 0; i < num_samples; i++)
    {
        float sample = 0.0f;
        uint32_t index = (uint32_t)(phase * TABLE_SIZE / (2.0f * M_PI)) % TABLE_SIZE;
        switch (waveform_type)
        {
        case OSC_WAVE_SINE:
            sample = sine_table[index];
            break;
        case OSC_WAVE_TRIANGLE:
            sample = 32767.0f * (2.0f * fabs(phase / M_PI - 1.0f) - 1.0f);
            break;
        case OSC_WAVE_SAW:
            sample = 32767.0f * (1.0f - (phase / M_PI));
            break;
        case OSC_WAVE_SQUARE:
            sample = (phase < M_PI) ? 32767.0f : -32767.0f;
            break;
        case OSC_WAVE_PULSE:
            sample = (phase < 2.0f * M_PI * pw_ratio) ? 32767.0f : -32767.0f;
            break;
        }
        sample *= (float)level / 65535.0f * amp_mod;
        buffer[i] = (int16_t)sample;
        phase += phase_inc + freq_mod;
        if (phase >= 2.0f * M_PI)
            phase -= 2.0f * M_PI;
    }
}