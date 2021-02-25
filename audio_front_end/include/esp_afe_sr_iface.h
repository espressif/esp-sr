#pragma once
#include "stdint.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"

//AFE: Audio Front-End 
//SR:  Speech Recognition
//afe_sr/AFE_SR: the audio front-end for speech recognition

//Opaque AFE_SR data container
typedef struct esp_afe_sr_data_t esp_afe_sr_data_t;

//Set AFE_SR mode
typedef enum {
	SR_MODE_LOW_COST = 0,            //LOW_COST, low memory consumption and CPU loading
    SR_MODE_MEDIUM = 1,              //MEDIUM
	SR_MODE_HIGH_PERF = 2,           //HIGH_PERF
} afe_sr_mode_t;

/**
 * @brief Function to initialze a AFE_SR instance with a specified mode
 * 
 * @param mode              The mode of AFE_SR 
 * @param perferred_core    The perferred core to be pinned. 
 *                          If all task in AFE_SR can not run in real time by only one core, the another core would be used. 
 * @returns Handle to the AFE_SR data
 */
typedef esp_afe_sr_data_t* (*esp_afe_sr_iface_op_create_t)(afe_sr_mode_t mode, int perferred_core);

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
 * @brief Get the channel number of samples that need to be passed to the fetch function
 * 
 * @param afe The AFE_SR object to query
 * @return The amount of samples to feed the fetch function
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
 * @Warning  The input data should be arranged in the format of [CH0_0, CH1_0, ..., CHN_0, CH0_1, CH0_1, ..., CHN_1, ...].
 *           The last channel is reference signal or far-end signal.
 *
 * @param afe   The AFE_SR object to query
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
 * @return      The style of output, -1: noise, 0: speech, 1: wake word 1, 2: wake word 2, ...
 */
typedef int (*esp_afe_sr_iface_op_fetch_t)(esp_afe_sr_data_t *afe, int16_t* out);

/**
 * @brief Initial wakenet and wake words coefficient, or reset wakenet and wake words coefficient 
 *        when wakenet has been initialized.  
 *
 * @param afe          The AFE_SR object to query
 * @param wakenet      The pointer of wakenet
 * @param model_coeff  The coefficient of wake word model
 * @return             0: fail, 1: success
 */
typedef int (*esp_afe_sr_iface_op_set_wakenet_t)(esp_afe_sr_data_t *afe,
                                                  esp_wn_iface_t *wakenet, 
                                                  const model_coeff_getter_t *model_coeff);

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
    esp_afe_sr_iface_op_feed_t feed;
    esp_afe_sr_iface_op_fetch_t fetch;
    esp_afe_sr_iface_op_get_samp_chunksize_t get_samp_chunksize;
    esp_afe_sr_iface_op_get_channel_num_t get_channel_num;
    esp_afe_sr_iface_op_get_samp_rate_t get_samp_rate;
    esp_afe_sr_iface_op_set_wakenet_t  set_wakenet; 
    esp_afe_sr_iface_op_disable_wakenet_t disable_wakenet;
    esp_afe_sr_iface_op_enable_wakenet_t enable_wakenet;
    esp_afe_sr_iface_op_disable_aec_t disable_aec;
    esp_afe_sr_iface_op_enable_aec_t enable_aec;
    esp_afe_sr_iface_op_destroy_t destroy;
} esp_afe_sr_iface_t;
