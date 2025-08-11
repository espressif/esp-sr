# Change log for esp-sr

## 2.1.5
- Fix ringbuf bug
- Add more wakenet9 models

## 2.1.4
- Fix the bug of multinet duration overflow
- Fix the memory leak bug in multinet7 and multinet6

## 2.1.3
- Replace kissfft with dl_fft for wakenet9s
- Replace esp-dsp fft with dl_fft for wakenet9
- Add more wakenet9 model

## 2.1.2
- Add debug mode
- Update wakenet trigget to v4

## 2.1.1
- Add 8KHz AEC for VoIP
- Add more wakenet9 models

## 2.1.0
- esp32c3 support wakenet9s and aec
- esp32c5 support wakenet9s and aec
- esp32c6 support wakenet9s and aec
- esp32s2 support wakenet9s and aec
- Add four wakenet9s model: hilexin,hiesp,nihaoxiaozhi,hijason

## 2.0.5
- Fix fftr bug

## 2.0.4
- Add DOA(Direction of Arrival) algorithm

## 2.0.3
- Add wakenet9 support for ESP32

## 2.0.2
- Accelerate the AEC of ESP32C5, reducing the CPU loading from 39% to 30%.

## 2.0.1
- Add `afe_aec_create`, `afe_aec_destroy`, `afe_aec_process` interface

## 2.0.0
- Add vadnet1_medium model
- Refactor AFE interface. Note AFE v2.0 is not compatible with previous versions
- Add esp32c5 AEC support
- Add some new wake words

## 1.9.5
- Add Hi,Jason; 小鸭小鸭; 璃奈板 wake word models

## 1.9.4
- Fix bugs when using C++ to compile
- Add more new wake words

## 1.9.3
- Fix nsnet2 crash
- Add esp32p4 ci test

## 1.9.2
- Improve nsnet2 performance

## 1.9.1
- Support esp32p4 for nsnet2
- Add a method to load model from rodata

## 1.9.0
- Support esp32p4 for WakeNet, MultiNet and AFE_SR


## 1.8.0
- Support esp-idf v5.3
- Add more new wake words
- Add setting "fixed_first_channel" in afe_config
 
## 1.7.1
- Add 喵喵同学，Hi,joy, (Hi,Lily/Hi,莉莉) wakenet model

## 1.7.0
- Add first Noise Suppression model: nsnet2
- Add more wake word model trained by TTS sample

## 1.6.1
- Add hey willow wakenet model: wn9_heywillow_tts
- Update wn9_alexa from v3 to v4
- Add model info into srmodels_list_t
- parse wake words from model_info string

## 1.6.0
- Add Chinese MultiNet7 models
- Add first Noise Suppression model: nsnet1
- Add wakenet model trained by TTS samples: Jarvis and Computer
- fix some bugs

## 1.5.1
- Reduce Internal RAM of multinet7
- Update benchmark
- Add ci build test for esp32
- Fix some bugs

## 1.5.0
- Add esp32c6 tts lib
- Return the volume of wake word audio when one wake word is detected
- Reduce MultiNet6 SRAM size from 48KB to 32 KB
- Add "Hi M Five" wake word model from M5Stack
- Remove all MultiNet4 models
- Update MultiNet7_en v2.0
- Fix some bugs in AFE
- Add Flite G2P module

## 1.4.2
- Reset timeout trigger of multinet6 when a new speech command is detected
- Allocate all beams from PSRAM

## 1.4.1
- Fix the wrong APIs of multinet2 when using ESP32 chip
- VAD can work fine when WakeNet is disabled

## 1.4.0
- Add ci tests to check the APIs of wakenet, multinet and AFE work fine
- Support to load and run two wakenet9 models at the same time in AFE
- Reduce the latency of multinet6

## 1.3.4
- Fix the bug of multinet5q8 whrn adding new speech commands

## 1.3.3
- Fix the crash of multinet5q8 when speech commands are triggered
- Update esp_mn_commands_update() in esp_process_sdkconfig.c

## 1.3.2
- Output text of ctc greedy search result when no command can be detected.
- Modify the default Chinese commands
- Remove __pycache__ in esp-sr/model folder

## 1.3.1
- Bugfix: remove all cxx11:string
- Bugfix: remove esp-partition for esp32s2 & esp32c3 on idf v4.4
- Update multinet API to add/modify/check new commands in the code
- Update documents to introduce how to use multinet API

## 1.3.0 
- Update the partition APIs to keep compatible with both IDF v4.4 and IDF v5.0
- Add a new Chinese MultiNet6 mdoel for air conditioner controller 

## 1.2.1
- Fix bugs in model loader
- Read all parameters sequentially, which reduces about 5x in model loading time.
- Use esp_partition_mmap to replace spiffs file system, which further reduces about 3x in model loading time
- Add WakeNet API unity test
- Add MultiNet API unity test

## 1.2.0
- ESP-DSP dependency is now installed from the component registry
- Add an English MultiNet6 model which is trained by RNNT and CTC
- Add a Chinese MultiNet6 model which is trained by RNNT and CTC
- Fixed CMake errors when esp-sr was installed from component registry
- Fixed the list of supported chips displayed in the component registry

## 1.1.0
- Support esp32c3 for Chinese TTS
- Update document of ESP-SR
- Add ESP-SR into Espressif component manager

## 1.0.0
- Add wakenet8 & wakenet9
- Add multinet5 to support English and Chinese speech command recognition
- Remove wakenet7
- Add AFE pipeline for voice communication

## 0.8.0
- support ESP32S3 chip
- add wakenet7 & update wakenet5 to support multi-channel detection
- remove wakenet6
- add AFE pipeline for speech recognition

## 0.7.0
- add chinese tts
- update noise suppression v2
- update AEC v3

## 0.6.0
- update multinet_cn_1.4 and add CONTINUOUS RECOGNITION mode
- improve hilexin wakeNet5X3 model(v5)
- support IDFv4.0 build system
- replace MAP algorithm with MASE(Mic Array Speech Enhancement) algorithm v1.0

## 0.5.0
- add multinet1 English model v1.0
- update multinet1 Chinese model v2.0
- add Mic Array Processing(MAP) algorithm
- Fix the bug of parsing speech command
- fix the bug of decoder

## 0.3.0
- add wakenet6
- support cmake
- add free wake word: hi jeson
- update wakenet5X3 wake word model(v2)

## 0.2.0
- add acoustic algorithm, include AEC, AGC, VAD ,NS
- add wakenet5X2 and wakenet5X3

## 0.1.0
- Initial commit, include wakenet4,wakenet5 and multinet1_cni
