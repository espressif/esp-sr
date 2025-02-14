#pragma once
#include "esp_aec.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

// AFE: Audio Front-End
// SR:  Speech Recognition
// VC:  Voice Communication

// Set AFE_SR mode
typedef enum {
    SR_MODE_LOW_COST = 0,  // Deprecated, please use afe_mode_t, AFE mode: low cost mode
    SR_MODE_HIGH_PERF = 1, // Deprecated, please use afe_mode_t, AFE mode: high performance mode
} afe_sr_mode_t;

// Set AFE mode
typedef enum {
    AFE_MODE_LOW_COST = 0,  // AFE mode: low cost mode
    AFE_MODE_HIGH_PERF = 1, // AFE mode: high performance mode
} afe_mode_t;

// Set AFE type
typedef enum {
    AFE_TYPE_SR = 0, // Speech recognition scenarios, excluding nonlinear noise suppression
    AFE_TYPE_VC = 1, // Voice communication scenarios, including nonlinear noise suppression
} afe_type_t;

typedef enum {
    AFE_MEMORY_ALLOC_MORE_INTERNAL = 1,          // malloc with more internal ram
    AFE_MEMORY_ALLOC_INTERNAL_PSRAM_BALANCE = 2, // malloc with internal ram and psram in balance
    AFE_MEMORY_ALLOC_MORE_PSRAM = 3              // malloc with more psram
} afe_memory_alloc_mode_t;

typedef enum {
    AFE_MN_PEAK_AGC_MODE_1 = -9, // The peak amplitude of fetch audio is -9dB
    AFE_MN_PEAK_AGC_MODE_2 = -6, // The peak amplitude of fetch audio is -6dB
    AFE_MN_PEAK_AGC_MODE_3 = -3, // The peak amplitude of fetcg is -3dB
    AFE_MN_PEAK_NO_AGC = 0,      // There is no agc gain
} afe_mn_peak_agc_mode_t;

typedef struct {
    int total_ch_num; // total channel num, include microphone channel, playback channel and unknown channel
    int mic_num;      // microphone channel number
    uint8_t *mic_ids; // microphone channel indices
    int ref_num;      // playback reference channel number
    uint8_t *ref_ids; // playback reference channel indices
    int sample_rate;  // sample rate of audio
} afe_pcm_config_t;

typedef enum {
    AFE_NS_MODE_WEBRTC = 0, // please use model name of NS, SSP: "WEBRTC"
    AFE_NS_MODE_NET = 1,    // please use model name of NSNET
} afe_ns_mode_t;

typedef enum {
    AFE_AGC_MODE_WEBRTC = 0,  // WEBRTC AGC
    AFE_AGC_MODE_WAKENET = 1, // AGC gain is calculated by wakenet model if wakenet is activated
} afe_agc_mode_t;


#ifdef __cplusplus
}
#endif

