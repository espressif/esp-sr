语音活动检测模型
==============================

:link_to_translation:`en:[English]`

VADNet 是一个基于神经网络的语音活动检测模型，专为低功耗嵌入式MCU设计。

概述
--------

VADNet 采用了与 WakeNet 相似的模型结构和数据处理流程，更多实现细节可参考 :doc:`WakeNet <../wake_word_engine/README>` 中的说明。

VADNet 训练数据包括了大约5000小时中文数据， 5000 小时英文数据，还有5000小时的多语言数据。

使用VADNet
-----------

-  选择VADNet模型

    ::
        
        idf.py menuconfig
        ESP Speech Recognition -> Select voice activity detection -> voice activity detection (vadnet1 medium).

-  运行VADNet

    VADNet 当前集成在 :doc:`音频前端处理模块 <../audio_front_end/README>` 中，默认处于启用状态，通过AFE的fetch接口返回检测结果。

    常用VAD参数配置如下：

    ::
        
        afe_config->vad_init = true           // 是否在AFE流水线中初始化VAD，默认启用
        afe_config->vad_min_noise_ms = 1000;  // 噪声/静音段的最短持续时间（毫秒）
        afe_config->vad_min_speech_ms = 128;  // 语音段的最短持续时间（毫秒）
        afe_config->vad_delay_ms = 128;       // VAD首帧触发到语音首帧数据的延迟量
        afe_config->vad_mode = VAD_MODE_1;    // 模式值越大，语音触发概率越高
    
    如需临时启用/禁用/重置VADNet，可使用以下接口：

    ::

        afe_handle->disable_vad(afe_data);  // 禁用VAD
        afe_handle->enable_vad(afe_data);    // 启用VAD
        afe_handle->reset_vad(afe_data);    // 重置VAD状态

- VAD缓存与检测

    VAD配置中的两个特性可能导致语音首帧触发延迟：

    1. VAD算法固有延迟：VAD无法在首帧精准触发，可能有1-3帧延迟
    2. 防误触机制：需持续触发时间达到配置参数`vad_min_speech_ms`才会正式触发

    为避免上述原因导致语音首字截断，AFE V2.0新增了VAD缓存机制。可通过检查vad_cache_size判断是否需要保存VAD缓存：

    ::
       
        afe_fetch_result_t* result = afe_handle->fetch(afe_data); 
        if (result->vad_cache_size > 0) {
            printf("vad cache size: %d\n", result->vad_cache_size);
            fwrite(result->vad_cache, 1, result->vad_cache_size, fp);
        }

        printf("vad state: %s\n", res->vad_state==VAD_SILENCE ? "noise" : "speech");


资源占用
------------------

本模型的资源占用情况请参考 :doc:`资源占用说明 <../benchmark/README>`。