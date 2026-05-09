#ifndef _ESP_AFE_DOA_H_
#define _ESP_AFE_DOA_H_

#include "esp_doa.h"
#include "esp_afe_config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    doa_handle_t *doa_handle;
    afe_pcm_config_t pcm_config;
    int16_t *leftdata;
    int16_t *rightdata;
    int frame_size;
} afe_doa_handle_t;

/**
 * @brief Initialize SRP-PHAT processor
 * @param input_format     The input format
 * @param fs Sampling rate (Hz), e.g., 16000
 * @param resolution Angular search resolution (degrees), e.g., 20
 * @param d_mics Microphone spacing (meters), e.g., 0.06
 * @param input_timedate_samples input timedate samples, e.g., 1024
 * @return Initialized doa_handle_t object pointer, Recommend using the above configuration for better performance
 */
afe_doa_handle_t *afe_doa_create(const char *input_format, int fs, float resolution, float d_mics, int input_timedate_samples);
/**
 * @brief Process audio frame for direction estimation
 * @param handle doa_handle_t instance pointer
 * @param indata Input audio data, format is define by input_format.
 * @return Estimated sound direction in degrees, e.g., 0-180
 */
float afe_doa_process(afe_doa_handle_t *handle, const int16_t *indata);
/**
 * @brief Release all allocated resources
 * @param doa doa_handle_t instance pointer to be freed
 */
void afe_doa_destroy(afe_doa_handle_t *handle);


#ifdef __cplusplus
}
#endif

#endif /* _ESP_AFE_DOA_H_ */