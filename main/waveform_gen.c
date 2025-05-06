#include "waveform_gen.h"
#include <math.h>

#define SAMPLE_RATE 44100
#define TABLE_SIZE 1024
#define MIDI_A4 69
#define A4_FREQ 440.0f
#define CENTS_PER_OCTAVE 1200.0f

static uint8_t base_freq_pitch = MIDI_A4;
static int16_t base_freq_fine = 0;
static OscWaveform_t waveform_type = OSC_WAVE_SINE;
static uint16_t level = 65535;
static uint16_t pulse_width = 32768; // 50%
static uint8_t amp_mod_slot = 0xFF;
static uint8_t freq_mod_slot = 0xFF;
static uint8_t sync_slot = 0xFF;
static int16_t sine_table[TABLE_SIZE];
static float phase = 0.0f;

void waveform_init(uint32_t sample_rate)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        sine_table[i] = (int16_t)(32767.0f * sinf(2.0f * M_PI * i / TABLE_SIZE));
    }
}

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

void waveform_generate(int16_t *buffer, uint32_t num_samples)
{
    float semitones = (float)(base_freq_pitch - MIDI_A4) + (float)base_freq_fine / CENTS_PER_OCTAVE * 12.0f;
    float base_frequency = A4_FREQ * powf(2.0f, semitones / 12.0f);
    float phase_inc = 2.0f * M_PI * base_frequency / SAMPLE_RATE;
    float pw_ratio = (float)pulse_width / 65535.0f;

    for (uint32_t i = 0; i < num_samples; i++)
    {
        float sample = 0.0f;
        uint32_t index = (uint32_t)(phase * TABLE_SIZE / (2.0f * M_PI)) % TABLE_SIZE;

        // TODO: Apply TDM modulation (amp_mod_slot, freq_mod_slot, sync_slot)

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

        sample *= (float)level / 65535.0f;
        buffer[i] = (int16_t)sample;

        phase += phase_inc;
        if (phase >= 2.0f * M_PI)
            phase -= 2.0f * M_PI;
    }
}