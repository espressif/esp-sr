#pragma once
#include "esp_wn_iface.h"

#ifdef __cplusplus
extern "C" {
#endif

// The prefix of wakenet model name is used to filter all wakenet from availabel models.
#define ESP_WN_PREFIX "wn"

/**
 * @brief Get the wakenet handle from model name
 *
 * @param model_name   The name of model
 * @returns The handle of wakenet
 */
const esp_wn_iface_t *esp_wn_handle_from_name(const char *model_name);

/**
 * @brief Get the wake word name from model name
 *
 * @param model_name   The name of model
 * @returns The wake word name, like "alexa","hilexin","xiaoaitongxue"
 */
char *esp_wn_wakeword_from_name(const char *model_name);

#ifdef __cplusplus
}
#endif

/*

static const sr_model_iface_t *model = esp_wn_handle_from_name(model_name);

//Initialize wakeNet model data
static model_iface_data_t *model_data=model->create(model_name, DET_MODE_90);

//Set parameters of buffer
int audio_chunksize=model->get_samp_chunksize(model_data);
int frequency = model->get_samp_rate(model_data);
int16_t *buffer=malloc(audio_chunksize*sizeof(int16_t));

//Detect
int r=model->detect(model_data, buffer);
if (r>0) {
    printf("Detection triggered output %d.\n",  r);
}

//Destroy model
model->destroy(model_data)

*/
