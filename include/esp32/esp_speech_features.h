#pragma once
#include "c_speech_features_config.h"
#include "stdlib.h"
#include <assert.h>
#include <stdbool.h>

#ifndef M_2PI
#define M_2PI 6.283185307179586476925286766559005
#endif

typedef struct {
    float *coeff;
    int *bank_pos;
    int nfilter;
} esp_mel_filter_t;

float *esp_mfcc_malloc(size_t size, bool from_psram);

void esp_mfcc_free(void *ptr);

/**
 * @brief Initialize FFT table
 * @warning For ESP-PLATFORM, use esp-dsp fft
 *          For Other platform, use kiss fft
 *
 * @param nfft  The input samples number
 * @return fft-table
 **/
void *esp_fft_init(int nfft);

/**
 * @brief Free FFT table
 * @warning For ESP-PLATFORM, use esp-dsp fft
 *          For Other platform, use kiss fft
 *
 * @param fft_table  The fft table initialized by esp_fft_init
 * @param nfft       The input samples number
 * @return fft-table
 **/
void esp_fft_deinit(void *fft_table, int nfft);

/**
 * @brief Initial window function
 *        Currently support hanning, hamming, sine, povey, rectangular,
 *        wn9(512-hanning to get wakenet9& multinet5 compatible)
 **/
float *esp_win_func_init(char *win_type, float *window_data, int frame_length);

float *esp_fftr(float *x, int nfft, void *fft_table);

float *esp_spectrum_step(float *x, int nfft, bool use_power, void *fft_handle);

void esp_audio_short_to_float(short *samples, float *x, int len, int remove_dc);

float *esp_preemphasis_step(float *x, unsigned int len, float coeff, float last);

esp_mel_filter_t *esp_mel_filter_init(
    int nfft, int nfilter, int low_freq, int high_freq, int samp_freq, bool from_psram);

void esp_mel_filter_deinit(esp_mel_filter_t *mel_filter);

float *esp_mel_dotprod_step(float *x, float *out, esp_mel_filter_t *mel_filter, int use_log_fbank, float epsilon);
