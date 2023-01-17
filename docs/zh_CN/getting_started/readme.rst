入门指南
========

:link_to_translation:`en:[English]`

乐鑫 `ESP-SR <https://github.com/espressif/esp-sr>`__ 可以帮助用户基于 ESP32 系列芯片或 ESP32-S3 系列芯片，搭建 AI 语音解决方案。本文档将通过一些简单的示例，展示如何使用 ESP-SR 中的算法和模型。

概述
----

ESP-SR 支持以下模块：

* :doc:`声学前端算法 AFE <../audio_front_end/README>`
* :doc:`唤醒词检测 WakeNet <../wake_word_engine/README>`
* :doc:`命令词识别 MultiNet <../speech_command_recognition/README>`
* :doc:`语音合成（目前只支持中文）<../speech_synthesis/readme>`

准备工作
--------

必备硬件
~~~~~~~~

.. list::

   :esp32s3: - 一款音频开发版，推荐使用 ESP32-S3-Korvo-1 或者 ESP32-S3-Korvo-２
   :esp32: - 一款音频开发版，推荐使用 ESP32-Korvo
   - USB 2.0 数据线（标准 A 型转 Micro-B 型）
   - 电脑（Linux）

.. note::
   目前一些开发板使用的是 USB Type C 接口。请确保使用合适的数据线连接开发板！

必备软件
~~~~~~~~

* 下载 `ESP-SKAINET <https://github.com/espressif/esp-skainet>`__，ESP-SR 将作为 ESP-SKAINET 的组件被一起下载。
* 配置安装 ESP-IDF，推荐使用 ESP-SKAINET 中包含的版本。安装方法请参考 `ESP-IDF 编程指南 <https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/index.html>`__ 中的 `快速入门 <https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/get-started/index.html>`__ 小节。


编译运行一个示例
----------------

* 进入 `ESP-SKAINET/examples/cn_speech_commands_recognition <https://github.com/espressif/esp-skainet/tree/master/examples/cn_speech_commands_recognition>`__ 目录。
* 参考该示例目录下的配置和编译说明，运行该示例。
* 该示例为中文命令指令识别示例，通过说唤醒词（Hi,乐鑫），触发语音指令识别。注意，当一段时间没有语音指令后，语音指令识别功能将关闭，并等待下一次唤醒词触发。