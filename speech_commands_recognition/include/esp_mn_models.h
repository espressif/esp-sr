#pragma once
#include "esp_mn_iface.h"

//Contains declarations of all available speech recognion models. Pair this up with the right coefficients and you have a model that can recognize
//a specific phrase or word.
extern const esp_mn_iface_t esp_sr_multinet1_quantized;

/*
 Configure network to use based on what's selected in menuconfig.
*/
#if CONFIG_SR_MN1_MODEL_QUANT
#define MULTINET_MODEL esp_sr_multinet1_quantized
#else
#error No valid neural network model selected.
#endif


/*
 Configure wake word to use based on what's selected in menuconfig.
*/
#if CONFIG_SR_MN1_CHINESE_QUANT
#include "multinet1_ch.h"
#define MULTINET_COEFF get_coeff_multinet1_ch
#else
#error No valid wake word selected.
#endif


/* example

static const esp_mn_iface_t *multinet = &MULTINET_MODEL;

//Initialize MultiNet model data
model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF);
add_speech_commands(multinet, model_data);

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
