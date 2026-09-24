# ESP-SR Speech Recognition Framework

[![Documentation Status](./docs/_static/sr_doc_latest.svg)](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/index.html)
[![Component Registry](https://components.espressif.com/components/espressif/esp-sr/badge.svg)](https://components.espressif.com/components/espressif/esp-sr)

Espressif [ESP-SR](https://github.com/espressif/esp-sr) helps users build AI speech solutions.

Overview
--------

ESP-SR framework includes the following modules:

* [Audio Front-end AFE](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/README.html)
* [Wake Word Engine WakeNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/wake_word_engine/README.html)
* [VAD VADNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/vadnet/README.html)
* [Speech Command Word Recognition MultiNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/speech_command_recognition/README.html)
* [Speech Synthesis](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/speech_synthesis/readme.html)

These algorithms are provided in the form of a component, so they can be integrated into your projects with minimum effort.


News
----
- [2026/08/28]: We added the [GSC beamforming algorithm](https://docs.espressif.com/projects/esp-sr/en/latest/esp32p4/gsc_beamforming/README.html) and updated the [DOA algorithm](https://docs.espressif.com/projects/esp-sr/en/latest/esp32p4/direction_of_arrival/README.html).
- [2026/08/17]: We have released the new WakeNet10 model and optimized the TTS pipeline to improve the final model's generalization performance. WakeNet10 is currently available in w16a16 and w8a16(default) quantization by [esp-dl](https://github.com/espressif/esp-dl).
- [2026/05/09]: Preliminary support for ESP32-S31 target.
- [2026/04/28]: We update a new AEC algorithm for full-duplex scenarios. For performance evaluation and usage instructions, please refer to the [documentation](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/acoustic_echo_cancellation/README.html).
- [2026/04/23]: TTS Pipeline V3 now supports wake word training for Chinese, English, Japanese, and French. Planned support includes Korean, Spanish, Portuguese, German, Russian, and Arabic.
- [2025/10/20]: We add a new model, WakeNet9l, which further improves the response rate of wake words spoken at extremely fast speeds based on WakeNet9. The usage of WakeNet9l is the same as WakeNet9, but its CPU and memory requirements are approximately 1.3 times higher than those of WakeNet9.
- [2025/04/21]: We add a new model WakeNet9s, which can run on chips that do not have PSRAM and do not support SIMD, such as ESP32C3 and ESP32C5. [examples](https://github.com/espressif/esp-skainet/tree/master/examples/wake_word_detection)
- [2025/04/17]: We add a new DOA(Direction of Arrival) algorithm.
- [2025/02/14]: We release **ESP-SR V2.0**. [Migration from ESP-SR V1.* to ESP-SR V2.*](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/migration_guide.html)
- [2025/02/13]: We release **VADNet**, a voice activity detection model. You can use it to replace the WebRTC VAD and improve the performance.

## Wake Word Engine

| Supported Targets | ESP32    | ESP32-S2 | ESP32-S3 | ESP32-S31 | ESP32-P4 | ESP32-C3 | ESP32-C5 | ESP32-C6 |
| ----------------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

Espressif wake word engine **WakeNet** is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always listen to wake words, such as “Alexa”, “Hi,lexin” and “Hi,ESP”. WakeNet9, WakeNet9s, and WakeNet10 models are supported. WakeNet9s is a cost-down version of WakeNet9, with fewer parameters and lower computational requirements. `_tts` suffix means this WakeNet model is trained by TTS samples. `_tts2` suffix means this WakeNet model is trained by TTS Pipeline V2. TTS Pipeline V3 start to support more language.

Espressif offers two ways to customize the wake word, please refer to the following document to choose the one that meets your needs:
[Espressif Speech Wake Words Customization Process](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/wake_word_engine/ESP_Wake_Words_Customization.html) or [Training Wake Words by TTS sample](https://github.com/espressif/esp-sr/issues/88).

The following wake word models have been tested:

| Wake word | WakeNet9 | WakeNet10 |
|:----------|:---------|:----------|
| Hi,乐鑫 | wn9_hilexin | wn10_hilexin |
| Hi,ESP | wn9_hiesp | wn10_hiesp |
| こんにちは ESP | wn9l_ja_konnichihaesp_tts3 | |
| Bonjour ESP | wn9l_fr_bonjouresp_tts3 | |
| Hola ESP | | wn10_es_holaesp |
| 你好小智 | wn9_nihaoxiaozhi_tts | wn10_nihaoxiaozhi |
| 小爱同学 | wn9_xiaoaitongxue | wn10_xiaoaitongxue |
| Alexa | wn9_alexa | wn10_alexa |

See the [complete list of supported wake word models](wakeword_list.md) for all models included in ESP-SR.

**NOTE**:

The product names, logos, and brands associated with the wake words listed in this software are the property of their respective owners. They are shown here solely to provide examples of wake words for users to understand and test Espressif’s Speech Recognition Framework, and do not imply any affiliation with or endorsement by their owners. This software is not affiliated with, endorsed by, or in any way officially connected to any trademark owner. Before any commercial use, you must ensure that you are the lawful rights holder of the relevant wake words or have obtained proper authorization from the lawful rights holder.

本软件所列唤醒词相关的产品名称、标识及品牌均归其各自所有权人所有。展示这些内容仅为了提供唤醒词示例，供用户了解和测试乐鑫提供的语音识别框架，并不表示与权利人存在任何关联或获得认可。本软件与任何商标权人无官方关系。在任何商业使用前，请确保您拥有相关唤醒词的合法权利或已获得合法授权。

## Speech Command Recognition

| Supported Targets | ESP32-S3 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- |

Espressif's speech command recognition model **MultiNet** is specially designed to provide a flexible off-line speech command recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again.

Currently, Espressif **MultiNet** supports up to 300 Chinese or English speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light).

The following MultiNet models are supported in esp-sr:

|language         |         ESP32-S3              |     ESP32-P4/ESP32-S31        |
|:--------------- | :----------------------------:| :----------------------------:|
|Chinese          | mn5q8_cn, mn6_cn, mn7_cn      |  mn7_cn                       |
|English          | mn5q8_en, mn6_en, mn7_en      |  mn7_en                       |


## Audio Front End

| Supported Targets | ESP32    | ESP32-S3 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- |

Espressif Audio Front-End **AFE** integrates AEC (Acoustic Echo Cancellation), VAD (Voice Activity Detection), BSS (Blind Source Separation) and NS (Noise Suppression), NSNET(Deep noise suppression) and other functions. It is designed to be used with the ESP-SR library.

Our two-mic Audio Front-End (AFE) have been qualified as a “Software Audio Front-End Solution” for [Amazon Alexa Built-in devices](https://developer.amazon.com/en-US/alexa/solution-providers/alexa-connect-kit).


## Documentation and Resources

- ESP-SR Documentation: [ESP-SR Documentation](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/index.html)
- Migration Guide: [Migration from V1.* to V2.*](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/migration_guide.html)
- Wake Word Training: [Wake Word Training by TTS Pipeline V2.0](https://github.com/espressif/esp-sr/issues/88)
- Examples: [esp-skainet/examples](https://github.com/espressif/esp-skainet)
- Fbank/MFCC: [esp-dl/audio](https://github.com/espressif/esp-dl/tree/master/esp-dl/audio)
- FFT: [dl_fft](https://components.espressif.com/components/espressif/dl_fft/versions/0.3.1/readme)
- AI inference framework: [esp-dl](https://github.com/espressif/esp-dl)
