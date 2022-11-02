# Acoustic Algorithm Introduction

Acoustic algorithms provided in esp-sr include voice activity detection (VAD), adaptive gain control (AGC), acoustic echo cancellation (AEC), noise suppression (NS), and mic-array speech enhancement (MASE). VAD, AGC, AEC, and NS are supported with either single-mic and multi-mic development board, MASE is supported with multi-mic board only.

## VAD

### Overview

VAD takes an audio stream as input, and outputs the prediction that a frame of the stream contains audio or not.  

### API Reference

#### Header

- esp_vad.h

#### Function

- `vad_handle_t vad_create(vad_mode_t vad_mode)`

	**Definition**

	Initialization of VAD handle. 

	**Parameter**

	- vad_mode: operating mode of VAD, VAD_MODE_0 to VAD_MODE_4, larger value indicates more aggressive VAD.

    **Return**

    Handle to VAD.

- `vad_state_t vad_process(vad_handle_t inst, int16_t *data, int sample_rate_hz, int one_frame_ms);`

    **Definition**

    Processing of VAD for one frame.

    **Parameter**

    - inst: VAD handle.
    - data: buffer to save both input and output audio stream.
    - sample_rate_hz: The Sampling frequency (Hz) can be 32000, 16000, 8000, default: 16000.
    - one_frame_ms: The length of the audio processing can be 10ms, 20ms, 30ms, default: 30.

  **Return**

    - VAD_SILENCE if no voice
    - VAD_SPEECH  if voice is detected

- `void vad_destroy(vad_handle_t inst)`

    **Definition**

    Destruction of a VAD handle.

    **Parameter**

    - inst: the VAD handle to be destroyed.

## AGC

### Overview

AGC keeps the volume of audio signal at a stable level to avoid the situation that the signal is so loud that gets clipped or too quiet to trigger the speech recognizer.

### API Reference

- `void *esp_agc_open(int agc_mode, int sample_rate)`

  **Definition**

    Initialization of AGC handle.

  **Parameter**

    - agc_mode: operating mode of AGC, 3 to enable AGC and 0 to disable it.
    - sample_rate: sampling rate of audio signal.

  **Return**

    - AGC handle.

- `int esp_agc_process(void *agc_handle, short *in_pcm, short *out_pcm, int frame_size, int sample_rate)`

  **Definition**

    Pocessing of AGC for one frame.

  **Parameter**

    - agc_handle: AGC handle.
    - in_pcm: input audio stream.
    - out_pcm: output audio stream.
    - frame_size: signal frame length in ms.
    - sample_rate: signal sampling rate in Hz.

  **Return**

    Return 0 if AGC processing succeeds, -1 if fails; -2 and -3 indicate invalid input of sample_rate and frame_size, respectively.

- `void esp_agc_clse(void *agc_handle)`

    **Definition**

    Destruction of an AGC handle.

    **Parameter**

    - agc_handle: the AGC handle to be destroyed.

## AEC

### Overview

AEC suppresses echo of the sound played by the speaker of the board.

### API Reference

- `aec_handle_t aec_create(int sample_rate, int frame_length, int filter_length)`

    **Definition**

    Initialization of AEC handle.

    **Parameter**

    - sample_rate: audio signal sampling rate.
    - frame_length: audio frame length in ms.
    - filter_length: the length of adaptive filter in AEC.

    **Return**

    Handle to AEC.

- `aec_create_t aec_create_multimic(int sample_rate, int frame_length, int filter_length, int nch)`

    **Definition**

    Initialization of AEC handle.

    **Parameter**

    - sample_rate: audio signal sampling rate.
    - frame_length: audio frame length in ms.
    - filter_length: the length of adaptive filter in AEC.
    - nch: number of channels of the signal to be processed.

    **Return**

    Handle to AEC.

- `void aec_process(aec_handle_t inst, int16_t *indata, int16_t *refdata, int16_t *outdata)`
    
    **Definition**

    Processing of AEC for one frame.

    **Parameter**

    - inst: AEC handle.
    - indata: input audio stream, which could be single- or multi-channel, depending on the channel number defined on initialization.
    - refdata: reference signal to be cancelled from the input.
    - outdata: output audio stream, the number of channels is the same as indata.

- `void aec_destroy(aec_handle_t inst)`

    **Definition**

    Destruction of an AEC handle.

    **Parameter**

    - inst: the AEC handle to be destroyed.

## NS

### Overview

Single-channel speech enhancement. If multiple mics are available with the board, MASE is recommened for noise suppression.

### API Reference

- `ns_handle_t ns_pro_create(int frame_length, int mode)`

    **Definition**

    Creates an instance of the more powerful noise suppression algorithm.

    **Parameter**

    - frame_length_ms: audio frame length in ms.
    - mode: 0: Mild, 1: Medium, 2: Aggressive  

    **Return**

    Handle to NS.

- `void ns_process(ns_handle_t inst, int16_t *indata, int16_t *outdata)`

    **Definition**

    Prodessing of NS for one frame.

    **Parameter**

    - inst: NS handle.
    - indata: input audio stream.
    - outdata: output audio stream.

- `void ns_destroy(ns_handle_t inst)`

    **Definition**

    Destruction of a NS handle.

    **Parameter**

    - inst: the NS handle to be destroyed.