#pragma once
#include "esp_afe_config.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifdef __cplusplus
extern "C" {
#endif

// AFE: Audio Front-End
// SR:  Speech Recognition
// afe_sr/AFE_SR: the audio front-end for speech recognition

// Opaque AFE_SR data container
typedef struct esp_afe_sr_data_t esp_afe_sr_data_t;

/**
 * @brief The state of vad
 */
typedef enum {
    AFE_VAD_SILENCE = 0, // Deprecated, please use vad_state_t, noise or silence
    AFE_VAD_SPEECH = 1   // Deprecated, please use vad_state_t, speech
} afe_vad_state_t;

/**
 * @brief The result of fetch function
 */
typedef struct afe_fetch_result_t {
    int16_t *data;      // the target channel data of audio.
    int data_size;      // the size of data. The unit is byte.
    int16_t *vad_cache; // the cache data of vad. It's only valid when vad_cache_size > 0. It is used to complete the
                        // audio that was truncated.
    int vad_cache_size; // the size of vad_cache. The unit is byte.
    float data_volume;  // the volume of input audio, the unit is decibel(dB). This value is calculated before agc.
                        // (note: invalid in vc). if enable wakenet, the window length is the receptive fields of
                        // wakenet(about 1.5s), otherwise is the frame length.
    wakenet_state_t wakeup_state; // the value is wakenet_state_t
    int wake_word_index;          // if the wake word is detected. It will store the wake word index which start from 1.
    int wakenet_model_index; // if there are multiple wakenets, this value identifies which model be wakes up. Index
                             // start from 1.
    vad_state_t vad_state;   // the value is afe_vad_state_t
    int trigger_channel_id;  // the channel index of output
    int wake_word_length;    // the length of wake word. The unit is the number of samples.
    int ret_value;           // the return state of fetch function
    int16_t *raw_data;       // the multi-channel output data of audio.
    int raw_data_channels;   // the channel number of raw data
    float ringbuff_free_pct; // the percent of ringbuff free size. if the value is larger than 0.5, it means the ringbuff is buzy. 
    void *reserved;          // reserved for future use
} afe_fetch_result_t;

/**
 * @brief Function to initialze a AFE_SR instance
 *
 * @param afe_config        The config of AFE_SR
 * @returns Handle to the AFE_SR data
 */
typedef esp_afe_sr_data_t *(*esp_afe_sr_iface_op_create_from_config_t)(afe_config_t *afe_config);

/**
 * @brief Get the amount of each channel samples per frame that need to be passed to the function
 *
 * Every speech enhancement AFE_SR processes a certain number of samples at the same time. This function
 * can be used to query that amount. Note that the returned amount is in 16-bit samples, not in bytes.
 *
 * @param afe The AFE_SR object to query
 * @return The amount of samples to feed the fetch function
 */
typedef int (*esp_afe_sr_iface_op_get_samp_chunksize_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Get the channel number
 *
 * @param afe   The AFE_SR object to query
 * @return      The amount of total channels
 */
typedef int (*esp_afe_sr_iface_op_get_channel_num_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Get the sample rate of the samples to feed to the function
 *
 * @param afe   The AFE_SR object to query
 * @return      The sample rate, in hz
 */
typedef int (*esp_afe_sr_iface_op_get_samp_rate_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Feed samples of an audio stream to the AFE_SR
 *
 * @Warning  The input data should be arranged in the format of channel interleaving.
 *           The last channel is reference signal if it has reference data.
 *
 * @param afe   The AFE_SR object to query
 *
 * @param in    The input microphone signal, only support signed 16-bit @ 16 KHZ. The frame size can be queried by the
 *              `get_feed_chunksize`.
 * @return      The size of input
 */
typedef int (*esp_afe_sr_iface_op_feed_t)(esp_afe_sr_data_t *afe, const int16_t *in);

/**
 * @brief fetch enhanced samples of an audio stream from the AFE_SR
 *
 * @Warning  The output is single channel data, no matter how many channels the input is.
 *           Timeout is 2000 ms. If you want to adjust timeout, please refer to the definition of `fetch_with_delay`.
 *
 * @param afe   The AFE_SR object to query
 * @return      The result of output, please refer to the definition of `afe_fetch_result_t`. (The frame size of output
 * audio can be queried by the `get_fetch_chunksize`.)
 */
typedef afe_fetch_result_t *(*esp_afe_sr_iface_op_fetch_t)(esp_afe_sr_data_t *afe);

/**
 * @brief fetch enhanced samples of an audio stream from the AFE_SR, same with the function `fetch`
 *
 * @Warning  The output is single channel data, no matter how many channels the input is.
 *
 * @param afe            The AFE_SR object to query
 * @param ticks_to_wait  The timeout value, in ticks, to wait for the fetch result.
 * @return      The result of output, please refer to the definition of `afe_fetch_result_t`. (The frame size of output
 * audio can be queried by the `get_fetch_chunksize`.)
 */
typedef afe_fetch_result_t *(*esp_afe_sr_iface_op_fetch_with_delay_t)(esp_afe_sr_data_t *afe, TickType_t ticks_to_wait);

/**
 * @brief reset ringbuf of AFE.
 *
 * @param afe          The AFE_SR object to query
 * @return             -1: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_reset_buffer_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Set wakenet detection threshold 
 * 
 * @param afe           The AFE_SR object to query
 * @param index         The wakenet index, just support 1: wakenet1 or  2: wakenet2
 * @param threshold     The wakenet detection threshold, the value is between 0.4 and 0.9999.
 * @return             -1: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_set_wakenet_threshold_t)(esp_afe_sr_data_t *afe, int index, float threshold);

/**
 * @brief Reset wakenet detection threshold to inital state
 * 
 * @param afe           The AFE_SR object to query
 * @param index         The wakenet index, just support 1: wakenet1 or  2: wakenet2
 * @return             -1: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_reset_wakenet_threshold_t)(esp_afe_sr_data_t *afe, int index);

/**
 * @brief Reset one function/module/algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             -1: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_reset_op_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Disable one function/module/algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             -1: fail, 0: disabled, 1: enabled
 */
typedef int (*esp_afe_sr_iface_op_disable_func_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Enable one function/module/algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             -1: fail, 0: disabled, 1: enabled
 */
typedef int (*esp_afe_sr_iface_op_enable_func_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Print all functions/modules/algorithms pipeline.
 *       The pipeline is the order of the functions/modules/algorithms.
 *       The format like this: [input] -> |AEC(VOIP_HIGH_PERF)| -> |WakeNet(wn9_hilexin)| -> [output]
 *
 * @param afe          The AFE_SR object to query
 */
typedef void (*esp_afe_sr_iface_op_print_pipeline_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Destroy a AFE_SR instance
 *
 * @param afe         AFE_SR object to destroy
 */
typedef void (*esp_afe_sr_iface_op_destroy_t)(esp_afe_sr_data_t *afe);

/**
 * This structure contains the functions used to do operations on a AFE_SR.
 */
typedef struct {
    esp_afe_sr_iface_op_create_from_config_t create_from_config;
    esp_afe_sr_iface_op_feed_t feed;
    esp_afe_sr_iface_op_fetch_t fetch;
    esp_afe_sr_iface_op_fetch_with_delay_t fetch_with_delay;
    esp_afe_sr_iface_op_reset_buffer_t reset_buffer;
    esp_afe_sr_iface_op_get_samp_chunksize_t get_feed_chunksize;
    esp_afe_sr_iface_op_get_samp_chunksize_t get_fetch_chunksize;
    esp_afe_sr_iface_op_get_channel_num_t get_channel_num; // same with get_feed_channel_num
    esp_afe_sr_iface_op_get_channel_num_t get_feed_channel_num;
    esp_afe_sr_iface_op_get_channel_num_t get_fetch_channel_num;
    esp_afe_sr_iface_op_get_samp_rate_t get_samp_rate;
    esp_afe_sr_iface_op_set_wakenet_threshold_t set_wakenet_threshold;
    esp_afe_sr_iface_op_reset_wakenet_threshold_t reset_wakenet_threshold;
    esp_afe_sr_iface_op_disable_func_t disable_wakenet;
    esp_afe_sr_iface_op_enable_func_t enable_wakenet;
    esp_afe_sr_iface_op_disable_func_t disable_aec;
    esp_afe_sr_iface_op_enable_func_t enable_aec;
    esp_afe_sr_iface_op_disable_func_t disable_se;
    esp_afe_sr_iface_op_enable_func_t enable_se;
    esp_afe_sr_iface_op_disable_func_t disable_vad;
    esp_afe_sr_iface_op_enable_func_t enable_vad;
    esp_afe_sr_iface_op_reset_op_t reset_vad;
    esp_afe_sr_iface_op_disable_func_t disable_ns;
    esp_afe_sr_iface_op_enable_func_t enable_ns;
    esp_afe_sr_iface_op_disable_func_t disable_agc;
    esp_afe_sr_iface_op_enable_func_t enable_agc;
    esp_afe_sr_iface_op_print_pipeline_t print_pipeline;
    esp_afe_sr_iface_op_destroy_t destroy;
} esp_afe_sr_iface_t;

// struct is used to store the AFE handle and data for the AFE task
typedef struct {
    esp_afe_sr_data_t *afe_data;
    esp_afe_sr_iface_t *afe_handle;
    TaskHandle_t feed_task;
    TaskHandle_t fetch_task;
} afe_task_into_t;

#ifdef __cplusplus
}
#endif
