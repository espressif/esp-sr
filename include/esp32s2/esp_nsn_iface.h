#pragma once
#include "stdint.h"

//Opaque model data container
typedef struct esp_nsn_data_t esp_nsn_data_t;


/**
 * @brief Easy function type to initialize a model instance
 *
 * @param model_name The name of the model instance
 * @returns Handle to the model data
 */
typedef esp_nsn_data_t* (*esp_nsn_iface_op_create_t)(char *model_name);

/**
 * @brief Get the amount of samples that need to be passed to the process function
 *
 * Every noise suppression model processes a certain number of samples at the same time. This function
 * can be used to query that amount. Note that the returned amount is in 16-bit samples, not in bytes.
 *
 * @param model The model object to query
 * @return The amount of samples to feed the process function
 */
typedef int (*esp_nsn_iface_op_get_samp_chunksize_t)(esp_nsn_data_t *model);

/**
 * @brief Feed samples of an audio stream to the noise suppression model and get data after process.
 *
 *
 * @param model The model object to query
 * @param in_data An array of 16-bit signed audio samples. The array size used can be queried by the 
 *        get_samp_chunksize function.
 * @param out_data An array of 16-bit signed audio samples after process.
 * @return The state of return.
 */
typedef int (*esp_nsn_iface_op_process_t)(esp_nsn_data_t *model, int16_t *in_data, int16_t *out_data);

/**
 * @brief Get the sample rate of the samples to feed to the process function
 *
 * @param model The model object to query
 * @return The sample rate, in hz
 */
typedef int (*esp_nsn_iface_op_get_samp_rate_t)(esp_nsn_data_t *model);

/**
 * @brief Destroy a noise suppression model
 *
 * @param model Model object to destroy
 */
typedef void (*esp_nsn_iface_op_destroy_t)(esp_nsn_data_t *model);

/**
 * @brief Easy function type to initialize a multi-channel model instance
 *
 * All channels share one neural network: the ERB mask is estimated once from
 * the reference channel and applied to every channel, so the extra cost per
 * additional channel is only windowing/FFT/mask/IFFT/OLA.
 *
 * @param model_name The name of the model instance
 * @param channel_num Number of channels to process (>= 1)
 * @param ref_channel Index of the reference channel used for mask estimation
 *                    (must be < channel_num)
 * @returns Handle to the model data
 */
typedef esp_nsn_data_t* (*esp_nsn_iface_op_create_mc_t)(char *model_name, int channel_num, int ref_channel);

/**
 * @brief Feed multi-channel samples to the noise suppression model.
 *
 * The mask estimated from the reference channel is applied to all channels.
 *
 * @param model The model object to query
 * @param in_data Array of per-channel input pointers (channel_num entries).
 *        Each entry points to get_samp_chunksize 16-bit signed samples.
 * @param out_data Array of per-channel output pointers (channel_num entries).
 * @return The state of return.
 */
typedef int (*esp_nsn_iface_op_process_mc_t)(esp_nsn_data_t *model, int16_t **in_data, int16_t **out_data);


/**
 * This structure contains the functions used to do operations on a wake word detection model.
 */
typedef struct {
    esp_nsn_iface_op_create_t create;
    esp_nsn_iface_op_get_samp_chunksize_t get_samp_chunksize;
    esp_nsn_iface_op_process_t process;
    esp_nsn_iface_op_get_samp_rate_t get_samp_rate;
    esp_nsn_iface_op_destroy_t destroy;
    // Multi-channel (shared-mask) operations; NULL when the model does not support them
    esp_nsn_iface_op_create_mc_t create_mc;
    esp_nsn_iface_op_process_mc_t process_mc;
} esp_nsn_iface_t;
