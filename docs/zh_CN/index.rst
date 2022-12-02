ESP-SR 用户指南
=================

:link_to_translation:`en:[English]`

这里是乐鑫 `ESP-SR <https://github.com/espressif/esp-sr>`__ 本文档旨在指导用户基于 ESP32 系列芯片为基础搭建　AI 语音解决方案，将通过一个简单的示例展示如何使用ESP-SR中的算法和模型。

概述
----

ESP-SR　支持以下模块：

    * `声学前端算法 <audio_front_end/README.md>`__
    * `唤醒词检测 <wake_word_engine/README.md>`__
    * `唤醒词检测 <speech_command_recognition/README>`__
    * 语音合成　(目前只支持中文)

准备工作
--------

硬件:
~~~~~~

    * 一款音频开发版，推荐使用　ESP32-S3-Korvo-1 或者　ESP32-S3-Korvo-２
    * **USB 数据线** (A 转　Micro-B)
    * 电脑　(Linux)

..　note:: 目前一些开发板使用的是 USB Type C 接口。请确保使用合适的数据线连接开发板！

软件：
~~~~~~

    * 下载 `ESP-SKAINET <https://github.com/espressif/esp-skainet>`，ESP-SR 将作为 ESP-SKAINET 的组件被一起下载。
    * 配置安装 ESP－IDF,推荐使用 ESP-SKAINET 中包含的版本。安装方法请参考 ｀ESP-IDF 快速入门 ＜https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html＞｀。


编译运行一个示例
----------------

    * 进入 ESP-SKAINET/examples/cn_speech_commands_recognition 目录。
    * 参考该示例目录下的配置和编译说明，运行该示例。
    * 该示例为中文命令指令识别示例，通过说唤醒词(Hi,乐鑫)，触发语音指令识别。当一段时间没有语音指令后，会关闭语音指令识别，并等待下一次唤醒词触发。
　　　
.. toctree::
    :hidden:

    AFE 声学前端算法 <audio_front_end/README>
    唤醒词模型 <wake_word_engine/README>
    定制化唤醒词 <wake_word_engine/ESP_Wake_Words_Customization>
    语音指令 <speech_command_recognition/README>
    模型加载方式 <flash_model/README>
    麦克风设计指南 <audio_front_end/Espressif_Microphone_Design_Guidelines>
    测试报告 <test_report/README>
    性能测试 <benchmark/README>