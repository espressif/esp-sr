# Audio Front-end Framework[[中文]](./README_CN.md)

Espressif Audio Front-end (AFE) algorithm framework is independently developed by ESPRESSIF AI Lab. Based on ESP32 series chips, the framework can provide high-quality and stable audio data to the host.

---

## Summary

Espressif AFE provides the most convenient way to do audio front-end processing on ESP32 series chips. Espressif AFE framework stably transfers high-quality audio data to the host for further wake-up or speech recognition.

The functions supported in Espressif AFE are as follows:

![overview](../img/AFE_overview.png)

The workflow of Espressif AFE is as follows:

![overview](../img/AFE_workflow.png)

The workflow of Espressif AFE can be divided into four parts:

- AFE creation and initialization
- AFE feed: Input audio data and will run AEC in the feed function
- Internal: In case of wake-up recognition/single microphone speech noise reduction scene, BSS/NS algorithm processing will be carried out; If it is a multi microphone speech noise reduction scene, BSS/MISO algorithm processing will be carried out.
- AFE fetch: Return the audio data after processing and the output value. In the wake-up recognition scenario, VAD processing and wake-up word detection will be carried out inside the fetch. The specific behavior depends on the config of `afe_config_t` structure; If it is a multi microphone speech noise reduction scene, noise reduction will be carried out. (Note: `wakenet_Init` and `voice_communication_Init` cannot be configured to true at the same time)

**Note:** `afe->feed()` and `afe->fetch()` are visible to users, while `internal BSS/NS/MISO task` is invisible to users.

> AEC runs in `afe->feed()` function; If aec_init is configured as false, BSS/NS will run in the afe->feed() function.  
> BSS/NS/MISO is an independent task in AFE;  
> The results of VAD/WakeNet and the audio data after processing are obtained by `afe->fetch()` function.  

### Select AFE handle

Espressif AFE supports both single MIC and dual MIC scenarios, and the algorithm module can be flexibly configured. The internal task of single MIC applications is processed by NS, and the internal task of dual MIC applications is processed by BSS. If the dual microphone scenario is configured for voice noise reduction (i.e. `wakenet_init=false, voice_communication_init=true`), the  miso internal task will be added.

- Get AFE handle

		esp_afe_sr_iface_t *afe_handle = &ESP_AFE_HANDLE;

### Input Audio data

The AFE supports two kinds of scenarios: single MIC and dual MIC. The  number of channels  can be configured according to the audio of `afe->feed()`.  Modify method: It can modify the `pcm_config` configuration in macro `AFE_CONFIG_DEFAULT()`. It supports the following configuration combinations (Note: It must meet `total_ch_num = mic_num + ref_num`) :

> total_ch_num=1,  mic_num=1,  ref_num=0
> total_ch_num=2,  mic_num=1,  ref_num=1
> total_ch_num=2,  mic_num=2,  ref_num=0
> total_ch_num=3,  mic_num=2,  ref_num=1

(Note: total_ch_num: the number of total channels, mic_num: the number of microphone channels, ref_num: the number of reference channels)

At present, the AEC only support one reference data , so ref_num can only be 0 or 1.

- AFE single MIC

   - Input audio data format: 16KHz, 16bit, two channels (one is mic data, another is reference data) ; If AEC is not required and the audio does not contain reference data. The input data can only have one channel of MIC data, and the ref_num need to be set 0.
   - The input data frame length will vary according to the algorithm module configured by the user. Users can use `afe->get_feed_chunksize()` to get the number of sampling points (the data type of sampling points is int16).

 The input data is arranged as follows:

 <img src="../img/AFE_mode_0.png" height = "100" align=center />  

- AFE dual MIC

   - Input audio data format: 16KHz, 16bit, three channels (two are mic data, another is reference data) ; If AEC is not required and the audio does not contain reference data. The input data can only have two channels of MIC data, and the ref_num need to be set 0.
   - The input data frame length will vary according to the algorithm module configured by the user. Users can use `afe->get_feed_chunksize()` to get the number of sampling points (the data type of sampling points is int16).

  The input data is arranged as follows:

 <img src="../img/AFE_mode_other.png" height = "70" align=center />     

Note: the converted data size is: `afe->get_feed_chunksize * channel number * sizeof(short)`

### AEC Introduction

The AEC (Acoustic Echo Cancellation) algorithm supports maximum two-mic processing, which can effectively remove the echo in the mic input signal, and help with further speech recognition.

### NS (noise suppression)

NS algorithm supports single-channel processing and can suppress the non-human noise in single-channel audio, especially for steady noise.

### BSS (Blind Source Separation)

BSS algorithm supports dual-channel processing, which can well separate the target sound source from the rest of the interference sound, so as to extract the useful audio signal and ensure the quality of the subsequent speech.

