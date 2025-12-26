#pragma once
#include "esp_aec.h"
#include "esp_agc.h"
#include "esp_nsn_models.h"
#include "esp_vad.h"
#include "esp_vadn_models.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "model_path.h"
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
    AFE_TYPE_VC = 1, // Voice communication scenarios, 16KHz input, including nonlinear noise suppression
    AFE_TYPE_VC_8K = 2, // Voice communication scenarios, 8KHz input, note that the input data must be 8KHz
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

/**
 * @brief Function to get the debug audio data
 *
 * @param data        The debug audio data which don't be modify. It should be copied away as soon as possible that
 * avoid blocking for too long.
 * @param data_size   The number of bytes of data.
 * @returns
 */
typedef void (*afe_debug_hook_callback_t)(const int16_t *data, int data_size);

typedef enum {
    AFE_DEBUG_HOOK_MASE_TASK_IN = 0,  // To get the input data of mase task
    AFE_DEBUG_HOOK_FETCH_TASK_IN = 1, // To get the input data of fetch task
    AFE_DEBUG_HOOK_MAX = 2
} afe_debug_hook_type_t;

typedef struct {
    afe_debug_hook_type_t hook_type;         // debug type of hook
    afe_debug_hook_callback_t hook_callback; // callback function which transfer debug audio data
} afe_debug_hook_t;

typedef struct {
    /********** AEC(Acoustic Echo Cancellation) **********/
    bool aec_init;         // Whether to init aec
    aec_mode_t aec_mode;   // The mode of aec, AEC_MODE_SR_LOW_COST or AEC_MODE_SR_HIGH_PERF
    int aec_filter_length; // The filter length of aec

    /********** SE(Speech Enhancement, microphone array processing) **********/
    bool se_init; // Whether to init se

    /********** NS(Noise Suppression) **********/
    bool ns_init;              // Whether to init ns
    char *ns_model_name;       // Model name of ns
    afe_ns_mode_t afe_ns_mode; // Model mode of ns

    /********** VAD(Voice Activity Detection) **********/
    bool vad_init;          // Whether to init vad
    vad_mode_t vad_mode;    // The value can be: VAD_MODE_0, VAD_MODE_1, VAD_MODE_2, VAD_MODE_3, VAD_MODE_4
    char *vad_model_name;   // The model name of vad, If it is null, WebRTC VAD will be used.
    int vad_min_speech_ms;  // The minimum duration of speech in ms. It should be bigger than 32 ms, default: 128 ms
    int vad_min_noise_ms;   // The minimum duration of noise or silence in ms. It should be bigger than 64 ms, default:
                            // 1000 ms
    int vad_delay_ms;       // The delay of the first speech frame in ms, default: 128 ms
                            // If you find vad cache can not cover all speech, please increase this value.
    bool vad_mute_playback; // If true, the playback will be muted for vad detection. default: false
    bool vad_enable_channel_trigger; // If true, the vad will be used to choose the channel id. default: false

    /********** WakeNet(Wake Word Engine) **********/
    bool wakenet_init;
    char *wakenet_model_name;   // The model name of wakenet 1
    char *wakenet_model_name_2; // The model name of wakenet 2 if has wakenet 2
    det_mode_t wakenet_mode;    // The mode of wakenet

    /********** AGC(Automatic Gain Control) **********/
    bool agc_init; // Whether to init agc
    afe_agc_mode_t
        agc_mode; // The AGC mode for ASR. and the gain generated by AGC acts on the audio after far linear gain.
    int agc_compression_gain_db; // Compression gain in dB (default 9)
    int agc_target_level_dbfs;   // Target level in -dBfs of envelope (default 3, means target level is -3 dBFS)

    /********** General AFE(Audio Front End) parameter **********/
    afe_pcm_config_t pcm_config; // Config the channel num of original data which is fed to the afe feed function.
    afe_mode_t afe_mode;         // The mode of afe， AFE_MODE_LOW_COST or AFE_MODE_HIGH_PERF
    afe_type_t afe_type;         // The mode of afe， AFE_MODE_LOW_COST or AFE_MODE_HIGH_PERF
    int afe_perferred_core;      // The preferred core of afe se task, which is created in afe_create function.
    int afe_perferred_priority;  // The preferred priority of afe se task, which is created in afe_create function.
    int afe_ringbuf_size;        // The ring buffer size: the number of frame data in ring buffer.
    afe_memory_alloc_mode_t memory_alloc_mode; // The memory alloc mode for afe. From Internal RAM or PSRAM
    float afe_linear_gain; // The linear gain for afe output the value should be in [0.1, 10.0]. This value acts
                           // directly on the output amplitude: out_linear_gain * amplitude.
    bool debug_init;
    bool fixed_first_channel; // If true, the channel after first wake-up is fixed to raw data of microphone
                              // otherwise, select channel number by wakenet
} afe_config_t;

