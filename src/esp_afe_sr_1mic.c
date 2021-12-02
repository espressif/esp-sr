#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "esp_aec.h"
#include "esp_aec3.h"
#include "esp_aec3_728.h"
#include "esp_ns.h"
#include "esp_vad.h"
#include "io.h"
#include "esp_afe_sr_iface.h"
#include "sr_ringbuf.h"
#include "dl_lib.h"
#include "webrtc_vad.h"

typedef void (*aec_feed_func_t)(void *model, int16_t *ref, int16_t* in, int16_t* out);
static void afe_se_task(void *arg);

struct esp_afe_sr_data_t {
    struct sr_RingBuf *rb_in;
    struct sr_RingBuf *rb_out;
    afe_sr_mode_t mode;
    int rb_in_size;
    int rb_out_size;
    int sample_rate;
    int nch;
    int enable_se;

    //AEC
    void* aec_handle;
    int aec_frame_size;
    int aec_filter_length;
    int enable_aec;
    int aec_nch;
    int16_t *aec_tmp;
    int16_t *aec_in;

    //NS
    void* ns_handle;
    int ns_frame_size;
    int16_t *ns_in;
    int16_t *ns_out;

    //VAD
    void *vad_handle, *ref_vad_handle;
    int vad_mode, ref_vad_mode;

    //WakeNet
    esp_wn_iface_t *wakenet;
    model_iface_data_t *model_data;
    int audio_chunksize;
    int16_t *buff_wn;  //alloc a big space once for wakenet task.
    int enable_wn;
    int wn_mode;
    int wn_gain;
    int wn_nch;
    int channel_id;
    int agc_mode;
};

#include "freertos/event_groups.h"
#define AFE_DESTROY_BIT (BIT0)
static bool afe_task_flag = true;
static EventGroupHandle_t events = NULL;

static int16_t channelDataAmpAdjust(int16_t data, float factor)
{
    int valueCh = (int)(data * factor);
    return (int16_t)(((valueCh)>(32767) ? (32767) : (valueCh)<-(32768) ? -(32768) : (valueCh)));
}

