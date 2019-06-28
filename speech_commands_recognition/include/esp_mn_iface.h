#pragma once
#include "stdint.h"
#include "esp_err.h"
#include "dl_lib_coefgetter_if.h"
#include "esp_wn_iface.h"
// //Opaque model data container
// typedef struct model_iface_data_t model_iface_data_t;

/**
 * @brief Initialze a model instance with specified model coefficient.
 *
 * @param coeff       The wakenet model coefficient.
 * @returns Handle to the model data.
 */
typedef model_iface_data_t* (*esp_mn_iface_op_create_t)(const model_coeff_getter_t *coeff);


/**
 * @brief Callback function type to fetch the amount of samples that need to be passed to the detect function
 *
 * Every speech recognition model processes a certain number of samples at the same time. This function
 * can be used to query that amount. Note that the returned amount is in 16-bit samples, not in bytes.
 *
 * @param model       The model object to query
 * @return The amount of samples to feed the detect function
 */
typedef int (*esp_mn_iface_op_get_samp_chunksize_t)(model_iface_data_t *model);


/**
 * @brief Get the sample rate of the samples to feed to the detect function
 *
 * @param model       The model object to query
 * @return The sample rate, in hz
 */
typedef int (*esp_mn_iface_op_get_samp_rate_t)(model_iface_data_t *model);

/**
 * @brief Add a command word and set its command ID.
 *
 * @param model           The model object to query.
 * @param command_id      The command id of this word.
 * @param phrase_spelling The chinese command word spelled using prescribed rules.
 * @param phrase_str      Auxiliary information of phrase. 
 * @return 1: setting success. 0: setting failure
 */
typedef int (*esp_mn_iface_op_add_speech_commands_t)(model_iface_data_t *model, 
                                                     int command_id,
                                                     char *phrase_spelling, 
                                                     char *phrase_str);

/**
 * @brief Feed samples of an audio stream to the speech recognition model and detect if there is a speech command found.
 *
 * @param model       The model object to query.
 * @param samples     An array of 16-bit signed audio samples. The array size used can be queried by the 
 *                    get_samp_chunksize function.
 * @return The command id, return 0 if no command word is detected,
 */
typedef int (*esp_mn_iface_op_detect_t)(model_iface_data_t *model,
                                        int16_t *samples);


/**
 * @brief Destroy a voiceprint recognition model
 *
 * @param model       The Model object to destroy
 */
typedef void (*esp_mn_iface_op_destroy_t)(model_iface_data_t *model);

typedef struct {
    esp_mn_iface_op_create_t create;
    esp_mn_iface_op_get_samp_rate_t get_samp_rate;
    esp_mn_iface_op_get_samp_chunksize_t get_samp_chunksize;
    esp_mn_iface_op_add_speech_commands_t add_speech_commands;
    esp_mn_iface_op_detect_t detect; 
    esp_mn_iface_op_destroy_t destroy;
} esp_mn_iface_t;