/**
 * @brief Get AFE default configuration. The default configuration will enable all algorithms as much as possible based
 * on the chip target and input format. You can manually fine-tune it after creating the configuration
 *
 * The input format:
 * M to represent the microphone channel
 * R to represent the playback reference channel
 * N to represent an unknown or unused channel
 *
 * For example, input_format="MMNR" indicates that the input data consists of four channels,
 * which are the microphone channel, the microphone channel, an unused channel, and the playback channel
 *
 * @param input_format     The input format
 * @param models           Models from partition, which is configured by Kconfig
 * @param type             The type of afe, AFE_TYPE_SR or AFE_TYPE_VC
 * @param mode             The mode of afe, AFE_MODE_LOW_COST or AFE_MODE_HIGH_PERF
 *
 * @return afe_config_t*  The default config of afe
 */
afe_config_t *afe_config_init(const char *input_format, srmodel_list_t *models, afe_type_t type, afe_mode_t mode);

/**
 * @brief Check AFE configuration and make sure it is correct.
 *
 * @warning If there is a configuration conflict, this function will modify some parameters.
 * The guiding behind these modifications is to maintain the highest performance of the output audio and results.
 * And remove the conflict between different algorithms.
 *
 * For example, If input is two-channel data, the SE(BSS) algorithm will be prioritized over the NS algorithm.
 * If SE(BSS) algorithm is deactivated, will only use the first microphone channel.
 *
 * @param afe_config       Input AFE config
 *
 * @return afe_config_t*  The modified AFE config
 */
afe_config_t *afe_config_check(afe_config_t *afe_config);

/**
 * @brief Parse input format
 *
 * @param input_format The input format, same with afe_config_init() function
 * @param pcm_config   The pcm config
 *
 * @return true if the input format is parsed successfully, otherwise false
 */
bool afe_parse_input_format(const char *input_format, afe_pcm_config_t *pcm_config);

/**
 * @brief Parse I2S input data
 *
 * @param data         The input multi channel data
 * @param frame_size   The frame size of input, it is also the size of single channel data
 * @param mic_data     The output microphone data
 * @param ref_data     The output playback reference data
 * @param pcm_config   The pcm config
 *
 */
void afe_parse_input(int16_t *data, int frame_size, int16_t *mic_data, int16_t *ref_data, afe_pcm_config_t *pcm_config);

/**
 * @brief Parse input data, from interleaved arrangement to contiguous arrangement
 *
 * @param data         The input multi channel data
 * @param frame_size   The frame size of input, it is also the size of single channel data
 * @param channel_num  The channel number of data
 * @param out_data     The output data
 *
 */
void afe_parse_data(int16_t *data, int frame_size, int channel_num, int16_t *out_data);

/**
 * @brief Format input data, from contiguous arrangement to interleaved arrangement
 *
 * @param data         The input multi channel data
 * @param frame_size   The frame size of input, it is also the size of single channel data
 * @param channel_num  The channel number of data
 * @param out_data     The output data
 *
 */
void afe_format_data(int16_t *data, int frame_size, int channel_num, int16_t *out_data);

/**
 * @brief Adjust the gain of input data
 *
 * @warning the input data will be modified inplace.
 *
 * @param data         The input audio data
 * @param frame_size   The frame size of input, it is also the size of single channel data
 * @param factor       The gain factor
 *
 * @return int16_t*    The output audio data
 */
int16_t *afe_adjust_gain(int16_t *data, int frame_size, float factor);

/**
 * @brief Adjust the gain of input data
 *
 * @warning the input data will be modified inplace.
 *
 * @param in_data         The input audio data
 * @param in_frame_size   Input data frame size of input
 * @param channel_num     The channel number of input data, which is same as output data
 * @param out_data        The output audio data
 * @param out_frame_size  Onput data frame size of input
 *
 */
void afe_concat_data(int16_t *in_data, int in_frame_size, int channel_num, int16_t *out_data, int out_frame_size);

/**
 * @brief Copy the afe config
 *
 * @param dst_config    The destination afe config
 * @param src_config    The source afe config
 *
 * @return   The destination afe config
 */
afe_config_t *afe_config_copy(afe_config_t *dst_config, const afe_config_t *src_config);

/**
 * @brief Print the afe config
 *
 * @param afe_config    The afe config
 */
void afe_config_print(const afe_config_t *afe_config);

/**
 * @brief Allocate afe config
 *
 * @return The afe config pointer
 */
afe_config_t *afe_config_alloc();

/**
 * @brief Free afe config
 *
 * @param afe_config  The afe config pointer
 */
void afe_config_free(afe_config_t *afe_config);

#ifdef __cplusplus
}
#endif
