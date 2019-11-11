# esp_sr [[English]](./README.md)

esp_sr 提供语音识别相关方向算法模型，目前主要包括三个模块：

* 唤醒词识别模型 [WakeNet](wake_word_engine/README_cn.md)
* 语音命令识别模型 [MultiNet](speech_command_recognition/README_cn.md) 
* 声学算法：AEC(Acoustic Echo Cancellation), VAD(Voice Activity Detection), AGC(Automatic Gain Control), NS(Noise Suppression)

这些算法以组件的形式提供，因此可以轻松地将它们集成到您的项目中。

## 唤醒词识别

唤醒词模型 [WakeNet](wake_word_engine/README_cn.md)，致力于提供一个低资源消耗的的高性能模型，支持类似“Alexa”，“天猫精灵”，“小爱同学”等唤醒词的识别。  

目前乐鑫免费开放“Hi，乐鑫”唤醒词。如果用户需要其它唤醒词，乐鑫提供有唤醒词定制服务，具体可参考 [乐鑫语音唤醒词定制流程](wake_word_engine/乐鑫语音唤醒词定制流程.md)。

## 语音命令识别

命令词识别模型 [MultiNet](speech_command_recognition/README_cn.md) ，致力于提供一个灵活的离线语音命词识别框架。用户可方便根据需求自定义语音命令，无需重新训练模型。  

目前模型支持类似“打开空调”，“打开卧室灯”等中文命令词识别和"Turn on/off the light" 等英文命令词识别，自定义语音命令词最大个数为 100。

