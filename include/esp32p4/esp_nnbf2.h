/* Multi-channel neural beamforming for ESP32-P4.
 *
 * Combines neural-network mask estimation (nsnet) with an online MVDR
 * beamformer: N input channels -> 1 enhanced output channel, streaming at
 * 16 kHz with a 16 ms (256-sample) hop. An optional neural post-filter
 * further cleans up the beamformed output (see
 * esp_nnbf2_config_t.enable_post_filter).
 *
 * The pipeline is fully causal and stays entirely in the STFT domain:
 * GTCRN#1 receives Y_ref and GTCRN#2 receives the beamformed spectrum
 * directly (nsnet4 spectrum-injection API), so no IFFT/STFT round-trips
 * remain; end-to-end algorithmic latency is 16 ms with or without the
 * post-filter.
 *
 * All allocations live in PSRAM; esp_nnbf2_process() performs no malloc.
 * esp_srmodel_init("model") must have been called before esp_nnbf2_create().
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_NNBF2_HOP 256     /* frame shift, identical to the nsnet chunksize */
#define ESP_NNBF2_SR  16000

/* Opaque model data container */
typedef struct esp_nnbf2_data_t esp_nnbf2_data_t;

typedef struct {
    int channel_num;     /* number of input channels, 2..ESP_NNBF2_MAX_CHANNELS (default 4) */
    int ref_channel;     /* reference channel the target speech is anchored to (default 0) */
    float alpha;         /* smoothing of the online spatial statistics, 0..1; higher
                            adapts more slowly and is more stable (default 0.95) */
    float mask_n_floor;  /* lower bound on the noise-estimate weight; keeps noise
                            tracking alive during long speech (default 0.02) */
    float diag_load;     /* regularisation of the internal matrix inversion; increase
                            if the output distorts in very quiet noise (default 1e-4) */
    int pi_iters;        /* refinement steps of the steering-vector estimate; more
                            gives sharper directivity at higher CPU (default 2) */
    int enable_post_filter; /* run the neural post-filter on the beamformed output
                               (default 1); when 0 the raw beamformer output is
                               returned, saving ~133 KB PSRAM, its CPU share and
                               16 ms of latency */
} esp_nnbf2_config_t;

#define ESP_NNBF2_MAX_CHANNELS 8

/**
 * @brief Fill cfg with the defaults (4 ch, ref 0, alpha 0.95, floor 0.02,
 *        load 1e-4, 2 refinement steps, post-filter enabled).
 */
void esp_nnbf2_default_config(esp_nnbf2_config_t *cfg);

/**
 * @brief Create a beamforming model instance
 *
 * @param model_name The nsnet model name, e.g. from
 *        esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL). Must be the
 *        nsnet4 model (chunksize 256, 16 kHz); GTCRN#1 is driven through the
 *        nsnet4-specific mask pass-through API.
 * @param cfg Pipeline configuration; NULL selects the defaults.
 * @returns Handle to the model data, NULL on failure.
 */
esp_nnbf2_data_t *esp_nnbf2_create(char *model_name, const esp_nnbf2_config_t *cfg);

/**
 * @brief Get the amount of samples that need to be passed to the process function
 *
 * @param model The model object to query
 * @return The amount of samples per channel per call, in 16-bit samples
 */
int esp_nnbf2_get_samp_chunksize(esp_nnbf2_data_t *model);

/**
 * @brief Get the sample rate of the samples to feed to the process function
 *
 * @param model The model object to query
 * @return The sample rate, in hz
 */
int esp_nnbf2_get_samp_rate(esp_nnbf2_data_t *model);

/**
 * @brief Feed one hop of multi-channel samples and get the beamformed output
 *
 * @param model The model object
 * @param in_data Array of per-channel input pointers (channel_num entries),
 *        each pointing to get_samp_chunksize 16-bit signed samples
 * @param out_data Receives get_samp_chunksize 16-bit signed enhanced samples
 * @return 0 on success, -1 on bad arguments
 */
int esp_nnbf2_process(esp_nnbf2_data_t *model, int16_t **in_data, int16_t *out_data);

/**
 * @brief Number of frequency bins that used the safe fallback beamformer
 *        since create (health metric; 0 in normal operation)
 */
int esp_nnbf2_get_fallback_count(esp_nnbf2_data_t *model);

/**
 * @brief Destroy a beamforming model instance
 *
 * @param model Model object to destroy
 */
void esp_nnbf2_destroy(esp_nnbf2_data_t *model);

#ifdef __cplusplus
}
#endif
