#pragma once
#include "esp_mn_iface.h"

//Contains declarations of all available speech recognion models. Pair this up with the right coefficients and you have a model that can recognize
//a specific phrase or word.
extern const esp_mn_iface_t esp_sr_multinet1_single_quantized_en;
extern const esp_mn_iface_t esp_sr_multinet3_single_quantized_en;
extern const esp_mn_iface_t esp_sr_multinet2_single_quantized_cn;
extern const esp_mn_iface_t esp_sr_multinet3_single_quantized_cn;
extern const esp_mn_iface_t esp_sr_multinet4_single_quantized_cn;
extern const esp_mn_iface_t esp_sr_multinet3_continuous_quantized_cn;
extern const esp_mn_iface_t esp_sr_multinet5_quantized;
extern const esp_mn_iface_t esp_sr_multinet5_quantized8;

/*
 Configure wake word to use based on what's selected in menuconfig.
*/
#if defined CONFIG_USE_MULTINET

#if CONFIG_SR_MN_CN_MULTINET2_SINGLE_RECOGNITION
#include "multinet2_ch.h"
#define MULTINET_MODEL_CN esp_sr_multinet2_single_quantized_cn
#define MULTINET_COEFF_CN get_coeff_multinet2_ch
#define MULTINET_MODEL esp_sr_multinet2_single_quantized_cn
#define MULTINET_COEFF get_coeff_multinet2_ch
#elif CONFIG_SR_MN_CN_MULTINET4_SINGLE_RECOGNITION
#define MULTINET_MODEL_CN esp_sr_multinet4_single_quantized_cn
#define MULTINET_COEFF_CN "mn4cn"
#define MULTINET_MODEL esp_sr_multinet4_single_quantized_cn
#define MULTINET_COEFF "mn4cn"
#elif CONFIG_SR_MN_CN_MULTINET4_5_SINGLE_RECOGNITION
#define MULTINET_MODEL_CN esp_sr_multinet4_single_quantized_cn
#define MULTINET_COEFF_CN "mn4_5cn"
#define MULTINET_MODEL esp_sr_multinet4_single_quantized_cn
#define MULTINET_COEFF "mn4_5cn"
#endif

#if CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION
#define MULTINET_MODEL_EN esp_sr_multinet5_quantized
#define MULTINET_COEFF_EN "mn5en"
#ifndef MULTINET_MODEL
#define MULTINET_MODEL esp_sr_multinet5_quantized
#define MULTINET_COEFF "mn5en"
#endif
#elif CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION_QUANT8
#define MULTINET_MODEL_EN esp_sr_multinet5_quantized8
#define MULTINET_COEFF_EN "mn5q8en"
#ifndef MULTINET_MODEL
#define MULTINET_MODEL esp_sr_multinet5_quantized8
#define MULTINET_COEFF "mn5q8en"
#endif
#endif

#else
#define MULTINET_MODEL "NULL"
#define MULTINET_COEFF "NULL"
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
