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

#ifdef __cplusplus
extern "C" {
#endif

#define USE_AEC_FFT                      // Not kiss_fft
#define AEC_SAMPLE_RATE     16000        // Only Support 16000Hz
#define AEC_FRAME_LENGTH_MS 32

typedef struct aec_handle_t aec_handle_t;
typedef enum {
    AEC_MODE_SR_LOW_COST = 0,     // Low Cost AEC fro speech recognition
    AEC_MODE_SR_HIGH_PERF = 1,    // High Perforamce AEC for speech recognition
    AEC_MODE_VOIP_LOW_COST = 3,   // Low Cost AEC for voice communication
    AEC_MODE_VOIP_HIGH_PERF = 4,  // High Perforamce AEC for voice communication
} aec_mode_t;

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
 * @brief Creates an instance to the AEC structure, same with aec_create().
 * 
 * @param filter_length     Number of filter, recommend to set 4. The larger the filter_length, the more resource consumption.
 * @param channel_num       The input microphone channel number
 * @param mode              The mode of AEC, recommend to set AEC_MODE_SR_LOW_COST
 * @return
 *         - NULL: Create failed
 *         - Others: The instance of AEC
 */
aec_handle_t *aec_pro_create(int filter_length, int channel_num, aec_mode_t mode);

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
void aec_process(const aec_handle_t *handel, int16_t *indata, int16_t *refdata, int16_t *outdata);

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
 * @brief Free the AEC instance
 *
 * @param inst The instance of AEC.
 *
 * @return None
 *
 */
void aec_destroy(aec_handle_t *handel);

#ifdef __cplusplus
}
#endif

#endif //_ESP_AEC_H_
