#pragma once
#include "stdint.h"
#if CONFIG_AFE_INTERFACE_V1
#include "esp_afe_config.h"
#else
#include "esp_afe_config.h"
#endif

//AFE: Audio Front-End 
//SR:  Speech Recognition
//afe_sr/AFE_SR: the audio front-end for speech recognition

//Opaque AFE_SR data container
typedef struct esp_afe_sr_data_t esp_afe_sr_data_t;

// the output state of fetch function
typedef enum {
    AFE_FETCH_ERROR = -3,                   // fetch empty data, retry it
    AFE_FETCH_CHANNEL_VERIFIED = -2,        // wwe state: output channel is verified
    AFE_FETCH_NOISE = -1,                   // vad state: noise or silence
    AFE_FETCH_SPEECH = 0,                   // vad state: speech
    AFE_FETCH_WWE_DETECTED = 1              // wwe state: wake word is detected
} afe_fetch_mode_t;

/**
 * @brief Function to initialze a AFE_SR instance with a specified mode
 * 
 * @param mode              The mode of AFE_SR 
 * @param perferred_core    The perferred core to be pinned. 
 *                          If all task in AFE_SR can not run in real time by only one core, the another core would be used. 
 * @returns Handle to the AFE_SR data
 */
typedef esp_afe_sr_data_t* (*esp_afe_sr_iface_op_create_t)(afe_sr_mode_t mode, int perferred_cor);

/**
 * @brief Function to initialze a AFE_SR instance
 * 
 * @param afe_config        The config of AFE_SR
 * @returns Handle to the AFE_SR data
 */
typedef esp_afe_sr_data_t* (*esp_afe_sr_iface_op_create_from_config_t)(afe_config_t *afe_config);

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
 * @brief Get the total channel number which be config
 * 
 * @param afe The AFE_SR object to query
 * @return The amount of total channels
 */
typedef int (*esp_afe_sr_iface_op_get_total_channel_num_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Get the mic channel number which be config
 * 
 * @param afe The AFE_SR object to query
 * @return The amount of mic channels
 */
typedef int (*esp_afe_sr_iface_op_get_channel_num_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Get the sample rate of the samples to feed to the function
 *
 * @param afe The AFE_SR object to query
 * @return The sample rate, in hz
 */
typedef int (*esp_afe_sr_iface_op_get_samp_rate_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Feed samples of an audio stream to the AFE_SR
 *
 * @Warning  The input data should be arranged in the format of [CH0_0, CH1_0, ..., CHN_0, CH0_1, CH1_1, ..., CHN_1, ...].
 *           The last channel is reference signal or far-end signal.
 *
 * @param afe   The AFE_SR object to queryq
 * 
 * @param in    The input microphone signal, only support signed 16-bit @ 16 KHZ. The frame size can be queried by the 
 *              `get_samp_chunksize`. The channel number can be queried `get_channel_num`.
 * @return      The size of input
 */
typedef int (*esp_afe_sr_iface_op_feed_t)(esp_afe_sr_data_t *afe, const int16_t* in);

/**
 * @brief fetch enhanced samples of an audio stream from the AFE_SR
 *
 * @Warning  The output is single channel data, no matter how many channels the input is.
 *
 * @param afe   The AFE_SR object to query
 * @param out   The output enhanced signal. The frame size can be queried by the `get_samp_chunksize`.
 * @return      The state of output, please refer to the definition of `afe_fetch_mode_t`
 */
typedef afe_fetch_mode_t (*esp_afe_sr_iface_op_fetch_t)(esp_afe_sr_data_t *afe, int16_t* out);

/**
 * @brief Initial wakenet and wake words coefficient, or reset wakenet and wake words coefficient 
 *        when wakenet has been initialized.  
 *
 * @param afe                The AFE_SR object to query
 * @param wakenet_word       The wakenet word, should be DEFAULT_WAKE_WORD or EXTRA_WAKE_WORD
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_set_wakenet_t)(esp_afe_sr_data_t *afe, esp_wn_word_t wakenet_word);

/**
 * @brief Disable wakenet model.
 *
 * @param afe          The AFE_SR object to query
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_disable_wakenet_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Enable wakenet model.
 *
 * @param afe          The AFE_SR object to query
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_enable_wakenet_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Disable AEC algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_disable_aec_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Enable AEC algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_enable_aec_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Disable SE algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_disable_se_t)(esp_afe_sr_data_t *afe);

/**
 * @brief Enable SE algorithm.
 *
 * @param afe          The AFE_SR object to query
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_enable_se_t)(esp_afe_sr_data_t *afe);

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
    esp_afe_sr_iface_op_create_t create;
    esp_afe_sr_iface_op_create_from_config_t create_from_config;
    esp_afe_sr_iface_op_feed_t feed;
    esp_afe_sr_iface_op_fetch_t fetch;
    esp_afe_sr_iface_op_get_samp_chunksize_t get_feed_chunksize;
    esp_afe_sr_iface_op_get_samp_chunksize_t get_fetch_chunksize;
    esp_afe_sr_iface_op_get_total_channel_num_t get_total_channel_num;
    esp_afe_sr_iface_op_get_channel_num_t get_channel_num;
    esp_afe_sr_iface_op_get_samp_rate_t get_samp_rate;
    esp_afe_sr_iface_op_set_wakenet_t  set_wakenet; 
    esp_afe_sr_iface_op_disable_wakenet_t disable_wakenet;
    esp_afe_sr_iface_op_enable_wakenet_t enable_wakenet;
    esp_afe_sr_iface_op_disable_aec_t disable_aec;
    esp_afe_sr_iface_op_enable_aec_t enable_aec;
    esp_afe_sr_iface_op_disable_se_t disable_se;
    esp_afe_sr_iface_op_enable_se_t enable_se;
    esp_afe_sr_iface_op_destroy_t destroy;
} esp_afe_sr_iface_t;
