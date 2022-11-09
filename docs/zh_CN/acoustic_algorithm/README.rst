声学算法介绍
============

esp-sr 中提供的声学算法包括语音活动检测 (VAD)、自适应增益控制 (AGC)、声学回声消除 (AEC)、噪声抑制 (NS) 和麦克风阵列语音增强 (MASE)。 VAD、AGC、AEC 和 NS 支持单麦克风和多麦克风开发板，MASE 仅支持多麦克风板。

VAD
---

概述
~~~~

VAD将一个音频流作为输入，并输出该流的某一帧是否包含音频的预测。

API 参考
~~~~~~~~~~~~~

头文件
^^^^^^

-  esp_vad.h

函数
^^^^

-  ``vad_handle_t vad_create(vad_mode_t vad_mode)``

    **定义**

    VAD 句柄的初始化。

    **范围**

    -  vad_mode：VAD的工作模式，VAD_MODE_0到VAD_MODE_4，数值越大表示VAD越激进。

    **返回值**

    vad_handle_t

-  ``vad_state_t vad_process(vad_handle_t inst, int16_t *data, int sample_rate_hz, int one_frame_ms);``

    **定义**

    处理一帧的 VAD。

    **范围**

    - inst：VAD句柄。
    - data: 保存输入和输出音频流的缓冲区。
    - sample_rate_hz: 采样频率（Hz）可以是32000、16000、8000，默认是16000。
    - one_frame_ms: 音频处理的长度可以是10ms、20ms、30ms，默认：30。

    **返回值**

    -  VAD_SILENCE if no voice
    -  VAD_SPEECH if voice is detected

-  ``void vad_destroy(vad_handle_t inst)``

    **定义**

    -  销毁 VAD 句柄.

    **范围**

    -  inst：要销毁的VAD句柄。

AGC
---

.. _overview-1:

概述
~~~~~~~~

AGC将音频信号的音量保持在一个稳定的水平，以避免信号过大而被削掉或过小而无法触发语音识别器的情况。

.. _api-reference-1:

API 参考
~~~~~~~~~~~~~

-  ``void *esp_agc_open(int agc_mode, int sample_rate)``

    **定义**

    AGC句柄的初始化。  

    **范围**

    - agc_mode：AGC的工作模式，3表示启用AGC，0表示禁用。
    - sample_rate：音频信号的采样率。

    **返回值**

    -  AGC 句柄.

-  ``int esp_agc_process(void *agc_handle, short *in_pcm, short *out_pcm, int frame_size, int sample_rate)``

    **定义**

    对一帧的AGC进行分配。

    **范围**

    - agc_handle: AGC手柄。
    - in_pcm: 输入音频流。
    - out_pcm：输出音频流。
    - frame_size: 信号帧的长度，单位是ms。
    - sample_rate：信号的采样率，单位为Hz。

    **返回值**

    - 返回 0 如果 AGC processing 成功, -1 如果失败; -2 和 -3 分别表示采样率和帧大小的无效输入。

-  ``void esp_agc_clse(void *agc_handle)``

    **定义**

    - 销毁一个AGC句柄。

    **范围**

    -  agc_handle: 销毁AGC句柄。

AEC
---

.. _overview-2:

概述
~~~~~~~~

AEC抑制了电路板上的扬声器所播放的声音的回声。

.. _api-reference-2:

API 参考
~~~~~~~~~~~~~

-  ``aec_handle_t aec_create(int sample_rate, int frame_length, int filter_length)``

    **定义**

    AEC 句柄的初始化。

    **范围**

    -  sample_rate: audio signal sampling rate.
    -  frame_length: audio frame length in ms.
    -  filter_length: the length of adaptive filter in AEC.

    **返回值**

    Handle to AEC.

-  ``aec_create_t aec_create_multimic(int sample_rate, int frame_length, int filter_length, int nch)``

    **定义**

    AEC 句柄的初始化。

    **范围**

    - sample_rate：音频信号采样率。
    - frame_length：以毫秒为单位的音频帧长度。
    - filter_length：AEC 中自适应滤波器的长度。
    - nch：要处理的信号的通道数。

    **返回值**

    Handle to AEC.

-  ``void aec_process(aec_handle_t inst, int16_t *indata, int16_t *refdata, int16_t *outdata)``

    **定义**

    一帧的AEC处理。

    **范围**

    - inst：AEC 手柄。
    - indata：输入音频流，可以是单声道或多声道，取决于初始化时定义的声道号。
    - refdata：要从输入中取消的参考信号。
    - outdata：输出音频流，通道数与indata相同。

-  ``void aec_destroy(aec_handle_t inst)``

    **定义**

    AEC 句柄的破坏。

    **范围**

    -inst：要销毁的 AEC 句柄。

NS
--

.. _overview-3:

概述
~~~~~~~~

单通道语音增强。如果电路板上有多个麦克风可用，建议使用 MASE 进行噪声抑制。

.. _api-reference-3:

API 参考
~~~~~~~~~~~~~

-  ``ns_handle_t ns_pro_create(int frame_length, int mode)``

    **定义**

    创建更强大的噪声抑制算法的实例。

    **范围**

    - frame_length_ms：以毫秒为单位的音频帧长度。
    - mode：0：轻度，1：中度，2：激进

    **返回值**

    Handle to NS.

-  ``void ns_process(ns_handle_t inst, int16_t *indata, int16_t *outdata)``

    **定义**

    NS 处理一帧。

    **范围**

    - inst：NS 句柄。
    - indata：输入音频流。
    - outdata：输出音频流。

-  ``void ns_destroy(ns_handle_t inst)``

    **定义**

    NS句柄的破坏。

    **范围**

    - inst：要销毁的 NS 句柄。
