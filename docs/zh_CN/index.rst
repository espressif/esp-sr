ESP-SR 用户指南
=================

:link_to_translation:`en:[English]`

这里是乐鑫 `ESP-SR <https://github.com/espressif/esp-sr>`__ 本文档将介绍乐鑫以 ESP32 和 ESP32S3 系列芯片为基础推出的 AI 语音解决方案。从前端音频处理，到语音命令词识别，从硬件设计建议，到性能测试方法，全面介绍乐鑫在 AI 语音方面的系统性工作，为用户在乐鑫 ESP32 系列芯片及开发板上构建 AIoT 应用，提供有力参考。

乐鑫 AFE 算法已通过亚马逊 Alexa 内置设备的 Software Audio Front-End 认证。可在语音通话和语音识别等场景下提供高质量音频输入。 AFE 算法中内置的唤醒模块可实现本地语音唤醒功能，且支持唤醒词定制。乐鑫语音命令词识别模型可支持最多 200 条中英文命令词，且可在运行中修改命令词，为应用带来极大灵活性。

基于多年硬件设计与开发经验，乐鑫可为客户提供语音开发板 Review 服务，并乐意为客户自制开发板进行测试和调优，以展现算法最优性能。客户也可按照乐鑫提供的测试方式和自测结果，对开发板和整机产品进行深入评估。

.. toctree::
    :hidden:

    AFE 声学前端算法 <audio_front_end/README>
    唤醒词模型 <wake_word_engine/README>
    定制化唤醒词 <wake_word_engine/ESP_Wake_Words_Customization>
    语音指令 <speech_command_recognition/README>
    模型加载方式 <flash_model/README>
    麦克风设计指南 <audio_front_end/Espressif_Microphone_Design_Guidelines>
    测试报告 <test_report/README>
    性能测试 <performance_test/README>