# ESP-SR Speech Recognition Framework

[![Documentation Status](./docs/_static/sr_doc_latest.svg)](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/index.html)
[![Component Registry](https://components.espressif.com/components/espressif/esp-sr/badge.svg)](https://components.espressif.com/components/espressif/esp-sr)

Espressif [ESP-SR](https://github.com/espressif/esp-sr) helps users build AI speech solutions based on ESP32 or ESP32-S3 chips.

Overview
--------

ESP-SR framework includes the following modules:

* [Audio Front-end AFE](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/audio_front_end/README.html)
* [Wake Word Engine WakeNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/wake_word_engine/README.html)
* [Speech Command Word Recognition MultiNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32s3/speech_command_recognition/README.html)
* Speech Synthesis (only supports Chinese language)

These algorithms are provided in the form of a component, so they can be integrated into your projects with minimum effort.

ESP32-S3 is recommended, which supports AI instructions and larger, high-speed octal SPI PSRAM.
The new algorithms will no longer support ESP32 chips.


## Wake Word Engine

Espressif wake word engine **WakeNet** is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always listen to wake words, such as “Alexa”, “Hi,lexin” and “Hi,ESP”. You can refer to **Model loading method** to build your project.

Currently, Espressif has not only provided an official wake word "Hi,Lexin","Hi,ESP" to the public for free, but also allows customized wake words. For details on how to customize your own wake words, please see **Espressif Speech Wake Words Customization Process**.

## Speech Command Recognition

Espressif's speech command recognition model **MultiNet** is specially designed to provide a flexible off-line speech command recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again. You can refer to **Model loading method** to build your project.

Currently, Espressif **MultiNet** supports up to 200 Chinese or English speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light).

## Audio Front End

Espressif Audio Front-End **AFE** integrates AEC (Acoustic Echo Cancellation), VAD (Voice Activity Detection), BSS (Blind Source Separation) and NS (Noise Suppression).

Our two-mic Audio Front-End (AFE) have been qualified as a “Software Audio Front-End Solution” for [Amazon Alexa Built-in devices](https://developer.amazon.com/en-US/alexa/solution-providers/dev-kits#software-audio-front-end-dev-kits).


**In order to achieve optimal performance:**

* Please refer to software design [esp-skainet](https://github.com/espressif/esp-skainet).
