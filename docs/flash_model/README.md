# Model loading method[[中文]](./README_CN.md)

In esp-sr, both WakeNet and MultiNet will use a large amount of model data, and the model data is located in `ESP-SR_PATH/model/`.
Currently esp-sr supports the following model loading methods:

ESP32:

- Load directly from Flash

ESP32S3:  

- Load from Flash spiffs partition
- Load from external SDCard

So that on ESP32S3 you can:

- Greatly reduce the size of the user application APP BIN
- Supports the selection of up to two wake words
- Support online switching of Chinese and English Speech Command Recognition
- Convenient for users to perform OTA
- Supports reading and changing models from SD card, which is more convenient and can reduce the size of module Flash used in the project
- When the user is developing the code, when the modification does not involve the model, it can avoid flashing the model data every time, greatly reducing the flashing time and improving the development efficiency

## 1. Model configuration introduction

Run `idf.py menuconfig` navigate to `ESP Speech Recognition`:

![overview](../img/model-1.png)

### 1.1 model data path

This option is only available on ESP32S3. It indicates the storage location of the model data. It supports the choice of `spiffs partition` or `SD Card`.

- `spiffs partition` means that the model data is stored in the Flash spiffs partition, and the model data will be loaded from the Flash spiffs partition
- `SD Card` means that the model data is stored in the SD card, and the model data will be loaded from the SD Card

### 1.2 use afe

This option needs to be turned on. Users do not need to modify it. Please keep the default configuration.

### 1.3 use wakenet

This option is turned on by default. When the user only uses `AEC` or `BSS`, etc., and does not need to run `WakeNet` or `MultiNet`, please turn off this option, which will reduce the size of the project firmware in some cases.

- First Wake word

 Select the first wake word. Please select the corresponding wake word in the options as needed.

 ESP32 支持：

  - WakeNet 5 (quantized with 16-bit)
 
 ESP32S3 支持：

 - WakeNet 7 (quantized with 16-bit)
 - WakeNet 7 (quantized with 8-bit)
 - WakeNet 8 (quantized with 16-bit)

- Second Wake wod

 For second wake word, please select the corresponding wake-up words in the options as needed.
 
 **Note: this option only supports ESP32S3, ESP32S3 supports the selection of up to two wake words and supports the user to switch in the code.**

For more details, please refer to [WakeNet](../wake_word_engine/README.md) .
 
### 1.4 use multinet

This option is turned on by default. When users only use WakeNet or other algorithm modules, please turn off this option, which will reduce the size of the project firmware in some cases.

ESP32 chip only supports Chinese Speech Commands Recognition. 

ESP32S3 supports Chinese and English Speech Commands Recognition, and supports Chinese and English recognition model switching.

- Chinese Speech Commands Model

 Chinese Speech Commands Recognition model selection.

 ESP32 supports:

  - None
  - chinese single recognition (MultiNet2)
 
 ESP32S3 支持：

  - None
  - chinese single recognition (MultiNet4.5)
  - chinese single recognition (MultiNet4.5 quantized with 8-bit)


- English Speech Commands Model

 English Speech Commands Recognition model selection.
 
 This option does not support ESP32.

 ESP32S3 Supports：

  - None
  - english recognition (MultiNet5 quantized with 8-bit, depends on WakeNet8)


- Add Chinese speech commands

 The user needs to add Chinese Speech Command words to this item when `Chinese Speech Commands Model` is not `None`.

- Add English speech commands

 The user needs to add English Speech Command words to this item when `Chinese Speech Commands Model` is not `None`.

For more details, please refer to [MultiNet](../speech_command_recognition/README.md) .

## 2. How to use

After the user completes the above configuration choices, please refer to esp-skainet to initialize and use the application layer. Here is an introduction to the code implementation of model data loading in the project.

### 2.1 Use ESP32

When the user uses ESP32, since it only supports loading the model data directly from the Flash, the model data in the code will automatically read the required data from the Flash according to the address.

### 2.2 Use ESP32S3

#### 2.2.1 Model data in SPIFFS

When the user configuration #1.2 model data storage location is `spiffs partition`, the user needs to:

- Write a partition table:

   ```
   model,  data, spiffs,         , SIZE,
   ```
   Among them, `SIZE` can refer to the recommended size when the user uses 'idf.py build' to compile, for example:
   
   ```
   Recommended model partition size: 500K
   ```

- Initialize the spiffs partition
 
 **Use the provided API**: User can use `srmodel_spiffs_init()` API to initialize spiffs.  
 
 **Write by yourself**: When users need to store other files in the spiffs partition at the same time, such as web pages, they can write the spiffs initialization function by themselves. Pay attention to the configuration of `esp_vfs_spiffs_conf`:
 
 - base_path: The model storage `base_path` is `srmodel` and cannot be changed
 - partition_label: The partition label of the model is `model`, which needs to be consistent with the `Name` in the above partition table
   
After completing the above configuration, the project will automatically generate `model.bin` after the project is compiled, and flash it to the spiffs partition. 

**<font color=red>Note: After the user changes the model, be sure to run `idf.py clean` before compiling again.</font>**

#### 2.2.1 Model data in SD Card

When the user configuration #1.2 model data storage location is `SD Card`, the user needs to:

- Manually move model data

 Move the model to SDCard. After the user completes the above configuration, you can compile first. After compiling, copy the files in the `ESP-SR_PATH/model/target/` directory to the root directory of the SD card.
 
- Custom model path

  If the user wants to place the model in the specified folder, he can modify the `get_model_base_path()` function by himself, located in `ESP-SR_PATH/model/model_path.c`.
  For example, if the specified folder is `espmodel` in the SD card directory, the function can be modified to:
 
    ```
     char *get_model_base_path(void)
    {
        #if defined CONFIG_MODEL_IN_SDCARD
            return "sdcard/espmodel";
        #elif defined CONFIG_MODEL_IN_SPIFFS
            return "srmodel";
        #else
            return NULL;
        #endif
    }
    ```
 
- Initialize the SD card

 The user needs to initialize the SD card to enable the system to record the SD card. The user can directly call `sd_card_mount("/sdcard")` to initialize the SD card that supports the development board if use esp-skainet. Otherwise, you need to write it yourself.