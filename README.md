# esp_sr

Espressif esp_sr provides basic algorithms for **Speech Interaction** applications. Now, this framework has two models:

* The wake word detection model [WakeNet](wake_words_engine/README.md)
* The speech commands recognition model [MultiNet](speech_commands_recognition/README.md) 

These algorithms are provided in the form of a component, so they can be integrated into your projects with minimum efforts.

## Wake Word Engine

Espressif wake word engine [WakeNet](wake_words_engine/README.md) is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always listen wake words, such as “Alexa”, “天猫精灵” (Tian Mao Jing Ling) and “小爱同学” (Xiao Ai Tong Xue).  

Currently, Espressif has not only provided an official wake word "Hi, Lexin" to public for free, but also allows customized wake words. For details on how to customize your own wake words, please see [Espressif Speech Wake Words Customization Process](wake_words_engine/ESP_Wake_Words_Customization.md).

## Speech Commands Recognition

Espressif's speech commands recognition model [MultiNet](speech_commands_recognition/README.md) is specially designed to provide a flexible off-line speech commands recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again.

Currently, Espressif **MultiNet** supports up to 100 Chinese speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light). 

We will add supports for English commands in the next release.
