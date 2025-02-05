#pragma once
#include "esp_vad.h"
#include "stdint.h"
#include "dl_lib_convq_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opaque model data container
typedef struct model_iface_data_t model_iface_data_t;

// /**
//  * @brief The state of vad
//  */
// typedef enum {
//     VAD_NOISE = -1,  // Noise
//     VADNET_STATE_SILENCE = 0, // Silence
//     VAD_SPEECH = 1   // Speech
// } vad_state_t;

/**
 * @brief Easy function type to initialze a model instance with a detection mode
 * and specified model name
 *
 * @param model_name  The specified model name
 * @param mode        The voice activity detection mode
 * @param channel_num The number of input audio channels
 * @param min_speech_ms  The minimum duration of speech in ms to trigger vad
 * speech
 * @param min_noise_ms   The minimum duration of noise in ms to trigger vad
 * noise
 * @returns Handle to the model data
 */
typedef model_iface_data_t *(*esp_vadn_iface_op_create_t)(
    const void *model_name, vad_mode_t mode, int channel_num, int min_speech_ms, int min_noise_ms);

/**
 * @brief Get the amount of samples that need to be passed to the detect
 * function
 *
 * Every speech recognition model processes a certain number of samples at the
 * same time. This function can be used to query that amount. Note that the
 * returned amount is in 16-bit samples, not in bytes.
 *
 * @param model The model object to query
 * @return The amount of samples to feed the detect function
 */
typedef int (*esp_vadn_iface_op_get_samp_chunksize_t)(model_iface_data_t *model);

/**
 * @brief Get the channel number of samples that need to be passed to the detect
 * function
 *
 * Every speech recognition model processes a certain number of samples at the
 * same time. This function can be used to query that amount. Note that the
 * returned amount is in 16-bit samples, not in bytes.
 *
 * @param model The model object to query
 * @return The amount of samples to feed the detect function
 */
typedef int (*esp_vadn_iface_op_get_channel_num_t)(model_iface_data_t *model);

/**
 * @brief Get the sample rate of the samples to feed to the detect function
 *
 * @param model The model object to query
 * @return The sample rate, in hz
 */
typedef int (*esp_vadn_iface_op_get_samp_rate_t)(model_iface_data_t *model);

/**
 * @brief Set the detection threshold to manually abjust the probability
 *
 * @param model The model object to query
 * @param det_treshold The threshold to trigger wake words, the range of
 * det_threshold is 0.5~0.9999
 * @return 0: setting failed, 1: setting success
 */
typedef int (*esp_vadn_iface_op_set_det_threshold_t)(model_iface_data_t *model, float det_threshold);

/**
 * @brief Get the voice activity detection threshold
 *
 * @param model The model object to query
 * @returns the detection threshold
 */
typedef float (*esp_vadn_iface_op_get_det_threshold_t)(model_iface_data_t *model);

/**
 * @brief Feed samples of an audio stream to the vad model and detect whether is
 * voice.
 *
 * @param model The model object to query
 * @param samples An array of 16-bit signed audio samples. The array size used
 * can be queried by the get_samp_chunksize function.
 * @return The index of wake words, return 0 if no wake word is detected, else
 * the index of the wake words.
 */
typedef vad_state_t (*esp_vadn_iface_op_detect_t)(model_iface_data_t *model, int16_t *samples);

/**
 * @brief Feed MFCC of an audio stream to the vad model and detect whether is
 * voice.
 *
 * @param model The model object to query
 * @param cq An array of 16-bit MFCC.
 * @return The index of wake words, return 0 if no wake word is detected, else
 * the index of the wake words.
 */
typedef vad_state_t (*esp_vadn_iface_op_detect_mfcc_t)(model_iface_data_t *model, dl_convq_queue_t *cq);

/**
 * @brief Get MFCC of an audio stream
 *
 * @param model The model object to query
 * @return MFCC data
 */
typedef dl_convq_queue_t* (*esp_vadn_iface_op_get_mfcc_data_t)(model_iface_data_t *model);

/**
 * @brief Get the triggered channel index. Channel index starts from zero
 *
 * @param model The model object to query
 * @return The channel index
 */
typedef int (*esp_vadn_iface_op_get_triggered_channel_t)(model_iface_data_t *model);

/**
 * @brief Clean all states of model
 *
 * @param model The model object to query
 */
typedef void (*esp_vadn_iface_op_clean_t)(model_iface_data_t *model);

/**
 * @brief Destroy a model object
 *
 * @param model Model object to destroy
 */
typedef void (*esp_vadn_iface_op_destroy_t)(model_iface_data_t *model);

/**
 * This structure contains the functions used to do operations on a voice
 * activity detection model.
 */
typedef struct {
    esp_vadn_iface_op_create_t create;
    esp_vadn_iface_op_get_samp_chunksize_t get_samp_chunksize;
    esp_vadn_iface_op_get_channel_num_t get_channel_num;
    esp_vadn_iface_op_get_samp_rate_t get_samp_rate;
    esp_vadn_iface_op_set_det_threshold_t set_det_threshold;
    esp_vadn_iface_op_get_det_threshold_t get_det_threshold;
    esp_vadn_iface_op_get_triggered_channel_t get_triggered_channel;
    esp_vadn_iface_op_detect_t detect;
    esp_vadn_iface_op_detect_mfcc_t detect_mfcc;
    esp_vadn_iface_op_get_mfcc_data_t get_mfcc_data;
    esp_vadn_iface_op_clean_t clean;
    esp_vadn_iface_op_destroy_t destroy;
} esp_vadn_iface_t;

#ifdef __cplusplus
}
#endif