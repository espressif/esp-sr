#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <limits.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_mfcc_iface.h"
#include "esp_mfcc_models.h"
#include "alexa.h"

esp_mfcc_opts_t *get_fbank_opts_kaldi(int dim)
{
    esp_mfcc_opts_t *opts = (esp_mfcc_opts_t*)malloc(sizeof(esp_mfcc_opts_t));
    opts->psram_first = true;
    opts->use_power = true;
    opts->use_log_fbank = 0;  // log(max(x, log_epsilon))
    opts->log_epsilon = 1.1920928955078125e-07f; // torch.finfo(torch.float32).eps
    opts->win_type = const_cast<char*>("hanning"); // remove [-Wwrite-strings] warning
    opts->low_freq = 20;
    opts->high_freq = 7600;
    opts->samp_freq = 16000;
    opts->nch = 1;
    opts->nfft = 512;
    opts->nfilter = 80;
    opts->numcep = dim;
    opts->preemph = 0.97;
    opts->append_energy = false;
    opts->winlen_ms = 25;
    opts->winstep_ms = 10;
    opts->remove_dc_offset = true;

    return opts;
}


TEST_CASE("test speech features", "[fbank]")
{
    int16_t *buffer = (int16_t *) malloc(512 * sizeof(int16_t));
    const esp_mfcc_iface_t *fbank_handle = &esp_fbank_f32;
    float* fbank_out = NULL;

    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    // MFCC init
    int out_dim = 80;
    esp_mfcc_data_t *fbank_model = fbank_handle->create(get_fbank_opts_kaldi(out_dim));
    memcpy(buffer, alexa, 512 * sizeof(int16_t));
    fbank_out = fbank_handle->run_step(fbank_model, buffer, 0);
    fbank_handle->destroy(fbank_model);
    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    for (int i = 0; i < out_dim; i++) {
        printf("%f ", fbank_out[i]);
    }
    
    fbank_model = fbank_handle->create(get_fbank_opts_kaldi(out_dim));
    memcpy(buffer, alexa, 512 * sizeof(int16_t));
    fbank_out = fbank_handle->run_step(fbank_model, buffer, 0);
    fbank_handle->destroy(fbank_model);
    int second_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    
    TEST_ASSERT_EQUAL(true, start_size - first_end_size < 100); // there are some memory leak in system
    TEST_ASSERT_EQUAL(true, first_end_size == second_end_size);
}
