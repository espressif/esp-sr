#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "esp_aec.h"
#include "esp_ns.h"
#include "esp_vad.h"
#include "io.h"
#include "esp_afe_sr_iface.h"
#include "sr_ringbuf.h"

typedef void (*aec_feed_func_t)(void *model, int16_t *ref, int16_t* in, int16_t* out);
static void afe_se_task(void *arg);

struct esp_afe_sr_data_t {
    struct sr_RingBuf *rb_in;
    struct sr_RingBuf *rb_out;
    int rb_buffer_size;
    int sample_rate;
    int nch;
    afe_sr_mode_t mode;
    int enable_se;

    // NS
    void* ns_handle;
    int ns_frame_size;

    // AEC
    void* aec_handle;
    int16_t aec_frame_size;
    int16_t aec_filter_length;
    aec_feed_func_t *aec_feed;
    int enable_aec;

    // New AFE
    int16_t *aec_in;
    int16_t *ns_in;

    int buff_se_max;

    //VAD
    void* vad_handle;
    int vad_mode;

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
};

#include "freertos/event_groups.h"
#define AFE_DESTROY_BIT (BIT0)
static bool afe_task_flag = true;
static EventGroupHandle_t events = NULL;


static esp_afe_sr_data_t *afe_create(afe_sr_mode_t mode, int perferred_core)
{
    esp_afe_sr_data_t *afe = malloc(sizeof(esp_afe_sr_data_t));
    afe->mode = mode;
    afe->sample_rate = 16000;
    afe->nch = 2;
    afe->enable_se = 1;
    afe->enable_wn = 0;
    afe->enable_aec = 1;
    afe->channel_id = 0;
    int ns_frame_len_ms = 32;

    events = xEventGroupCreate();

    // MODE=0, aec frame_ms=16ms;  NS frame_ms=10ms;
    if (mode == 0) {
        afe->nch = 1;
        afe->aec_frame_size = 256;
        afe->aec_filter_length = 1600;
        afe->aec_handle = aec_create(afe->sample_rate,
                                     afe->aec_frame_size / 16,
                                     afe->aec_filter_length);

        ns_frame_len_ms = 10;
        afe->ns_handle = ns_create(ns_frame_len_ms);
        afe->ns_frame_size = 16 * ns_frame_len_ms;

        afe->wn_mode = DET_MODE_90;
        afe->wn_nch = afe->nch;
    }

    // MODE=1, aec frame_ms=16ms;  NS frame_ms=10ms;
    else if (mode == 1) {
        afe->nch = 1;
        afe->aec_frame_size = 256;
        afe->aec_filter_length = 1600;
        afe->aec_handle = aec_create(afe->sample_rate,
                                     afe->aec_frame_size / 16,
                                     afe->aec_filter_length);

        ns_frame_len_ms = 10;
        afe->ns_handle = ns_pro_create(ns_frame_len_ms, 2);
        afe->ns_frame_size = 16 * ns_frame_len_ms;

        afe->wn_mode = DET_MODE_90;
        afe->wn_nch = afe->nch;
    }

    // //AEC input + AEC output + AEC ref +  NS output
    afe->aec_in = malloc(afe->aec_frame_size * (afe->nch + 1) * sizeof(int16_t));
    afe->ns_in = malloc(afe->ns_frame_size * afe->wn_nch * sizeof(int16_t));

    //WakeNet
    afe->wakenet = NULL;
    afe->model_data = NULL;
    afe->buff_wn = NULL;
    afe->audio_chunksize = 512;
    afe->wn_gain = 1;

    int rb_in_size = afe->aec_frame_size * afe->wn_nch * 50;
    int rb_out_size = afe->ns_frame_size * afe->wn_nch * 50;

    afe->rb_in = sr_rb_init(BUFFER_PROCESS, rb_in_size, 1, NULL);
    afe->rb_out = sr_rb_init(BUFFER_PROCESS, rb_out_size, 1, NULL);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //VAD
    afe->vad_mode = 3;
    afe->vad_handle = vad_create(afe->vad_mode, 16000, 30);


    xTaskCreatePinnedToCore(&afe_se_task, "afe_mase", 8 * 1024, (void *)afe, 5, NULL, perferred_core);

    printf("Initial ONE-MIC auido front-end for speech recognition, mode:%d, (%s %s)\n", mode, __DATE__, __TIME__);
    return afe;
}

static int afe_feed(esp_afe_sr_data_t *afe, int16_t *in)
{
    int aec_nch = afe->nch + 1;

    if (afe->enable_aec == 0) {
        if (afe->mode == 0 || afe->mode == 1) {
            for (int i = 0; i < afe->aec_frame_size; i++) {
                afe->aec_in[i] = in[i * aec_nch];
                afe->aec_in[i + afe->aec_frame_size] = in[i * aec_nch + 1];
            }
            memcpy(in, afe->aec_in, afe->aec_frame_size * sizeof(int16_t));
        }
    } else if (afe->mode == 0 || afe->mode == 1) {
        for (int i = 0; i < afe->aec_frame_size; i++) {
            afe->aec_in[i] = in[i * aec_nch];
            afe->aec_in[i + afe->aec_frame_size] = in[i * aec_nch + 1];
        }
        aec_process(afe->aec_handle, afe->aec_in, afe->aec_in + afe->aec_frame_size, in);
    }

    if (sr_rb_available(afe->rb_in) < (afe->nch) * afe->aec_frame_size * sizeof(int16_t)) {
        ets_printf("ERROR! rb_in SLOW!!!\n");
    }

    memcpy(afe->aec_in, in, afe->wn_nch * afe->aec_frame_size * sizeof(int16_t));

    int size = sr_rb_write(afe->rb_in, afe->aec_in, afe->wn_nch * afe->aec_frame_size * sizeof(int16_t), 0);

    return size;
}


