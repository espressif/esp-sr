#pragma once
#include "esp_mfcc_iface.h"

extern const esp_mfcc_iface_t esp_fbank_f32; // float32-fbank handle
extern const esp_mfcc_iface_t esp_fbank_s16; // int16-fbank handle

/**
 * @brief Return basic opts used in wakenet9 & multinet5
 **/
esp_mfcc_opts_t *get_mfcc_opts_wn9();

/**
 * @brief Return basic opts used in wakenet9s
 **/
esp_mfcc_opts_t *get_mfcc_opts(const char *win_type, bool use_power, int winstep_ms, int winlen_ms, int nfilter);

/**
 * @brief Return basic opts for default kaldifeat
 *
    opts->psram_first = true;
    opts->use_power = true;
    opts->use_log_fbank = 2;  // log(max(x, log_epsilon))
    opts->log_epsilon = 1.1920928955078125e-07f; // torch.finfo(torch.float32).eps
    opts->win_type = "povey";
    opts->low_freq = 20;
    opts->high_freq = 7600;
    opts->samp_freq = 16000;
    opts->nch = 1;
    opts->nfft = 512;
    opts->nfilter = 80;
    opts->numcep = 80;
    opts->preemph = 0.97;
    opts->append_energy = false;
    opts->winlen_ms = 25;
    opts->winstep_ms = 10;
    opts->remove_dc_offset = true;
 *
 **/
esp_mfcc_opts_t *get_mfcc_opts_kaldi();

/**
 * @brief Print mfcc opts
 **/
void print_mfcc_opts(esp_mfcc_opts_t *opts);