### MISO (Multi Input Single Output) 

Miso algorithm supports dual channel input and single channel output. It is used to select a channel of audio output with high signal-to-noise ratio when there is no wakenet enable in the dual mic scene.

### VAD (Voice Activity Detection)

VAD algorithm supports real-time output of the voice activity state of the current frame.

### WakeNet or Bypass

Users can choose whether to detect wake words in AFE. When calling `afe->disable_wakenet(afe_data)`, it will enter bypass mode, and the WakeNet will not run.

### Output Audio

The output audio of AFE is single-channel data. When WakeNet is enabled, AFE will output single-channel data with human voice.

---

## Quick Start

### 1. Define afe_handle

`afe_handle ` is the function handle that the user calls the AFE interface. Therefore, the first step is to obtain `afe_handle`.

	    esp_afe_sr_iface_t *afe_handle = &ESP_AFE_HANDLE;

### 2. Configure AFE

Get the configuration of AFE:

	afe_config_t afe_config = AFE_CONFIG_DEFAULT();
	
Users can adjust the switch of each algorithm module and its corresponding parameters in macros ` AFE_ CONFIG_ DEFAULT ()`:

```
#define AFE_CONFIG_DEFAULT() { \
    .aec_init = true, \
    .se_init = true, \
    .vad_init = true, \
    .wakenet_init = true, \
    .voice_communication_init = false, \
    .vad_mode = VAD_MODE_3, \
    .wakenet_model = (esp_wn_iface_t *)&WAKENET_MODEL, \
    .wakenet_coeff = (void *)&WAKENET_COEFF, \
    .wakenet_mode = DET_MODE_2CH_90, \
    .afe_mode = SR_MODE_LOW_COST, \
    .afe_perferred_core = 0, \
    .afe_perferred_priority = 5, \
    .afe_ringbuf_size = 50, \
    .memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM, \
    .agc_mode = AFE_MN_PEAK_AGC_MODE_2, \
    .pcm_config.total_ch_num = 3, \
    .pcm_config.mic_num = 2, \
    .pcm_config.ref_num = 1, \
}
```

- aec_init: Whether the AEC algorithm is enabled.

- se_init: Whether the BSS/NS algorithm is enabled.

- vad_init: Whether the VAD algorithm is enabled.

- wakenet_init: Whether the wake algorithm is enabled.

- voice_communication_init: Whether voice communication noise reduction is enabled. It cannot be enabled with wakenet_init at the same time.

- vad_mode: The VAD operating mode. The bigger, the more radical.

- wakenet_model/wakenet_coeff/wakenet_mode: Use `idf.py menuconfig` to choose WakeNet model. Please refer to：[WakeNet](../wake_word_engine/README.md)

- afe_mode: Espressif AFE supports two working modes: SR_MODE_LOW_COST, SR_MODE_HIGH_PERF. See the afe_sr_mode_t enumeration for details.

	- SR_MODE_LOW_COST: The quantified version occupies less resources.

	- SR_MODE_HIGH_PERF: The non-quantified version occupies more resources.
	
        **ESP32 only supports SR_MODE_HIGH_PERF;   
        And ESP32S3 supports both of the modes **

- afe_perferred_core: The internal BSS/NS/MISO algorithm of AFE will be running on which CPU core.

- afe_perferred_priority: The running priority of BSS/NS/MISO algorithm task.

- afe_ringbuf_size: Configuration of internal ringbuf size.

- memory_alloc_mode: Memory allocation mode. Three values can be configured:

	- AFE_MEMORY_ALLOC_MORE_INTERNAL: More memory is allocated from internal ram.
	
	- AFE_MEMORY_ALLOC_INTERNAL_PSRAM_BALANCE: Part of memory is allocated from internal psram.
	
	- AFE_MEMORY_ALLOC_MORE_PSRAM: Most of memory is allocated from external psram.
	
- agc_mode: Configuration for linear audio amplification. Four values can be configured:

	- AFE_MN_PEAK_AGC_MODE_1: Linearly amplify the audio  which will fed to multinet. The peak value is -5 dB.
	
	- AFE_MN_PEAK_AGC_MODE_2: Linearly amplify the audio  which will fed to multinet. The peak value is -4 dB.
	
	- AFE_MN_PEAK_AGC_MODE_3: Linearly amplify the audio  which will fed to multinet. The peak value is -3 dB.
	
	- AFE_MN_PEAK_NO_AGC: No amplification.
	
- pcm_config: Configure according to the audio that fed by `afe->feed()`. This structure has three member variables to configure:

	- total_ch_num: Total number of audio channels，total_ch_num = mic_num + ref_num。
	
	- mic_num: The number of microphone channels. It only can be set to 1 or 2.
	
	- ref_num: The number of reference channels. It only can be set to 0 or 1.

