
#ifndef _ESP_AFE_AEC_H_
#define _ESP_AFE_AEC_H_

#include "esp_aec.h"
#include "esp_afe_config.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    aec_handle_t *handle;
    aec_mode_t mode;
    afe_pcm_config_t pcm_config;
    int frame_size;
    int16_t *data;
} afe_aec_handle_t;

/**
 * @brief Creates an instance to the AEC structure.
 *
 * @warning Currently only support 1 microphone channel and 1 playback channe.
 * If input has multiple microphone channels and playback channels, just the first microphone channel and playback
 * channel will be selected.
 *
 * The input format, same as afe config:
 * M to represent the microphone channel
 * R to represent the playback reference channel
 * N to represent an unknown or unused channel
 *
 * For example, input_format="MMNR" indicates that the input data consists of four channels,
 * which are the microphone channel, the microphone channel, an unused channel, and the playback channel
 *
 * @param input_format     The input format
 * @param filter_length    The length of filter. The larger the filter, the higher the CPU loading.
 *                         Recommended filter_length = 4 for esp32s3 and esp32p4. Recommended filter_length = 2 for
 * esp32c5.
 * @param type             The type of afe, AFE_TYPE_SR or AFE_TYPE_VC
 * @param mode             The mode of afe, AFE_MODE_LOW_COST or AFE_MODE_HIGH_PERF
 *
 * @return afe_config_t*  The default config of afe
 */
afe_aec_handle_t *afe_aec_create(const char *input_format, int filter_length, afe_type_t type, afe_mode_t mode);

/**
 * @brief Performs echo cancellation a frame, based on the audio sent to the speaker and frame from mic.
 *
 * @param inst        The instance of AEC.
 * @param indata      Input audio data, format is define by input_format.
 * @param outdata     Near-end signal with echo removed.  outdata must be 16-bit aligned.
 *                    please use heap_caps_aligned_calloc(16, n, size, caps) to allocate an aligned chunk of memory

 * @return The bytes of outdata.
 */
size_t afe_aec_process(afe_aec_handle_t *handel, const int16_t *indata, int16_t *outdata);

/**
 * @brief Get frame size of AEC (the samples of one frame)
 * @param handle The instance of AEC.
 * @return Frame size
 */
int afe_aec_get_chunksize(afe_aec_handle_t *handle);

/**
 * @brief Free the AEC instance
 *
 * @param inst The instance of AEC.
 *
 * @return None
 *
 */
void afe_aec_destroy(afe_aec_handle_t *handel);

#ifdef __cplusplus
}
#endif

#endif //_ESP_AEC_H_
