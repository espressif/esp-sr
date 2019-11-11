# esp_sr [[中文]](./README_cn.md)

Espressif esp_sr provides basic algorithms for **Speech Recognition** applications. Now, this framework has three modules:

* The wake word detection model [WakeNet](wake_word_engine/README.md)
* The speech command recognition model [MultiNet](speech_command_recognition/README.md) 
* Acoustic algorithm: AEC(Acoustic Echo Cancellation), VAD(Voice Activity Detection), AGC(Automatic Gain Control), NS(Noise Suppression)

These algorithms are provided in the form of a component, so they can be integrated into your projects with minimum efforts.

## Wake Word Engine

Espressif wake word engine [WakeNet](wake_word_engine/README.md) is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always listen wake words, such as “Alexa”, “天猫精灵” (Tian Mao Jing Ling) and “小爱同学” (Xiao Ai Tong Xue).  

Currently, Espressif has not only provided an official wake word "Hi, Lexin" to public for free, but also allows customized wake words. For details on how to customize your own wake words, please see [Espressif Speech Wake Words Customization Process](wake_word_engine/ESP_Wake_Words_Customization.md).

## Speech Command Recognition

Espressif's speech command recognition model [MultiNet](speech_command_recognition/README.md) is specially designed to provide a flexible off-line speech command recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again.

Currently, Espressif **MultiNet** supports up to 100 Chinese or English speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light).
