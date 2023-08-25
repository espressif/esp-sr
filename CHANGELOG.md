# Change log for esp-sr

## Known issues: 
- Available storage is less than the remaining flash space on IDF v5.0.   
If you can not map model partition successfully, please check the left free storage by `spi_flash_mmap_get_free_pages(ESP_PARTITION_MMAP_DATA)` or update IDF to v5.1 or later.

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
