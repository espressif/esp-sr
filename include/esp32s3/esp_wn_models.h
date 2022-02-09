#pragma once
#include "esp_wn_iface.h"

//Contains declarations of all available speech recognion models. Pair this up with the right coefficients and you have a model that can recognize
//a specific phrase or word.

typedef enum {
    FIRST_WAKE_WORD = 0,
    SECOND_WAKE_WORD,
} esp_wn_word_t;

typedef enum {
    NO_WAKE_WORD = -1,
    CUSTOMIZED_WORD_WN7 = 0,
    XIAOAITONGXUE_WN7 = 1,
    XIAOAITONGXUE_WN7Q8,
    ALEXA_WN7,
    ALEXA_WN7Q8,
    ALEXA_WN8,
    HILEXIN_WN7,
    HILEXIN_WN7Q8,
    HILEXIN_WN8,
    HIESP_WN8,
    HIESP_WN8Q8,
} esp_wn_model_t;

extern const esp_wn_iface_t esp_sr_wakenet5_quantized;
extern const esp_wn_iface_t esp_sr_wakenet7_quantized;
extern const esp_wn_iface_t esp_sr_wakenet7_quantized8;
extern const esp_wn_iface_t esp_sr_wakenet8_quantized;
extern const esp_wn_iface_t esp_sr_wakenet8_quantized8;

/*
 Configure wake word to use based on what's selected in menuconfig.
*/
#if defined CONFIG_USE_WAKENET
#if CONFIG_SR_WN_WN5_HILEXIN_FIRST || CONFIG_SR_WN_WN5_HILEXIN_SECOND
#include "hilexin_wn5.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_hilexin_wn5

#elif CONFIG_SR_WN_WN5X2_HILEXIN_FIRST || CONFIG_SR_WN_WN5X2_HILEXIN_SECOND
#include "hilexin_wn5X2.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_hilexin_wn5X2

#elif CONFIG_SR_WN_WN5X3_HILEXIN_FIRST || CONFIG_SR_WN_WN5X3_HILEXIN_SECOND
#include "hilexin_wn5X3.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_hilexin_wn5X3

#elif CONFIG_SR_WN_WN5_NIHAOXIAOZHI_FIRST || CONFIG_SR_WN_WN5_NIHAOXIAOZHI_SECOND
#include "nihaoxiaozhi_wn5.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_nihaoxiaozhi_wn5

#elif CONFIG_SR_WN_WN5X2_NIHAOXIAOZHI_FIRST || CONFIG_SR_WN_WN5X2_NIHAOXIAOZHI_SECOND
#include "nihaoxiaozhi_wn5X2.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_nihaoxiaozhi_wn5X2

#elif CONFIG_SR_WN_WN5X3_NIHAOXIAOZHI_FIRST || CONFIG_SR_WN_WN5X3_NIHAOXIAOZHI_SECOND
#include "nihaoxiaozhi_wn5X3.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_nihaoxiaozhi_wn5X3

#elif CONFIG_SR_WN_WN5X3_NIHAOXIAOXIN_FIRST || CONFIG_SR_WN_WN5X3_NIHAOXIAOXIN_SECOND
#include "nihaoxiaoxin_wn5X3.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_nihaoxiaoxin_wn5X3

#elif CONFIG_SR_WN_WN5X3_HIJESON_FIRST || CONFIG_SR_WN_WN5X3_HIJESON_SECOND
#include "hijeson_wn5X3.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_hijeson_wn5X3

#elif CONFIG_SR_WN_WN5_CUSTOMIZED_WORD_FIRST || CONFIG_SR_WN_WN5_CUSTOMIZED_WORD_SECOND
#include "customized_word_wn5.h"
#define WAKENET_MODEL esp_sr_wakenet5_quantized
#define WAKENET_COEFF get_coeff_customized_word_wn5

#elif CONFIG_SR_WN_WN7_CUSTOMIZED_WORD_FIRST || CONFIG_SR_WN_WN7_CUSTOMIZED_WORD_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized
#define WAKENET_COEFF "custom7"

#elif CONFIG_SR_WN_WN7_XIAOAITONGXUE_FIRST || CONFIG_SR_WN_WN7_XIAOAITONGXUE_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized
#define WAKENET_COEFF "xiaoaitongxue7"

#elif CONFIG_SR_WN_WN7Q8_XIAOAITONGXUE_FIRST || CONFIG_SR_WN_WN7Q8_XIAOAITONGXUE_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized8
#define WAKENET_COEFF "xiaoaitongxue7q8"

#elif CONFIG_SR_WN_WN7_HILEXIN_FIRST || CONFIG_SR_WN_WN7_HILEXIN_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized
#define WAKENET_COEFF "hilexin7"

#elif CONFIG_SR_WN_WN7_HILEXIN_FIRST || CONFIG_SR_WN_WN7_HILEXIN_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized
#define WAKENET_COEFF "hilexin7q8"

#elif CONFIG_SR_WN_WN7_ALEXA_FIRST || CONFIG_SR_WN_WN7_ALEXA_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized
#define WAKENET_COEFF "alexa7"

#elif CONFIG_SR_WN_WN8_ALEXA_FIRST || CONFIG_SR_WN_WN8_ALEXA_SECOND
#define WAKENET_MODEL esp_sr_wakenet8_quantized
#define WAKENET_COEFF "alexa8"

#elif CONFIG_SR_WN_WN7Q8_ALEXA_FIRST || CONFIG_SR_WN_WN7Q8_ALEXA_SECOND
#define WAKENET_MODEL esp_sr_wakenet7_quantized8
#define WAKENET_COEFF "alexa7q8"

#elif CONFIG_SR_WN_WN8_HIESP_FIRST || CONFIG_SR_WN_WN8_HIESP_SECOND
#define WAKENET_MODEL esp_sr_wakenet8_quantized
#define WAKENET_COEFF "hiesp8"

#elif CONFIG_SR_WN_WN8Q8_HIESP_FIRST || CONFIG_SR_WN_WN8Q8_HIESP_SECOND
#define WAKENET_MODEL esp_sr_wakenet8_quantized8
#define WAKENET_COEFF "hiesp8q8"

#elif CONFIG_SR_WN_WN8_HILEXIN_FIRST || CONFIG_SR_WN_WN8_HILEXIN_SECOND
#define WAKENET_MODEL esp_sr_wakenet8_quantized
#define WAKENET_COEFF "hilexin8"

#else
#error No valid wake word selected.
#endif
#else
#define WAKENET_MODEL "NULL"
#define WAKENET_COEFF "NULL"
#endif
/* example

static const sr_model_iface_t *model = &WAKENET_MODEL;

//Initialize wakeNet model data
static model_iface_data_t *model_data=model->create(DET_MODE_90);

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
