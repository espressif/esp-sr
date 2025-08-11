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
[21/4/2025]: We add a new model WakeNet9s, which can run on chips that do not have PSRAM and do not support SIMD, such as ESP32C3 and ESP32C5. [examples](https://github.com/espressif/esp-skainet/tree/master/examples/wake_word_detection)  
[17/4/2025]: We add a new DOA(Direction of Arrival) algorithm.  
[14/2/2025]: We release **ESP-SR V2.0**. [Migration from ESP-SR V1.* to ESP-SR V2.*](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/migration_guide.html)   
[13/2/2025]: We release **VADNet**, a voice activaty detection model. You can use it to replace the WebRTC VAD and improve the performance.

## Wake Word Engine

| Supported Targets | ESP32    | ESP32-S2 | ESP32-S3 | ESP32-P4 | ESP32-C3 | ESP32-C5 | ESP32-C6 | 
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

Espressif wake word engine **WakeNet** is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always listen to wake words, such as “Alexa”, “Hi,lexin” and “Hi,ESP”. WakeNet9 and WakeNet9s models are supported. WakeNet9s is a cost-down version of WakeNet9, with fewer parameters and lower computational requirements. 

Espressif offers two ways to customize the wake word, please refer to the following document to choose the one that meets your needs:   
[Espressif Speech Wake Words Customization Process](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/wake_word_engine/ESP_Wake_Words_Customization.html) or [Training Wake Words by TTS sample](https://github.com/espressif/esp-sr/issues/88).

The following wake words are supported in esp-sr:

|wake words       |             WakeNet9s           |  WakeNet9              | 
|:--------------- | :------------------------------:| :---------------------:| 
|Hi,乐鑫           |  wn9s_hilexin                   | wn9_hilexin            | 
|Hi,ESP           |  wn9s_hiesp                      | wn9_hiesp              | 
|你好小智          |  wn9s_nihaoxiaozhi              | wn9_nihaoxiaozhi_tts   |
|Hi,Jason         |   wn9s_hijason_tts2              | wn9_hijason_tts2       |
|你好喵伴          |                                 | wn9_nihaomiaoban_tts2   |
|小爱同学          |                                 | wn9_xiaoaitongxue      | 
|Hi,M Five        |                                  | wn9_himfive            | 
|Alexa            |                                  | wn9_alexa              | 
|Jarvis           |                                  | wn9_jarvis_tts         | 
|Computer         |                                  | wn9_computer_tts       |
|Hey,Willow       |                                  | wn9_heywillow_tts      | 
|Sophia           |                                  | wn9_sophia_tts         |
|Mycroft          |                                  | wn9_mycroft_tts        |
|Hey,Printer      |                                  | wn9_heyprinter_tts     |
|Hi,Joy           |                                  | wn9_hijoy_tts          |
|Hey,Wand         |                                  | wn9_heywanda_tts       |
|Astrolabe        |                                  | wn9_astrolabe_tts      |
|Hey,Ily          |                                  | wn9_heyily_tts2        |
|Hi,Jolly         |                                  | wn9_hijolly_tts2        |
|Hi,Fairy         |                                  | wn9_hifairy_tts2        |
|Blue Chip        |                                  | wn9_bluechip_tts2        |
|Hi,Wall E/Hi,瓦力|                                  | wn9_hiwalle_tts2       |
|你好小鑫         |                                  | wn9_nihaoxiaoxin_tts   |
|小美同学         |                                  | wn9_xiaomeitongxue_tts |
|Hi,小星          |                                  | wn9_hixiaoxing_tts     |
|小龙小龙         |                                  | wn9_xiaolongxiaolong_tts    |
|喵喵同学         |                                  | wn9_miaomiaotongxue_tts|
|Hi,喵喵          |                                  | wn9_himiaomiao_tts     |
|Hi,Lily/Hi,莉莉  |                                  | wn9_hilili_tts         |
|Hi,Telly/Hi,泰力 |                                  | wn9_hitelly_tts        |
|小滨小滨/小冰小冰|                                  | wn9_xiaobinxiaobin_tts |
|Hi,小巫          |                                  | wn9_haixiaowu_tts      |
|小鸭小鸭         |                                  | wn9_xiaoyaxiaoya_tts2  |
|璃奈板           |                                  | wn9_linaiban_tts2      |
|小酥肉           |                                  | wn9_xiaosurou_tts2      |
|小宇同学         |                                  | wn9_xiaoyutongxue_tts2  |
|小明同学         |                                  | wn9_xiaomingtongxue_tts2|
|小康同学         |                                  | wn9_xiaokangtongxue_tts2|
|小箭小箭         |                                  | wn9_xiaojianxiaojian_tts2|
|小特小特         |                                  | wn9_xiaotexiaote_tts2|
|你好小益         |                                  | wn9_nihaoxiaoyi_tts2|
|你好百应         |                                  | wn9_nihaobaiying_tts2|
|小鹿小鹿         |                                  | wn9_xiaoluxiaolu_tts2|
|你好东东         |                                  | wn9_nihaodongdong_tts2|
|你好小安         |                                  | wn9_nihaoxiaoan_tts2|

*NOTE:* `_tts` suffix means this WakeNet model is trained by TTS samples. `_tts2` suffix means this WakeNet model is trained by TTS Pipeline V2.

## Speech Command Recognition

| Supported Targets | ESP32    | ESP32-S3 | ESP32-P4 | 
| ----------------- | -------- | -------- | -------- |

Espressif's speech command recognition model **MultiNet** is specially designed to provide a flexible off-line speech command recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again. 

Currently, Espressif **MultiNet** supports up to 300 Chinese or English speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light).

The following MultiNet models are supported in esp-sr:  

|language         |           ESP32            |         ESP32-S3              |          ESP32-P4             | 
|:--------------- | :-------------------------:| :----------------------------:| :----------------------------:|
|Chinese          | mn2_cn                     | mn5q8_cn, mn6_cn, mn7_cn      |  mn7_cn                       |
|English          |                            | mn5q8_en, mn6_en, mn7_en      |  mn7_en                       |

## Audio Front End

| Supported Targets | ESP32    | ESP32-S3 | ESP32-P4 | 
| ----------------- | -------- | -------- | -------- |

Espressif Audio Front-End **AFE** integrates AEC (Acoustic Echo Cancellation), VAD (Voice Activity Detection), BSS (Blind Source Separation) and NS (Noise Suppression), NSNET(Deep noise suppression) and other functions. It is designed to be used with the ESP-SR library.

Our two-mic Audio Front-End (AFE) have been qualified as a “Software Audio Front-End Solution” for [Amazon Alexa Built-in devices](https://developer.amazon.com/en-US/alexa/solution-providers/alexa-connect-kit).


## Documentation and Resources

ESP-SR Documentation: [ESP-SR Documentation](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/index.html)   
Migration Guide: [Migration from V1.* to V2.*](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/migration_guide.html)  
Wake Word Training: [Wake Word Training by TTS Pipeline V2.0](https://github.com/espressif/esp-sr/issues/88)  
Examples: [esp-skainet/examples](https://github.com/espressif/esp-skainet)  