static esp_afe_sr_data_t *afe_create_from_config(afe_config_t *afe_config)
{
    esp_afe_sr_data_t *afe = malloc(sizeof(esp_afe_sr_data_t));
    afe->mode = afe_config->afe_mode;
    afe->sample_rate = 16000;
    afe->nch = 1;
    afe->enable_se = 1;
    afe->enable_wn = 1;
    afe->enable_aec = 1;
    afe->channel_id = 0;
    int ns_frame_len_ms = 32;
    afe_task_flag = true;

    events = xEventGroupCreate();

    /****************************************************/
    /**********************INIT AEC**********************/
    /****************************************************/
    if (afe_config->aec_init) {
        afe->aec_frame_size = 512;
        afe->aec_filter_length = 4;
        afe->ref_vad_mode = 3;
        afe->aec_nch = afe->nch + 1;
        afe->ref_vad_handle = vad_create(afe->ref_vad_mode, 16000, 30);

        if (afe_config->alloc_from_psram) {
            afe->aec_tmp = dl_lib_calloc_psram(afe->aec_frame_size * afe->aec_nch, sizeof(int16_t), 16);
        } else {
            afe->aec_tmp = dl_lib_calloc(afe->aec_frame_size * afe->aec_nch, sizeof(int16_t), 16);
        }

        if (afe->mode == SR_MODE_LOW_COST) {
#ifdef CONFIG_IDF_TARGET_ESP32S3
            afe->aec_handle = esp_aec3_728_init(afe->nch, afe->aec_frame_size,
                                                afe->aec_filter_length, afe_config->alloc_from_psram);
#else
            printf("Not support SR_MODE_LOW_COST on ESP32\n");
            afe->aec_handle = NULL;
#endif
        } else {
            afe->aec_handle = esp_aec3_init(afe->nch, afe->aec_frame_size,
                                            afe->aec_filter_length, 0);
        }
    } else {
        afe->aec_frame_size = 512;
        afe->aec_nch = afe->nch + 1;
        afe->aec_handle = NULL;
    }

    /****************************************************/
    /**********************INIT SE**********************/
    /****************************************************/
    if (afe_config->se_init) {
        ns_frame_len_ms = 10;
        afe->ns_frame_size = 16 * ns_frame_len_ms;
        afe->ns_handle = ns_pro_create(ns_frame_len_ms, 2);
    } else {
        ns_frame_len_ms = 10;
        afe->ns_frame_size = 16 * ns_frame_len_ms;
        afe->ns_handle = NULL;
    }

    /****************************************************/
    /**********************INIT VAD**********************/
    /****************************************************/
    if (afe_config->vad_init) {
        afe->vad_mode = afe_config->vad_mode;
        afe->vad_handle = vad_create(afe->vad_mode, 16000, 30);
    } else {
        afe->vad_handle = NULL;
    }

    /********************************************************/
    /**********************INIT WAKENET**********************/
    /********************************************************/
    if (afe_config->wakenet_init) {
        afe->wn_nch = 1;
        afe->enable_wn = 1;
        afe->wn_mode = afe_config->wakenet_mode;
        afe->wakenet = afe_config->wakenet_model;
        afe->model_data = afe->wakenet->create(afe_config->wakenet_coeff, afe_config->wakenet_mode);
        afe->audio_chunksize = afe->wakenet->get_samp_chunksize(afe->model_data);
        afe->wn_gain = 2;
        if (afe_config->wakenet_mode == DET_MODE_2CH_90 || afe_config->wakenet_mode == DET_MODE_2CH_95) {
            afe->wn_nch = 2;
        }
        if (afe_config->wakenet_mode == DET_MODE_3CH_90 || afe_config->wakenet_mode == DET_MODE_3CH_95) {
            printf("ERROR: Please use 1CH or 2CH wakenet, please modify wakenet_mode!\n");
            abort();
        }
        afe->agc_mode = afe_config->agc_mode;
        if (afe->agc_mode > 3) afe->agc_mode = 3;
        if (afe->agc_mode < 0) afe->agc_mode = 0;
    } else {
        afe->wn_nch = 1;
        afe->enable_wn = 0;
        afe->audio_chunksize = 512;
        afe->wakenet = NULL;
        afe->buff_wn = NULL;
        afe->wn_gain = 2;
        afe->agc_mode = 0;
    }
    if (afe_config->alloc_from_psram) {
        afe->buff_wn = heap_caps_malloc(afe->audio_chunksize * (afe->wn_nch + afe->wn_nch - afe->nch) * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    } else {
        afe->buff_wn = heap_caps_malloc(afe->audio_chunksize * (afe->wn_nch + afe->wn_nch - afe->nch) * sizeof(int16_t), MALLOC_CAP_INTERNAL);
    }

    /********************************************************/
    /**********************INIT OTRHERS**********************/
    /********************************************************/
    if (afe_config->alloc_from_psram) {
        afe->aec_in = dl_lib_calloc_psram(afe->aec_frame_size * (afe->nch + 1) * sizeof(int16_t), 1, 16);
        afe->ns_in = heap_caps_malloc(afe->ns_frame_size * afe->wn_nch * sizeof(int16_t), MALLOC_CAP_SPIRAM);
        afe->ns_out = heap_caps_malloc(afe->ns_frame_size * afe->wn_nch * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    } else {
        afe->aec_in = dl_lib_calloc(afe->aec_frame_size * (afe->nch + 1) * sizeof(int16_t), 1, 16);
        afe->ns_in = heap_caps_malloc(afe->ns_frame_size * afe->wn_nch * sizeof(int16_t), MALLOC_CAP_INTERNAL);
        afe->ns_out = heap_caps_malloc(afe->ns_frame_size * afe->wn_nch * sizeof(int16_t), MALLOC_CAP_INTERNAL);
    }

    afe->rb_in_size = afe->aec_frame_size * afe->nch * afe_config->afe_ringbuf_size;
    afe->rb_out_size = afe->ns_frame_size * afe->wn_nch * afe_config->afe_ringbuf_size;

    afe->rb_in = sr_rb_init(BUFFER_PROCESS, afe->rb_in_size, 1, NULL);
    afe->rb_out = sr_rb_init(BUFFER_PROCESS, afe->rb_out_size, 1, NULL);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore(&afe_se_task, "afe_mase", 8 * 1024, (void *)afe, afe_config->afe_perferred_priority, NULL, afe_config->afe_perferred_core);

    printf("Initial ONE-MIC auido front-end for speech recognition, mode:%d, (%s %s)\n", afe->mode, __DATE__, __TIME__);
    return afe;
}

static esp_afe_sr_data_t *afe_create(afe_sr_mode_t mode, int perferred_core)
{
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.afe_mode = mode;
    afe_config.afe_perferred_core = perferred_core;
    esp_afe_sr_data_t *afe_handle = afe_create_from_config(&afe_config);

    return afe_handle;
}

static int afe_feed(esp_afe_sr_data_t *afe, int16_t *in)
{
    if (!afe_task_flag) return;

    int ref_vad_res = 0;

    if (afe->aec_handle != NULL && afe->enable_aec == 1) {
        for (int i = 0; i < afe->aec_frame_size; i++) {
            afe->aec_in[i] = in[i * afe->aec_nch];
            afe->aec_in[i + afe->aec_frame_size] = in[i * afe->aec_nch + 1];
        }

        ref_vad_res = vad_process(afe->ref_vad_handle, afe->aec_in + afe->aec_frame_size);
        static int frame_cnt = 0;
        if (frame_cnt++ < 4) ref_vad_res = 0;

        if (afe->mode == SR_MODE_LOW_COST) {
#ifdef CONFIG_IDF_TARGET_ESP32S3
            esp_aec3_728_write_ref_vad(afe->aec_handle, ref_vad_res);
            esp_aec3_728_process(afe->aec_handle, afe->aec_in, afe->aec_in + afe->aec_frame_size, afe->aec_tmp);
#endif
        } else if (afe->mode == SR_MODE_HIGH_PERF) {
            esp_aec3_write_ref_vad(afe->aec_handle, ref_vad_res);
            esp_aec3_process(afe->aec_handle, afe->aec_in, afe->aec_in + afe->aec_frame_size, afe->aec_tmp);
        }

        memcpy(afe->aec_in, afe->aec_tmp, afe->nch * afe->aec_frame_size * sizeof(int16_t));
    } else {
        for (int i = 0; i < afe->aec_frame_size; i++) {
            afe->aec_in[i] = in[afe->aec_nch * i];
        }
    }

    // // now--- aec_in: {l_o,r_o,l,l_o,r_o,l,l_o,r_o,l,...}
    if (sr_rb_available(afe->rb_in) < afe->nch * afe->aec_frame_size * sizeof(int16_t)) {
        printf("ERROR! rb_in slow!!!\n");
    }

    int size = sr_rb_write(afe->rb_in, afe->aec_in, afe->nch * afe->aec_frame_size * sizeof(int16_t), 0);

    return size;
}

static void afe_se_task(void *arg)
{
    esp_afe_sr_data_t *afe = (esp_afe_sr_data_t *)arg;

    while (afe_task_flag) {
        sr_rb_read(afe->rb_in, afe->ns_in, afe->nch * afe->ns_frame_size * sizeof(int16_t), portMAX_DELAY);

        if (afe->ns_handle != NULL && afe->enable_se == 1) {
            if (afe->wn_nch - afe->nch == 1) {
                memcpy(afe->ns_out + afe->ns_frame_size, afe->ns_in, afe->ns_frame_size * sizeof(int16_t));
            }

            if (afe->mode == SR_MODE_LOW_COST || afe->mode == SR_MODE_HIGH_PERF) {
                ns_process(afe->ns_handle, afe->ns_in, afe->ns_out);
            }

            for (int n = 0; n < afe->wn_nch; n++) {
                int ns_out_shift = n * afe->ns_frame_size;
                for (int i = 0; i < afe->ns_frame_size; i++) {
                    afe->ns_in[i * afe->wn_nch + n] = afe->ns_out[i + ns_out_shift];
                }
            }
        } else {
            if (afe->wn_nch - afe->nch == 1) {
                memcpy(afe->ns_out, afe->ns_in, afe->nch * afe->ns_frame_size * sizeof(int16_t));

                for (int i = 0; i < afe->ns_frame_size; i++) {
                    int ret = afe->ns_out[afe->nch * i];
                    afe->ns_in[afe->wn_nch * i + 0] = ret;
                    afe->ns_in[afe->wn_nch * i + 1] = ret;
                }
            }
        }
        // if (sr_rb_available(afe->rb_out) < afe->wn_nch * afe->ns_frame_size * sizeof(int16_t)) {
        //     printf("ERROR! rb_out slow!!!\n");
        // }
        sr_rb_write(afe->rb_out, afe->ns_in, afe->wn_nch * afe->ns_frame_size * sizeof(int16_t), 0);
    }
    printf("afe_se_task quit\n");

    xEventGroupSetBits(events, AFE_DESTROY_BIT);
    vTaskDelete(NULL);
}

static int afe_fetch(esp_afe_sr_data_t *afe, int16_t *out)
{
    static float out_gain = 1.0;
    int res = 0;

    if (!afe_task_flag) return;
    sr_rb_read(afe->rb_out, afe->buff_wn, afe->wn_nch * afe->audio_chunksize * sizeof(int16_t), portMAX_DELAY);


    if (afe->wakenet != NULL && afe->enable_wn == 1) {

        if (afe->wn_nch - afe->nch == 1) {
            for (int i = 0; i < afe->audio_chunksize; i++) {
                out[i] = channelDataAmpAdjust(afe->buff_wn[afe->wn_nch * i], (float)afe->wn_gain);
                int ret = channelDataAmpAdjust(afe->buff_wn[afe->wn_nch * i + 1], (float)afe->wn_gain);
                afe->buff_wn[i] = ret;
            }
            memcpy(afe->buff_wn + afe->audio_chunksize, out, afe->audio_chunksize * sizeof(int16_t));
        } else {
            for (int i = 0; i < afe->audio_chunksize; i++) {
                int ret = channelDataAmpAdjust(afe->buff_wn[i], (float)afe->wn_gain); //channel 1
                afe->buff_wn[i] = ret;
            }
        }

        res = afe->wakenet->detect(afe->model_data, afe->buff_wn);
        //selector & gainer
        afe->channel_id = afe->wakenet->get_triggered_channel(afe->model_data);
        if (res > 0 && afe->agc_mode >= 0) {
            if (afe->agc_mode == 0) out_gain = 1;
            else out_gain = afe->wakenet->get_vol_gain(afe->model_data, (afe->agc_mode - 6)) * afe->wn_gain;
        }

        int shift = afe->audio_chunksize * afe->channel_id;
        for (int i = 0; i < afe->audio_chunksize; i++) {
            out[i] = afe->buff_wn[i + shift];
        }
    } else {
        for (int i = 0; i < afe->audio_chunksize; i++) {
            out[i] = channelDataAmpAdjust(afe->buff_wn[afe->wn_nch * i + afe->channel_id], (float)out_gain);
        }
    }

    if (res == 0) {
        // vad
        if (afe->vad_handle != NULL) {
            res = vad_process(afe->vad_handle, out);
        }
        if (res == VAD_SILENCE) return AFE_FETCH_NOISE;
        else return AFE_FETCH_SPEECH;
    } else if (res == -1) {
        return AFE_FETCH_CHANNEL_VERIFIED;
    } else {
        return AFE_FETCH_WWE_DETECTED;
    }

    return res;
}

static int afe_set_wakenet(esp_afe_sr_data_t *afe, esp_wn_iface_t *wakenet,
                           const model_coeff_getter_t *model_coeff)
{
    afe->enable_wn = 1;
    if (afe->wakenet == NULL) {
        afe->wakenet = wakenet;
    } else {
        afe->wakenet->destroy(afe->model_data);
        afe->wakenet = wakenet;
    }

    // initial wakenet
    afe->model_data = wakenet->create(model_coeff, afe->wn_mode);
    afe->audio_chunksize = wakenet->get_samp_chunksize(afe->model_data);
    printf("wakenet audio_chunksize = %d\n", afe->audio_chunksize);

    return 1;
}

static int afe_get_feed_chunksz(esp_afe_sr_data_t *afe)
{
    return afe->aec_frame_size;
}

static int afe_get_fetch_chunksz(esp_afe_sr_data_t *afe)
{
    if (afe->wakenet == NULL) {
        printf("wakenet is NULL, please use set_wakenet function to set wakenet.\n");
    }
    return afe->audio_chunksize;
}

static int afe_get_channel_num(esp_afe_sr_data_t *afe)
{
    return afe->nch;
}

static int afe_get_sample_rate(esp_afe_sr_data_t *afe)
{
    return afe->sample_rate;
}

static int afe_disable_wakenet(esp_afe_sr_data_t *afe)
{
    afe->enable_wn = 0;
    return afe->enable_wn;
}

static int afe_enable_wakenet(esp_afe_sr_data_t *afe)
{
    afe->enable_wn = 1;
    return afe->enable_wn;
}

static int afe_disable_aec(esp_afe_sr_data_t *afe)
{
    afe->enable_aec = 0;
    return afe->enable_aec;
}

static int afe_enable_aec(esp_afe_sr_data_t *afe)
{
    afe->enable_aec = 1;
    return afe->enable_aec;
}

static int afe_disable_se(esp_afe_sr_data_t *afe)
{
    afe->enable_se = 0;
    return afe->enable_se;
}

static int afe_enable_se(esp_afe_sr_data_t *afe)
{
    afe->enable_se = 1;
    return afe->enable_se;
}

static void afe_destory(esp_afe_sr_data_t *afe)
{
    afe_task_flag = false;
    vTaskDelay(10 / portTICK_PERIOD_MS);
    sr_rb_abort(afe->rb_in, 1);
    sr_rb_abort(afe->rb_out, 1);
    EventBits_t bits = xEventGroupWaitBits(events, AFE_DESTROY_BIT, true, false, portMAX_DELAY);

    vEventGroupDelete(events);
    events = NULL;

    if (bits & AFE_DESTROY_BIT) {
        printf("afe task destroy finished\n");
    } else {
        printf("Fail to destroy afe task\n");
    }
    if (afe != NULL) {
        dl_lib_free(afe->aec_in);
        dl_lib_free(afe->aec_tmp);
        free(afe->ns_in);
        free(afe->ns_out);
        sr_rb_unint(afe->rb_in);
        sr_rb_unint(afe->rb_out);

        if (afe->aec_handle != NULL) {
            if (afe->mode == SR_MODE_LOW_COST) {
#ifdef CONFIG_IDF_TARGET_ESP32S3
                vad_destroy(afe->ref_vad_handle);
                esp_aec3_728_destroy(afe->aec_handle);
#endif
            }
            else if (afe->mode == SR_MODE_HIGH_PERF) {
                vad_destroy(afe->ref_vad_handle);
                esp_aec3_destroy(afe->aec_handle);
            }
            afe->aec_handle = NULL;
        }

        if (afe->ns_handle != NULL) {
            if (afe->mode == SR_MODE_LOW_COST) {
                ns_destroy(afe->ns_handle);
            }
            afe->ns_handle = NULL;
        }

        if (afe->wakenet != NULL) {
            afe->wakenet->destroy(afe->model_data);
            afe->wakenet = NULL;
        }
        if (afe->buff_wn != NULL) {
            free(afe->buff_wn);
            afe->buff_wn = NULL;
        }

        if (afe->vad_handle != NULL) {
            vad_destroy(afe->vad_handle);
            afe->vad_handle = NULL;
        }

        free(afe);
        afe = NULL;
    }
}

void print_afe1mic_info(esp_afe_sr_data_t *afe)
{
    printf("\n***********************************************\n");
    printf("**********************AFE**********************\n");
    printf("***********************************************\n");
    printf("afe_mode = %d\n", afe->mode);
    printf("sample_rate = %d\n", afe->sample_rate);
    printf("nch = %d\n", afe->nch);
    printf("rb_in size = %d\n", afe->rb_in_size);
    printf("rb_out size = %d\n", afe->rb_out_size);

    printf("**********************AEC**********************\n");
    printf("aec_handle = %p\n", afe->aec_handle);
    printf("aec_frame_size = %d\n", afe->aec_frame_size);
    printf("aec_filter_length = %d\n", afe->aec_filter_length);
    printf("enable_aec = %d\n", afe->enable_aec);
    printf("aec_nch = %d\n", afe->aec_nch);

    printf("**********************NS**********************\n");
    printf("ns_handle = %p\n", afe->ns_handle);
    printf("ns_frame_size = %d\n", afe->ns_frame_size);
    printf("enable_ns = %d\n", afe->enable_se);

    printf("**********************VAD**********************\n");
    printf("vad_handle = %p\n", afe->vad_handle);
    printf("vad mode = %d\n", afe->vad_mode);

    printf("**********************WAKENET**********************\n");
    printf("wakenet_handle = %p\n", afe->wakenet);
    printf("audio_chunksize = %d\n", afe->audio_chunksize);
    printf("wn_mode = %d\n", afe->wn_mode);
    printf("wn_gain = %d\n", afe->wn_gain);
    printf("wn_nch = %d\n", afe->wn_nch);
    printf("enable_wn = %d\n", afe->enable_wn);
    printf("***********************************************\n");
    printf("**********************END**********************\n");
    printf("***********************************************\n\n");
}

// (AEC)NLMS + ( NS)
const esp_afe_sr_iface_t esp_afe_sr_1mic = {
    .create = afe_create,
    .create_from_config = afe_create_from_config,
    .feed = afe_feed,
    .fetch = afe_fetch,
    .get_feed_chunksize = afe_get_feed_chunksz,
    .get_fetch_chunksize = afe_get_fetch_chunksz,
    .get_samp_rate = afe_get_sample_rate,
    .get_channel_num = afe_get_channel_num,
    .set_wakenet = afe_set_wakenet,
    .disable_wakenet = afe_disable_wakenet,
    .enable_wakenet = afe_enable_wakenet,
    .disable_aec = afe_disable_aec,
    .enable_aec = afe_enable_aec,
    .disable_se = afe_disable_se,
    .enable_se = afe_enable_se,
    .destroy = afe_destory,
};
