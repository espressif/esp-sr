#ifndef _ESP_DOA_H_
#define _ESP_DOA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct doa_handle_t doa_handle_t;
/**
 * @brief Initialize SRP-PHAT processor
 * @param fs Sampling rate (Hz), e.g., 16000
 * @param resolution Angular search resolution (degrees), e.g., 20
 * @param d_mics Microphone spacing (meters), e.g., 0.06
 * @param input_timedate_samples input timedate samples, e.g., 1024
 * @return Initialized doa_handle_t object pointer, Recommend using the above configuration for better performance
 */
doa_handle_t *esp_doa_create(int fs, float resolution, float d_mics, int input_timedate_samples);

/**
 * @brief Release all allocated resources
 * @param doa doa_handle_t instance pointer to be freed
 */
void esp_doa_destroy(doa_handle_t *doa);

/**
 * @brief Process audio frame for direction estimation
 * @param doa doa_handle_t instance pointer
 * @param left Left channel 16-bit PCM data
 * @param right Right channel 16-bit PCM data
 * @return Estimated sound direction in degrees, e.g., 0-180
 */
float esp_doa_process(doa_handle_t *doa, int16_t* left, int16_t* right);


#ifdef __cplusplus
}
#endif

#endif /* _ESP_DOA_H_ */
