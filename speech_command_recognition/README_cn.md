# MultiNet 介绍 [[English]](./README.md)

MultiNet 是为了在 ESP32 上实现多命令词识别, 基于 [CRNN](https://arxiv.org/pdf/1703.05390.pdf) 网络和 [CTC](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.75.6306&rep=rep1&type=pdf) 设计的轻量化模型，目前支持 100 个以内的自定义命令词识别。

## 概述

MultiNet 输入为音频经过 **MFCC** 处理后的特征值，输出为汉语/英语的“音素”分类。通过对输出音素进行组合，则可以对应到相应的汉字或单词。  

## 命令词识别流程

1. 添自定义命令词
2. 输入一帧时间长度为 30ms 的音频（16KHz, 16bit, 单声道）
3. 获得输入音频的 **MFCC** 特征值
4. 将特征值输入 MultiNet，输出该帧对应的识别**音素**
5. 将识别出的音素送至语言模型输出最终识别结果
6. 将识别结果和已存储的命令词队列比对，输出对应的命令词 ID

其中 3-6 步均在接口内完成，无须用户自己处理。

可以参考以下命令词识别流程：

![speech_command-recognition-system](../img/multinet_workflow.png)


## 使用指南

### 命令词

目前，用户可以使用 `make menuconfig` 命令来添加自定义命令词。可以通过 `menuconfig->Component config > ESP Speech Recognition->Add speech commands` 添加命令词，目前已经添加有 20 个命令词，如下表所示：

|Command ID|命令词|Command ID|命令词|Command ID|命令词|Command ID|命令词|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|0|打开空调|5|降低一度|10| 关闭节能模式|15| 播放歌曲
|1|关闭空调|6|制热模式|11| 除湿模式|16| 暂停播放
|2|增大风速|7|制冷模式|12| 关闭除湿模式|17| 定时一小时
|3|减少风速|8|送风模式|13| 打开蓝牙|18| 打开电灯
|4| 升高一度|9|节能模式|10| 关闭蓝牙|19| 关闭电灯

网络支持自定义命令词，用户可以将自己想要的设置的命令词加入 MultiNet，注意新添加的命令词需要有其的对应 Command ID 已便于 MultiNet 时候后输出。

### 添加自定义命令词

可以通过 `make menuconfig > Component config > ESP Speech Recognition->Add speech commands` 命令使用拼音添加对应的命令词，比如：

当添加“打开空调”命令时，应该输入 "da kai kong tiao"，请注意：

- 一个 Commnad ID 可以对应多个命令短语
- 最多支持 100 个 Command ID 或者 命令短语
- 同一个 Command ID 对应的几条命令短语之间应该由 "," 隔开

### 基础配置
在使用命令词识别模型前首先需要定义以下变量：  

1. 模型版本

 模型版本可以需要在 `menuconfig` 中进行预选择，请在选择后在代码里添加如下的代码  
		static const esp_mn_iface_t *multinet = &MULTINET_MODEL;  
        
2. 生成模型句柄  

 支持的语言和模型的有效性由模型参数决定，现在只支持中文命令。请在 `menuconfig` 中配置 `MULTINET_COEFF` 选项，并在代码中添加以下行以生成模型句柄。 6000 是语音识别的音频长度，以 ms 为单位，sample_length 的范围为 0~6000。  
		model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF, 6000);

### API 参考

#### 头文件
- esp_mn_iface.h
- esp_mn_models.h

#### 函数

- `typedef model_iface_data_t* (*esp_mn_iface_op_create_t)(const model_coeff_getter_t *coeff, int sample_length);`  

  **Definition**  
   
 	Easy function type to initialize a model instance with a coefficient.
    
  **Parameter**  
   
 	* coeff: The coefficient for speech commands recognition.  
 	* sample_length: Audio length for speech recognition, in ms. The range of sample_length is 0~6000.
    
  **Return**  
 	  
 	Handle to the model data.

- `typedef int (*esp_mn_iface_op_get_samp_chunksize_t)(model_iface_data_t *model);`

   **Definition**  
   
	 Callback function type to fetch the amount of samples that need to be passed to the detection function. Every speech recognition model processes a certain number of samples at the same time. This function can be used to query the amount. Note that the returned amount is in 16-bit samples, not in bytes.
       
  **Parameter**  
   
 	model: The model object to query.
  
  **Return**
  
    The amount of samples to feed the detection function.


- `typedef int (*esp_mn_iface_op_get_samp_chunknum_t)(model_iface_data_t *model);`

   **Definition**  
   
	 Callback function type to fetch the number of frames recognized by the speech command.
       
  **Parameter**  
   
 	model: The model object to query.
  
  **Return**
  
    The number of the frames recognized by the speech command.
    
- `typedef int (*esp_mn_iface_op_get_samp_rate_t)(model_iface_data_t *model);`

   **Definition**  
   
 	Get the sample rate of the samples to feed to the detection function.

  **Parameter**  
  
 	model: The model object to query.
 
  **Return**  
  
 	The sample rate, in Hz.

- `typedef float* (*esp_mn_iface_op_detect_t)(model_iface_data_t *model, int16_t *samples);`  

   **Definition**
 
    Easy function type to initialize a model instance with a coefficient.
    
  **Parameter**  

    coeff: The coefficient for speech commands recognition.  
    
  **Return**  
   
 	* The command id, if a matching command is found.
 	* -1, if no matching command is found.
 
- `typedef void (*esp_mn_iface_op_destroy_t)(model_iface_data_t *model);`  

   **Definition**  
  
   Destroy a voiceprint recognition model.
 
  **Parameters**  
  
  model: Model object to destroy.
