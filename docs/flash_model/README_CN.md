# 模型加载方式[[English]](./README.md)

在 esp-sr 中，WakeNet 和 MultiNet 均会使用到大量的模型数据，模型数据位于 `ESP-SR_PATH/model/` 中。
目前 esp-sr 支持以下模型加载方式：

ESP32：

- 从 Flash 中直接加载

ESP32S3：  

- 从 Flash spiffs 分区加载
- 从外部 SDCard 加载

从而在 ESP32S3 上可以：

- 大大减小用户应用 APP BIN 的大小
- 方便用户进行 OTA
- 支持从 SD 卡读取和更换模型，更加便捷且可以缩减项目使用的模组 Flash 大小
- 当用户进行开发时，当修改不涉及模型时，可以避免每次烧录模型数据，大大缩减烧录时间，提高开发效率

## 1. 模型配置介绍

运行 `idf.py menuconfig` 进入 `ESP Speech Recognition`:

![overview](../img/model-1.png)

### 1.1 Net to use acceleration

该选项可以配置模型的加速方式，用户无须修改，请保持默认配置。

### 1.2 model data path

该选项只在 ESP32S3 上可用，表示模型数据的存储位置，支持选择 `spiffs partition` 或 `SD Card`。

- `spiffs partition` 表示模型数据存储在 Flash spiffs 分区中，模型数据将会从 Flash spiffs 分区中加载
- `SD Card` 表示模型数据存储在 SD 卡中，模型数据将会从 SD Card 中加载

### 1.3 use wakenet

此选项默认打开，当用户只使用 AEC 或者 BSS 等，无须运行 WakeNet 或 MultiNet 时，请关闭次选项，将会在一些情况下减小工程固件的大小。

- Wake word engine
 
 唤醒模型引擎选择。  

 ESP32 支持：
 
 - WakeNet 5 (quantized with 16-bit)
 
 ESP32S3 支持：
 
 - WakeNet 7 (quantized with 16-bit)
 - WakeNet 7 (quantized with 8-bit)
 - WakeNet 8 (quantized with 16-bit)

- Wake word name

 唤醒词选择，每个唤醒引擎支持的唤醒词有所不同，用户可以自行选择。
 
更多细节请参考 [WakeNet](../wake_word_engine/README.md) 。
 
### 1.4 use multinet

此选项默认打开。当用户只使用 WakeNet 或者其他算法模块时，请关闭此选项，将会在一些情况下减小工程固件的大小。

- langugae

 命令词识别语言选择，ESP32 只支持中文，ESP32S3 支持中文或英文。
 
- speech commands recognition model

 命令词识别模型选择。  
 ESP32 支持：
 
 - chinese single recognition (MultiNet2)
 
 ESP32S3 支持：
 
 - chinese single recognition (MultiNet3)
 - chinese continuous recognition (MultiNet3)
 - chinese single recognition (MultiNet4)

- Add speech commands

用户按照需求自定义添加命令词，具体请参考 [MultiNet](../speech_command_recognition/README.md) 。

## 2. 模型使用

当用户完成以上的配置选择后，应用层请参考 esp-skainet 进行初始化和使用。这里介绍一下模型数据加载在用户工程中的代码实现。

### 2.1 使用 ESP32

当用户使用 ESP32 时，由于只支持从 Flash 中直接加载模型数据，因此代码中模型数据会自动按照地址从 Flash 中读取所需数据。

### 2.2 使用 ESP32S3

#### 2.2.1 模型数据存储在 SPIFFS

当用户配置 #1.2 模型数据存储位置是 `spiffs partition` 时，用户需要：

- 编写分区表：

   ```
   model,  data, spiffs,         , SIZE,
   ```
   其中 SIZE 可以参考在用户使用 'idf.py build' 编译时的推荐大小，例如：
   
   ```
   Recommended model partition size: 500K
   ```
- 初始化 spiffs 分区
 
 **直接调用提供的 API**：用户可以直接调用 `srmodel_spiffs_init()` API 来初始化 spiffs。  
 
 **自行编写**：当用户需要在 spiffs 分区同时存放其他文件，如 web 网页时，可以自行编写 spiffs 初始化函数，需要注意 `esp_vfs_spiffs_conf`的配置：
 
 - base_path：模型的存储 `base_path` 为 `srmodel`，不可更改
 - partition_label：模型的分区 label 为 `model`，需要和 上述分区表中的 `Name` 保持一致
   
完成上述配置后，模型会在工程编译完成后自动生成 `model.bin`，并在用户烧写时候烧写到 spiffs 分区。  

**<font color=red>注：当用户更改模型后，再次编译前请务必进行 `idf.py clean`</font>**

#### 2.2.1 模型存储在 SD Card

当用户配置 #1.2 模型数据存储位置是 `SD Card` 时，用户需要：

- 手动移动模型数据

 将模型移动到 SDCard 中，用户完成以上配置后，可以先进行编译，编译完成后将 `ESP-SR_PATH/model/target/` 目录下的文件拷贝至 SD 卡的根目录。
 
- 自定义路径
 如果用户想将模型放置于指定文件夹，可以自己修改 `get_model_base_path()` 函数，位于 `ESP-SR_PATH/model/model_path.c`。
 比如，指定文件夹为 SD 卡目录中的 `espmodel`, 则可以修改该函数为：
 
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
 
- 初始化 SD 卡

 用户需要初始化 SD 卡，来使系统能够记载 SD 卡，如果用户使用 esp-skainet，可以直接调用 `sd_card_mount("/sdcard")` 来初始化其支持开发板的 SD 卡。否则，需要自己编写。

完成以上操作后，便可以进行工程的烧录。