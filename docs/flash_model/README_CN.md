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
- 支持选择最多两个唤醒词
- 支持中文和英文命令词识别在线切换
- 方便用户进行 OTA
- 支持从 SD 卡读取和更换模型，更加便捷且可以缩减项目使用的模组 Flash 大小
- 当用户进行开发时，当修改不涉及模型时，可以避免每次烧录模型数据，大大缩减烧录时间，提高开发效率

## 1. 模型配置介绍

运行 `idf.py menuconfig` 进入 `ESP Speech Recognition`:

![overview](../img/model-1.png)

### 1.1 model data path

该选项只在 ESP32S3 上可用，表示模型数据的存储位置，支持选择 `spiffs partition` 或 `SD Card`。

- `spiffs partition` 表示模型数据存储在 Flash spiffs 分区中，模型数据将会从 Flash spiffs 分区中加载
- `SD Card` 表示模型数据存储在 SD 卡中，模型数据将会从 SD Card 中加载

### 1.2 use afe

该选项需要打开，用户无须修改，请保持默认配置。

### 1.3 use wakenet

此选项默认打开，当用户只使用 AEC 或者 BSS 等，无须运行 WakeNet 或 MultiNet 时，请关闭次选项，将会减小工程固件的大小。
- 根据menuconfig列表选择唤醒词模型，`ESP Speech Recognition -> Select wake words`.　括号中为唤醒词模型的名字，你需要在代码用名字切换，初始化wakenet. 
![select wake wake](../img/wn_menu1.png)  
- 如果想加载多个唤醒词，以便在代码中进行唤醒词的切换，首选选择'Load Multiple Wake Words'  
![multi wake wake](../img/wn_menu2.png)  
然后按照列表选择多个唤醒词:  
![multi wake wake](../img/wn_menu3.png)
  
 **注：多唤醒词选项只支持 ESP32S3，具体根据客户硬件flash容量，选择合适数量的唤醒词。**

更多细节请参考 [WakeNet](../wake_word_engine/README.md) 。
 
### 1.4 use multinet

此选项默认打开。当用户只使用 WakeNet 或者其他算法模块时，请关闭此选项，将会在一些情况下减小工程固件的大小。

ESP32 芯片只支持中文命令词识别。ESP32S3 支持中文和英文命令词识别，且支持中英文识别模型切换。

- Chinese Speech Commands Model

 中文命令词识别模型选择。  
 
 ESP32 支持：
 
 - None
 - chinese single recognition (MultiNet2)
 
 ESP32S3 支持：
 
 - None
 - chinese single recognition (MultiNet4.5)
 - chinese single recognition (MultiNet4.5 quantized with 8-bit)


- English Speech Commands Model

 英文命令词识别模型选择。  
 
 该选项不支持 ESP32。
 
 ESP32S3 支持：
 
 - None
 - english recognition (MultiNet5 quantized with 8-bit, depends on WakeNet8)


- Add Chinese speech commands

 当用户在 `Chinese Speech Commands Model` 中选择非 `None` 时，需要在该项处添加中文命令词。
 

- Add English speech commands

 当用户在 `English Speech Commands Model` 中选择非 `None` 时，需要在该项处添加中文命令词。

用户按照需求自定义添加命令词，具体请参考 [MultiNet](../speech_command_recognition/README.md) 。

## 2. 模型使用

当用户完成以上的配置选择后，应用层请参考 esp-skainet 进行初始化和使用。这里介绍一下模型数据加载在用户工程中的代码实现。
也可以参考代码　[model_path.c](../../src/model_path.c)

### 2.1 使用 ESP32

当用户使用 ESP32 时，由于只支持从 Flash 中直接加载模型数据，因此代码中模型数据会自动按照地址从 Flash 中读取所需数据。
为了和ESP32S3进行兼容，代码中模型的初始化方法是和ESP32S3相同的，可参考下面ESP32S3的模型加载API．

### 2.2 使用 ESP32S3

#### 2.2.1 模型数据存储在 SPIFFS

- 编写分区表：

   ```
   model,  data, spiffs,         , SIZE,
   ```
   其中 SIZE 可以参考在用户使用 'idf.py build' 编译时的推荐大小，例如：
   
   ```
   Recommended model partition size: 500K
   ```
- 初始化 spiffs 分区
 **调用提供的 API**：用户可以直接调用 `esp_srmodel_init()` API 来初始化 spiffs，并返回spiffs中的模型。  
   - base_path：模型的存储 `base_path` 为 `srmodel`，不可更改
   - partition_label：模型的分区 label 为 `model`，需要和 上述分区表中的 `Name` 保持一致
   
完成上述配置后，模型会在工程编译完成后自动生成 `model.bin`，并在用户调用`idf.py flash`时烧写到 spiffs 分区。  


#### 2.2.２ 模型存储在 SD Card

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

 用户需要初始化 SD 卡，来使系统能够记载 SD 卡，如果用户使用 esp-skainet，可以直接调用 `esp_sdcard_init("/sdcard", num);` 来初始化其支持开发板的 SD 卡。否则，需要自己编写。

完成以上操作后，便可以进行工程的烧录。

#### 2.2.３ 代码中模型初始化与使用
``` 
    //
    // step1: initialize spiffs and return models in spiffs
    // 
    srmodel_list_t *models = esp_srmodel_init();

    //
    // step2: select the specific model by keywords
    //
    char *wn_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL); // select wakenet model
    char *nm_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL); // select multinet model
    char *alexa_wn_name  = esp_srmodel_filter(models, ESP_WN_PREFIX, "alexa"); // select wakenet with "alexa" wake word.
    char *en_mn_name  = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_ENGLISH); // select english multinet model
    char *cn_mn_name  = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_CHINESE); // select english multinet model
    
    // It also works if you use the model name directly in your code.
    char *my_wn_name = "wn9_hilexin"  
    // we recommend you to check that it is loaded correctly
    if (!esp_srmodel_exists(models, my_wn_name))
        printf("%s can not be loaded correctly\n")

    //
    // step3: initialize model
    //
    esp_wn_iface_t *wakenet = esp_wn_handle_from_name(wn_name);
    model_iface_data_t *wn_model_data = wakenet->create(wn_name, DET_MODE_2CH_90);

    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);
    model_iface_data_t *mn_model_data = multinet->create(mn_name, 6000);

```

