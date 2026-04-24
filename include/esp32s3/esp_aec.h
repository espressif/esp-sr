// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License
#ifndef _ESP_AEC_H_
#define _ESP_AEC_H_

#include <stdint.h>
#include "esp_aec_nlp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AEC_MODE_SR_LOW_COST = 0,     // Low Cost AEC for speech recognition
    AEC_MODE_SR_HIGH_PERF = 1,    // High Perforamce AEC for speech recognition
    AEC_MODE_VOIP_LOW_COST = 3,   // Low Cost AEC for voice communication
    AEC_MODE_VOIP_HIGH_PERF = 4,  // High Perforamce AEC for voice communication
    AEC_MODE_FD_LOW_COST = 5,    // Low Cost AEC for full duplex
    AEC_MODE_FD_HIGH_PERF = 6,   // High Perforamce AEC for full duplex
} aec_mode_t;


/**
 * @brief Configuration structure for the ESP Acoustic Echo Canceller (AEC).
 *
 * This structure is used to configure the parameters for the ESP AEC module.
 */
typedef struct {
    int mic_num;          /**< Number of microphones used for input. */
    int ref_num;          /**< Number of reference signals (e.g., playback signals). */
    int out_num;          /**< Number of output channels. */
    int filter_length;    /**< Length of the echo cancellation filter. */
    int sample_rate;      /**< Sampling rate of the audio signals in Hz. */
    uint32_t caps;        /**< Bitwise OR of MALLOC_CAP_* flags indicating the type of memory to be returned. */
    aec_mode_t mode;      /**< Operating mode of the AEC (e.g., full-duplex, half-duplex). */
    aec_nlp_level_t nlp_level; /**< Non-Linear Processing (NLP) level for echo suppression. */
} aec_config_t;

typedef struct {
    void* aec_handle;
    int frame_size;
    aec_config_t config;
}aec_handle_t;

/**
 * @brief Creates an instance to the AEC structure.
 * Please get frame size by aec_get_chunksize() function
 * 
 * @param sample_rate       The Sampling frequency (Hz) must be 16000.
 * @param filter_length     Number of filter, recommend to set 4. The larger the filter_length, the more resource consumption.
 * @param channel_num       The input microphone channel number
 * @param mode              The mode of AEC, recommend to set AEC_MODE_SR_LOW_COST
 * @return
 *         - NULL: Create failed
 *         - Others: The instance of AEC
 */
aec_handle_t *aec_create(int sample_rate, int filter_length, int channel_num, aec_mode_t mode);

/**
 * @brief Create an Acoustic Echo Canceller (AEC) instance with the specified configuration.
 *
 * This function initializes and returns a handle to an AEC instance based on the provided
 * configuration parameters. The AEC is used to reduce or eliminate echo in audio processing
 * applications.
 *
 * @param config Pointer to an aec_config_t structure containing the configuration
 *                   parameters for the AEC instance.
 *
 * @return Pointer to the created aec_handle_t instance on success, or NULL on failure.
 */
aec_handle_t *aec_create_from_config(aec_config_t *config);

/**
 * @brief Performs echo cancellation a frame, based on the audio sent to the speaker and frame from mic.
 *
 * @warning The indata, refdata and outdata must be 16-bit signed. please allocate memory by heap_caps_aligned_alloc().
 * 
 * @param inst        The instance of AEC. Format for multi-channel data is "ch0 ch0 ch0 ..., ch1 ch1 ch1 ..."
 * @param indata      An array of 16-bit signed audio samples from mic.
 * @param refdata     An array of 16-bit signed audio samples sent to the speaker.
 * @param outdata     Returns near-end signal with echo removed. Format for multi-channel data is "ch0 ch0 ch0..., ch1 ch1 ch1 ..."
 * @return None
 *
 */
void aec_process(const aec_handle_t *handle, int16_t *indata, int16_t *refdata, int16_t *outdata);


/**
 * @brief Performs echo cancellation a frame, based on the audio sent to the speaker and frame from mic.
 *
 * @warning The indata, refdata and outdata must be 16-bit signed. please allocate memory by heap_caps_aligned_alloc().
 * 
 * @param inst        The instance of AEC. Format for multi-channel data is "ch0 ch0 ch0 ..., ch1 ch1 ch1 ..."
 * @param indata      An array of 16-bit signed audio samples from mic.
 * @param refdata     An array of 16-bit signed audio samples sent to the speaker.
 * @param outdata     Returns near-end signal with echo removed. Format for multi-channel data is "ch0 ch0 ch0..., ch1 ch1 ch1 ..."
 * @return None
 *
 */
void aec_linear_process(const aec_handle_t *handle, int16_t *indata, int16_t *refdata, int16_t *outdata);

/*
 * @brief Performs nonlinear post-processing for residual echo suppression.
 * Must be called immediately after aec_linear_process() 
 * @param handle The instance of AEC.
 * @param outdata Returns near-end signal with echo removed. Format for multi-channel data is "ch0 ch0 ch0..., ch1 ch1 ch1 ..."
 * @return The number of samples in the output data after NLP processing, or 0 if NLP is not applied.
 */
int aec_nlp_process(const aec_handle_t *handle, int16_t *outdata);

/**
 * @brief Set the Non-Linear Processing (NLP) level for the Acoustic Echo Canceller (AEC).
 *
 * This function configures the NLP level for the AEC module, which determines the aggressiveness
 * of the echo suppression. Higher levels may result in more aggressive echo cancellation but
 * could also affect the quality of the desired audio signal.
 *
 * @param handle Pointer to the AEC handle.
 * @param level The desired NLP level to set. This should be a value of type aec_nlp_level_t.
 *
 * @return The NLP level that was set, or an error code if the operation fails.
 */
aec_nlp_level_t aec_set_nlp_level(const aec_handle_t *handle, aec_nlp_level_t level);

/**
 * @brief Get frame size of AEC (the samples of one frame)
 * @param handle The instance of AEC.
 * @return Frame size
 */
int aec_get_chunksize(const aec_handle_t *handle);

/**
 * @brief Get AEC mode string 
 * 
 * @param aec_mode  The mode of AEC.
 * 
 * @return AEC mode string
 */
char * aec_get_mode_string(aec_mode_t aec_mode);

/**
 * @brief Get AEC config string, which is used for debug and log.
 * 
 * @param handle The instance of AEC.
 * 
 * @return AEC config string
 */
char * aec_get_config_string(const aec_handle_t *handle);

/**
 * @brief Free the AEC instance
 *
 * @param inst The instance of AEC.
 *
 * @return None
 *
 */
void aec_destroy(aec_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif //_ESP_AEC_H_
