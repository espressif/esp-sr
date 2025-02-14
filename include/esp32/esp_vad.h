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
#ifndef _ESP_VAD_H_
#define _ESP_VAD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_RATE_HZ 16000   // Supports 32000, 16000, 8000
#define VAD_FRAME_LENGTH_MS 30 // Supports 10ms, 20ms, 30ms

/**
 * @brief Sets the VAD operating mode. A more aggressive (higher mode) VAD is more
 * restrictive in reporting speech. So If you want trigger more speech, please select lower mode.
 */
typedef enum {
    VAD_MODE_0 = 0, // Normal
    VAD_MODE_1,     // Aggressive
    VAD_MODE_2,     // Very Aggressive
    VAD_MODE_3,     // Very Very Aggressive
    VAD_MODE_4      // Very Very Very Aggressive
} vad_mode_t;

typedef enum {
    VAD_SILENCE = 0,
    VAD_SPEECH = 1,
} vad_state_t;

typedef struct vad_trigger_tag {
    vad_state_t state;
    unsigned int min_speech_len;
    unsigned int noise_len;
    unsigned int min_noise_len;
    unsigned int speech_len;
} vad_trigger_t;

#define vad_MAX_LEN INT32_MAX - 1
/**
 * @brief Allocate wakenet trigger
 *
 * @param min_speech_len  Minimum frame number of speech duration
 * @param min_noise_len   Minimum frame number of noise duration
 *
 * @return Trigger pointer
 **/
vad_trigger_t *vad_trigger_alloc(int min_speech_len, int min_noise_len);

/**
 * @brief Free wakenet trigger
 **/
void vad_trigger_free(vad_trigger_t *trigger);

/**
 * @brief Reset wakenet trigger
 **/
void vad_trigger_reset(vad_trigger_t *trigger);

/**
 * @brief detect activaty voice by trigger
 **/
vad_state_t vad_trigger_detect(vad_trigger_t *trigger, vad_state_t state);

typedef struct {
    vad_trigger_t *trigger;
    void *vad_inst;
    int sample_rate;
    int frame_size;
} vad_handle_with_trigger_t;

typedef vad_handle_with_trigger_t *vad_handle_t;

// typedef vad_handle_tag * vad_handle_t;

/**
 * @brief Creates an instance to the VAD structure.
 *
 * @param vad_mode          Sets the VAD operating mode.
 *
 * @return
 *         - NULL: Create failed
 *         - Others: The instance of VAD
 */
vad_handle_t vad_create(vad_mode_t vad_mode);

/**
 * @brief Creates an instance to the VAD structure.
 *
 * @param vad_mode          Sets the VAD operating mode.
 * @param sample_rate       Sample rate in Hz
 * @param one_frame_ms      Length of the audio chunksize, can be 10ms, 20ms, 30ms, default: 30.
 * @param min_speech_ms     Minimum speech duration, unit is ms
 * @param min_noise_ms      Minimum noise duration, unit is ms
 * @return
 *         - NULL: Create failed
 *         - Others: The instance of VAD
 */
vad_handle_t vad_create_with_param(
    vad_mode_t vad_mode, int sample_rate, int one_frame_ms, int min_speech_ms, int min_noise_ms);

/**
 * @brief Feed samples of an audio stream to the VAD and check if there is someone speaking.
 *
 * @param handle            The instance of VAD.
 * @param data              An array of 16-bit signed audio samples.
 * @param sample_rate_hz    The Sampling frequency (Hz) can be 32000, 16000, 8000, default: 16000.
 * @param one_frame_ms      The length of the audio processing can be 10ms, 20ms, 30ms, default: 30.
 * @return
 *         - VAD_SILENCE if no voice
 *         - VAD_SPEECH  if voice is detected
 *
 */
vad_state_t vad_process(vad_handle_t handle, int16_t *data, int sample_rate_hz, int one_frame_ms);

/**
 * @brief Feed samples of an audio stream to the VAD and check if there is someone speaking.
 *
 * @param handle            The instance of VAD.
 * @param data              An array of 16-bit signed audio samples.
 * @return
 *         - VAD_SILENCE if no voice
 *         - VAD_SPEECH  if voice is detected
 *
 */
vad_state_t vad_process_with_trigger(vad_handle_t handle, int16_t *data);

/**
 * @brief Reset trigger state as Silence
 *
 * @param handle            The instance of VAD.
 */
void vad_reset_trigger(vad_handle_t handle);

/**
 * @brief Free the VAD instance
 *
 * @param inst The instance of VAD.
 *
 * @return None
 *
 */
void vad_destroy(vad_handle_t inst);

/*
 * Programming Guide:
 *
 * @code{c}
 * vad_handle_t vad_inst = vad_create(VAD_MODE_3, SAMPLE_RATE_HZ, VAD_FRAME_LENGTH_MS);     // Creates an instance to
 * the VAD structure.
 *
 * while (1) {
 *    //Use buffer to receive the audio data from MIC.
 *    vad_state_t vad_state = vad_process(vad_inst, buffer);      // Feed samples to the VAD process and get the result.
 * }
 *
 * vad_destroy(vad_inst);   // Free the VAD instance at the end of whole VAD process
 *
 * @endcode
 */

#ifdef __cplusplus
}
#endif

#endif //_ESP_VAD_H_