### 3. Create afe_data

The user uses the `afe_handle->create_from_config(&afe_config)` function to obtain the data handle, which will be used internally in afe, and the parameters passed in are the configurations obtained in step 2 above.

```
/**
 * @brief Function to initialze a AFE_SR instance
 * 
 * @param afe_config        The config of AFE_SR
 * @returns Handle to the AFE_SR data
 */
typedef esp_afe_sr_data_t* (*esp_afe_sr_iface_op_create_from_config_t)(afe_config_t *afe_config);
```

### 4. feed audio data

After initializing AFE, users need to input audio data into AFE by `afe_handle->feed()` function for processing.

The input audio size and layout format can refer to the step **Input Audio data**.

```
/**
 * @brief Feed samples of an audio stream to the AFE_SR
 *
 * @Warning  The input data should be arranged in the format of channel interleaving.
 *           The last channel is reference signal if it has reference data.
 *
 * @param afe   The AFE_SR object to query
 * 
 * @param in    The input microphone signal, only support signed 16-bit @ 16 KHZ. The frame size can be queried by the 
 *              `get_feed_chunksize`.
 * @return      The size of input
 */
typedef int (*esp_afe_sr_iface_op_feed_t)(esp_afe_sr_data_t *afe, const int16_t* in);

```

Get the number of audio channels:

 `afe_handle->get_total_channel_num()` function can provide the number of channels that need to be put into `afe_handle->feed()` function. Its return value is equal to `pcm_config.mic_num + pcm_config.ref_num` in AFE_CONFIG_DEFAULT()

```
/**
 * @brief Get the total channel number which be config
 * 
 * @param afe   The AFE_SR object to query
 * @return      The amount of total channels
 */
typedef int (*esp_afe_sr_iface_op_get_total_channel_num_t)(esp_afe_sr_data_t *afe);
```

### 5. fetch audio data

Users can get the processed single-channel audio by `afe_handle->fetch()` function.

The number of data sampling points of fetch (the data type of sampling point is int16) can be got by `afe_handle->get_fetch_chunksize`.

```
/**
 * @brief Get the amount of each channel samples per frame that need to be passed to the function
 *
 * Every speech enhancement AFE_SR processes a certain number of samples at the same time. This function
 * can be used to query that amount. Note that the returned amount is in 16-bit samples, not in bytes.
 *
 * @param afe The AFE_SR object to query
 * @return The amount of samples to feed the fetch function
 */
typedef int (*esp_afe_sr_iface_op_get_samp_chunksize_t)(esp_afe_sr_data_t *afe);
```

Please pay attention to the return value of `afe_handle->fetch()`: 

- AFE_FETCH_ERROR: Get empty data, please try again.
- AFE_FETCH_CHANNEL_VERIFIED: Audio channel confirmation (This value is not returned while use single mic wakenet.)
- AFE_FETCH_NOISE: Noise detected.
- AFE_FETCH_SPEECH: Speech detected.
- AFE_FETCH_WWE_DETECTED: Wakeup detected.
- ...

```
/**
 * @brief fetch enhanced samples of an audio stream from the AFE_SR
 *
 * @Warning  The output is single channel data, no matter how many channels the input is.
 *
 * @param afe   The AFE_SR object to query
 * @param out   The output enhanced signal. The frame size can be queried by the `get_fetch_chunksize`.
 * @return      The state of output, please refer to the definition of `afe_fetch_mode_t`
 */
typedef afe_fetch_mode_t (*esp_afe_sr_iface_op_fetch_t)(esp_afe_sr_data_t *afe, int16_t* out);
```

### 6. Usage of WakeNet  

When users need to perform other operations after wake-up, such as offline or online speech recognition. They can pause the operation of WakeNet to reduce the CPU resource consumption.

Users can call `afe_handle->disable_wakenet(afe_data)` to stop WakeNet, or call `afe_handle->enable_wakenet(afe_data)` to enable WakeNet.

In addition, ESP32S3 chip supports switching between two wakenet words. (Note: ESP32 chip only supports one wake-up word and does not support switching). After  AFE initialization, the ESP32S3 can switch to the second wakenet word by `afe_handle->set_wakenet(afe_data, SECOND_WAKE_WORD)`. How to configure two wakenet words, please refer to: [flash_model](../flash_model/README.md)

### 7. Usage of AEC

The usage of AEC is similar to that of WakeNet. Users can disable or enable AEC according to requirements.

- Disable AEC

	afe->disable_aec(afe_data);

- Enable AEC

	afe->enable_aec(afe_data);

