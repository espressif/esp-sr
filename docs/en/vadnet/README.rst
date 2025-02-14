Voice Activaty Detection Model
==============================

:link_to_translation:`zh_CN:[中文]`

VADNet is a Voice Activaty Detection model built upon neural network for low-power embedded MCUs. 

Overview
--------

VADNet uses a model structure and data processing flow similar to WakeNet, for more details, you can refer to :doc:`WakeNet <../wake_word_engine/README>`

VADNet is trained by about 5,000 hours of Chinese data, 5,000 hours of English data, and 5,000 hours of multilingual data.


Use VADNet
-----------

-  Select VADNet model

    ::
        
        idf.py menuconfig
        ESP Speech Recognition -> Select voice activity detection -> voice activity detection (vadnet1 medium).

-  Run VADNet

    VADNet is currently included in the :doc:`AFE <../audio_front_end/README>`, which is enabled by default, and returns the detection results through the AFE fetch interface.

    The common vad setting is as follows:

    ::
        
        afe_config->vad_init = true           // Whether to initial vad in AFE pipeline. Default is true.
        afe_config->vad_min_noise_ms = 1000;  // The minimum duration of noise or silence in ms.
        afe_config->vad_min_speech_ms = 128;  // The minimum duration of speech in ms.
        afe_config->vad_delay_ms = 128;       // The delay between the first frame trigger of VAD and the first frame of speech data.
        afe_config->vad_mode = VAD_MODE_1;    // The larger the mode, the higher the speech trigger probability.
    
    If users want to enable/disable/reset VADNet temporarily, please use:

    ::

        afe_handle->disable_vad(afe_data);  // disable VADNet
        afe_handle->enable_vad(afe_data);   // enable VADNet
        afe_handle->reset_vad(afe_data);    // reset VADNet status

- VAD Cache

    There are two issues in the VAD settings that can cause a delay in the first frame trigger of speech.

    1. The inherent delay of the VAD algorithm itself. VAD cannot accurately trigger speech on the first frame and may delay by 1 to 3 frames.
    2. To avoid false triggers, the VAD is triggered when the continuous trigger duration reaches the `vad_min_speech_ms` parameter in AFE configuation.

    Due to the above two reasons, directly using the first frame trigger of VAD may cause the first word to be truncated. 
    To avoid this case, AFE V2.0 has added a VAD cache. You can determine whether a VAD cache needs to be saved by checking the vad_cache_size

    ::
       
        afe_fetch_result_t* result = afe_handle->fetch(afe_data); 
        if (result->vad_cache_size > 0) {
            printf("vad cache size: %d\n", result->vad_cache_size);
            fwrite(result->vad_cache, 1, result->vad_cache_size, fp);
        }

        printf("vad state: %s\n", res->vad_state==VAD_SILENCE ? "noise" : "speech");


Resource Occupancy
------------------

For the resource occupancy for this model, see :doc:`Resource Occupancy <../benchmark/README>`.