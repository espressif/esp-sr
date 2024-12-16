# ESP-SR Speech Recognition Framework

[![Documentation Status](./docs/_static/sr_doc_latest.svg)](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/index.html)
[![Component Registry](https://components.espressif.com/components/espressif/esp-sr/badge.svg)](https://components.espressif.com/components/espressif/esp-sr)

Espressif [ESP-SR](https://github.com/espressif/esp-sr) helps users build AI speech solutions based on ESP32-S3 or ESP32-P4 chips.

Overview
--------

ESP-SR framework includes the following modules:

* [Audio Front-end AFE](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/README.html)
* [Wake Word Engine WakeNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/wake_word_engine/README.html)
* [Speech Command Word Recognition MultiNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/speech_command_recognition/README.html)
* [Speech Synthesis](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/speech_synthesis/readme.html)

These algorithms are provided in the form of a component, so they can be integrated into your projects with minimum effort.

ESP32-S3/ESP32-P4 are recommended, which support AI instructions and larger, high-speed octal SPI PSRAM.
The new algorithms will no longer support ESP32 chips.


## Wake Word Engine

Espressif wake word engine **WakeNet** is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always listen to wake words, such as “Alexa”, “Hi,lexin” and “Hi,ESP”.

Espressif offers two ways to customize the wake word, please refer to the following document to choose the one that meets your needs:   
[Espressif Speech Wake Words Customization Process](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/wake_word_engine/ESP_Wake_Words_Customization.html) or [Training Wake Words by TTS sample](https://github.com/espressif/esp-sr/issues/88).

The following wake words are supported in esp-sr:

|wake words       |                 ESP32                 |  ESP32-S3/ESP32-P4     | 
|:--------------- | :------------------------------------:| :---------------------:| 
|Hi,乐鑫           |  wn5_hilexin, wn5_hilexinX3           | wn9_hilexin            | 
|你好小智          |  wn5_nihaoxiaozhi,wn5_nihaoxiaozhiX3   | wn9_nihaoxiaozhi_tts   |
|小爱同学          |                                        | wn9_xiaoaitongxue      | 
|Hi,ESP           |                                        | wn9_hiesp              | 
|Hi,M Five        |                                        | wn9_himfive            | 
|Alexa            |                                        | wn9_alexa              | 
|Jarvis           |                                        | wn9_jarvis_tts         | 
|Computer         |                                        | wn9_computer_tts       |
|Hey,Willow       |                                        | wn9_heywillow_tts      | 
|Sophia           |                                        | wn9_sophia_tts         |
|Mycroft          |                                        | wn9_mycroft_tts        |
|Hey,Printer      |                                        | wn9_heyprinter_tts     |
|Hi,Joy           |                                        | wn9_hijoy_tts          |
|Hey,Wand         |                                        | wn9_heywanda_tts       |
|Astrolabe        |                                        | wn9_astrolabe_tts      |
|Hi,Jason         |                                        | wn9_hijason_tts2       |
|你好小鑫         |                                        | wn9_nihaoxiaoxin_tts   |
|小美同学         |                                        | wn9_xiaomeitongxue_tts |
|Hi,小星          |                                        | wn9_hixiaoxing_tts     |
|小龙小龙         |                                        | wn9_xiaolongxiaolong_tts    |
|喵喵同学         |                                        | wn9_miaomiaotongxue_tts|
|Hi,喵喵          |                                        | wn9_himiaomiao_tts     |
|Hi,Lily/Hi,莉莉  |                                        | wn9_hilili_tts         |
|Hi,Telly/Hi,泰力 |                                        | wn9_hitelly_tts        |
|小滨小滨/小冰小冰|                                        | wn9_xiaobinxiaobin_tts |
|Hi,小巫          |                                        | wn9_haixiaowu_tts      |
|小鸭小鸭         |                                        | wn9_xiaoyaxiaoya_tts2  |
|璃奈板           |                                        | wn9_linaiban_tts2      |

*NOTE:* `_tts` suffix means this WakeNet model is trained by TTS samples. `_tts2` suffix means this WakeNet model is trained by TTS Pipeline V2.

## Speech Command Recognition

Espressif's speech command recognition model **MultiNet** is specially designed to provide a flexible off-line speech command recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again. 

Currently, Espressif **MultiNet** supports up to 300 Chinese or English speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light).

The following MultiNet models are supported in esp-sr:  

|language         |           ESP32            |         ESP32-S3              |          ESP32-P4             | 
|:--------------- | :-------------------------:| :----------------------------:| :----------------------------:|
|Chinese          | mn2_cn                     | mn5q8_cn, mn6_cn, mn7_cn      |  mn7_cn                       |
|English          |                            | mn5q8_en, mn6_en, mn7_en      |  mn7_en                       |

## Audio Front End

Espressif Audio Front-End **AFE** integrates AEC (Acoustic Echo Cancellation), VAD (Voice Activity Detection), BSS (Blind Source Separation) and NS (Noise Suppression).

Our two-mic Audio Front-End (AFE) have been qualified as a “Software Audio Front-End Solution” for [Amazon Alexa Built-in devices](https://developer.amazon.com/en-US/alexa/solution-providers/dev-kits#software-audio-front-end-dev-kits).


**In order to achieve optimal performance:**

* Please refer to software design [esp-skainet](https://github.com/espressif/esp-skainet).
