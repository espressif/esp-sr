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
#ifndef _ESP_WEBRTC_H_
#define _ESP_WEBRTC_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_agc.h"
#include "esp_log.h"
#include "esp_ns.h"
#include "sr_ringbuf.h"
#include <stdint.h>

#include "esp_heap_caps.h"

typedef struct {
    void *ns_handle;
    void *agc_handle;
    int frame_size;
    int sample_rate;
    int16_t *buff;
    int16_t *out_data;
    sr_ringbuf_handle_t rb;
} webrtc_handle_t;

/**
 * @brief Creates an instance of webrtc.
 *
 * @warning frame_length can supports be 10 ms, 20 ms, 30 ms, 32 ms.
 *
 * @param frame_length_ms    The length of the audio processing
 * @param ns_mode            The mode of NS. -1 means NS is disabled. 0: Mild, 1: Medium, 2: Aggressive
 * @param agc_mode           The model of AGC
 * @param agc_gain           The gain of AGC. default is 9
 * @param agc_target_level   The target level of AGC. default is -3 dbfs
 * @param sample_rate        The sample rate of the audio.
 *
 * @return
 *         - NULL: Create failed
 *         - Others: The instance of webrtc
 */
webrtc_handle_t *webrtc_create(
    int frame_length_ms, int ns_mode, agc_mode_t agc_mode, int agc_gain, int agc_target_level, int sample_rate);

/**
 * @brief Feed samples of an audio stream to the webrtc and get the audio stream after Noise suppression.
 *
 * @param handle        The instance of NS.
 * @param in_data       An array of 16-bit signed audio samples.
 * @param out_size      The sample size of output data
 * @param enable_ns     Enable noise suppression
 * @param enable_agc    Enable automatic gain control
 *
 * @return data after noise suppression
 */
int16_t *webrtc_process(webrtc_handle_t *handle, int16_t *indata, int *size, bool enable_ns, bool enable_agc);

/**
 * @brief Free the webrtc instance
 *
 * @param handle The instance of webrtc.
 *
 * @return None
 *
 */
void webrtc_destroy(webrtc_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif //_ESP_NS_H_