static void afe_se_task(void *arg)
{
    esp_afe_sr_data_t *afe = (esp_afe_sr_data_t *)arg;
    int16_t *samples = malloc(afe->wn_nch * afe->ns_frame_size * sizeof(int16_t));

    while (afe_task_flag) {
        sr_rb_read(afe->rb_in, samples, afe->wn_nch * afe->ns_frame_size * sizeof(int16_t), portMAX_DELAY);

        for (int n = 0; n < afe->wn_nch; n++) {
            int shift = n * afe->ns_frame_size;
            for (int i = 0; i < afe->ns_frame_size; i++) {
                afe->ns_in[i + shift] = samples[i * afe->nch + n];
            }
        }

        if (afe->mode == 0 || afe->mode == 1) {
            ns_process(afe->ns_handle, afe->ns_in, samples);
        }

        memcpy(afe->ns_in, samples, afe->wn_nch * afe->ns_frame_size * sizeof(int16_t));

        sr_rb_write(afe->rb_out, afe->ns_in, afe->wn_nch * afe->ns_frame_size * sizeof(int16_t), 0);
    }
    printf("afe_se_task quit\n");
    free(samples);

    xEventGroupSetBits(events, AFE_DESTROY_BIT);
    vTaskDelete(NULL);

}


static int afe_fetch(esp_afe_sr_data_t *afe, int16_t *out)
{
    static float out_gain = 1.0;
    int res = 0;
    int audio_chunksize = afe->audio_chunksize;
    int nch = afe->nch;
    int wn_nch = afe->wn_nch;
    int wn_gain = afe->wn_gain;
    esp_wn_iface_t *wakenet = afe->wakenet;
    model_iface_data_t *model_data = afe->model_data;

    int buff_nch = afe->wn_nch + afe->wn_nch - afe->nch;

    if (afe->buff_wn == NULL) {
        afe->buff_wn = malloc(audio_chunksize * buff_nch * sizeof(int16_t));
        afe->audio_chunksize = audio_chunksize;
    }

    int16_t *buff = afe->buff_wn;
    sr_rb_read(afe->rb_out, buff, wn_nch * audio_chunksize * sizeof(int16_t), portMAX_DELAY);

    if (afe->mode == 0 || afe->mode == 1) {
        for (int i = 0; i < audio_chunksize; i++) {
            int ret = buff[i] * wn_gain; //channel 1
            buff[i] = ret;
        }
    }

    // output
    if (afe->enable_wn)
        res = wakenet->detect(model_data, buff);

    //selector & gainer
    if (afe->enable_wn) {
        afe->channel_id = wakenet->get_triggered_channel(model_data);
        if (res > 0) {
            out_gain = wakenet->get_vol_gain(model_data, -5);
        }
    }

    int shift = audio_chunksize * afe->channel_id;
    for (int i = 0; i < audio_chunksize; i++) {
        out[i] = buff[i + shift] * out_gain;
    }

    // vad
    if (res <= 0) {
        res = vad_process(afe->vad_handle, out);
        res -= 1;
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

static void afe_destory(esp_afe_sr_data_t *afe)
{
    afe_task_flag = false;
    sr_rb_abort(afe->rb_in, 1);
    sr_rb_abort(afe->rb_out, 1);
    EventBits_t bits = xEventGroupWaitBits(events, AFE_DESTROY_BIT, true, false, portMAX_DELAY);

    vEventGroupDelete(events);

    if (bits & AFE_DESTROY_BIT) {
        printf("afe task destroy finished\n");
    } else {
        printf("Fail to destroy afe task\n");
    }
    if (afe != NULL) {
        free(afe->aec_in);
        free(afe->ns_in);
        sr_rb_unint(afe->rb_in);
        sr_rb_unint(afe->rb_out);

        if (afe->aec_handle != NULL) {
            if (afe->mode == 0 || afe->mode == 1) {
                aec_destroy(afe->aec_handle);
            }
            afe->aec_handle = NULL;
        }

        if (afe->ns_handle != NULL) {
            if (afe->mode == 0 || afe->mode == 1) {
                ns_destroy(afe->ns_handle);
            }
            afe->ns_handle = NULL;
        }

        if (afe->wakenet != NULL) {
            afe->wakenet->destroy(afe->model_data);
            if (afe->buff_wn != NULL) {
                free(afe->buff_wn);
            }

            afe->wakenet = NULL;
        }

        if (afe->vad_handle != NULL) {
            vad_destroy(afe->vad_handle);
            afe->vad_handle = NULL;
        }

        free(afe);
        afe = NULL;
    }
}

// (AEC)NLMS + ( NS)
const esp_afe_sr_iface_t esp_afe_sr_1mic = {
    .create = afe_create,
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
    .destroy = afe_destory,
};
