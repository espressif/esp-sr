#pragma once
#include "esp_speech_features.h"
#include <stdint.h>

/*
This describes an interface for a MFCC runner, that is, some kind of implementation that can be
fed sample chunks and returns the MFCC cepstrum of those samples. This is an abstracted interface so
multiple implementations can be used.
*/

typedef struct esp_mfcc_data_t esp_mfcc_data_t;

// Options for the mfcc algorithm itself. These more-or-less match the parameters of csf_mfcc (from c_speech_features),
// please refer to its documentation for details.
typedef struct {
    int winstep_ms; // The step between successive windows in ms. (10)
    int winlen_ms;  // The length of the analysis window in ms. (25)
    int nch;        // The number of input channel
    int numcep;     // The number of cepstrum to return
    int nfilter;    // The number of filters in the filterbank
    int nfft;       // The FFT size
    int samp_freq;  // The sample-rate of the signal.
    int low_freq;   // The lowest band edge of mel filters, in hz. (e.g. 0)
    int high_freq;  // The highest band edge of mel filters, in hz. Must not be higher than samp_freq
    float preemph;  // Preemphasis filter coefficient. 0 is no filter. (e.g. 0.97)
    char *win_type; // Analysis window type to apply to each frame， "hanning","hamming","sine","rectangular","povey"
    bool append_energy; // 　If true, the zeroth cepstral coefficient is replaced with the log of the total frame energy
    bool use_power;     // If true, use power of fft spectrum, else use magnitude of fft spectrum
    int use_log_fbank;  // 0: return fbank, 1:  return log(x+log_epsilon), 2: return log(max(x, log_epsilon))
    float log_epsilon;  // log epsilon. (e.g. 1e-7)
    bool psram_first;   // Alloc memory from PSRAM first
    bool remove_dc_offset; // Whether to subtract mean of wave before FFT
} esp_mfcc_opts_t;

/**
 * @brief Un-initialize and free a mfcc runner
 *
 * Function to free a previously allocated mfcc runner.
 *
 * @param r Runner object to destroy
 */
typedef void (*esp_mfcc_op_destroy_t)(esp_mfcc_data_t *r);

/**
 * @brief Initialize parameters for a mfcc runner.
 *
 * After creation, a mfcc runner needs to be initialized first; this is usually done
 * in the initialization routine of a speech recognition algorithm. This provides
 * a pointer to do this for a specific mfcc runner.
 *
 * @param opt Options for the mfcc process
 * @return True if success, false on error.
 */
typedef esp_mfcc_data_t *(*esp_mfcc_op_create_t)(const esp_mfcc_opts_t *opt);

/**
 * @brief Run a mfcc iteration on frame by frame
 *
 * This will take a set of samples and return a ceptrum. Note that this may be pipelined:
 * an initial call to this function may return NULL and subsequent calls may return the
 * cepstrum of previous calls.
 *
 * @param r The mfcc runner
 * @param samp An array of signed 16-bit samples. The amount of samples should be sampfreq/(winstep_ms/1000).
 * @return A set of cepstral values, or NULL if no such values are available yet. Free using the free_cepbuf function
 *         when done with this buffer. Note that some implementations require the buffer to be freed before another call
 *         to this function is done.
 */
typedef float *(*esp_mfcc_op_run_step_t)(esp_mfcc_data_t *r, int16_t *samp, int16_t nch);

typedef void (*esp_mfcc_op_run_step_s16_t)(esp_mfcc_data_t *r, int16_t *samp, int16_t *fbank);

/**
 * @brief Clean all state of mfcc handle
 *
 * @param r The mfcc runner
 */
typedef void (*esp_mfcc_op_clean_t)(esp_mfcc_data_t *r);

/**
 * @brief Operations possible on a mfcc runner
 */
typedef struct {
    esp_mfcc_op_destroy_t destroy;
    esp_mfcc_op_create_t create;
    esp_mfcc_op_run_step_t run_step;
    esp_mfcc_op_run_step_s16_t run_step_s16;
    esp_mfcc_op_clean_t clean;
} esp_mfcc_iface_t;
