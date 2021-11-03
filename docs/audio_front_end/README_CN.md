# Audio Front-end 框架[[English]](./README.md)

乐鑫 Audio Front-end(AFE) 算法框架由乐鑫 AI 实验室自主开发。该框架基于 ESP32 系列芯片，能够向主机端提供高质量并且稳定的音频数据。

---

## 概述

乐鑫 AFE 框架以最便捷的方式基于乐鑫的 ESP32 系列芯片进行语音前端处理。使用乐鑫 AFE 框架，您可以获取高质量且稳定的音频数据，从而更加方便地构建唤醒或语音识别等应用。

乐鑫 AFE 的功能支持如下所示：

![overview](../img/AFE_overview.png)

乐鑫 AFE 的工作流程如下：

![overview](../img/AFE_workflow.png)

乐鑫 AFE 工作流程可以分为 4 块：

- AFE 的创建和初始化
- AFE feed，输入音频数据，feed 内部会先进行 AEC 算法处理
- 内部：AFE BSS/NS 算法处理
- AFE fetch，返回处理过的音频数据和返回值， fetch 内部会进行 VAD 处理，如果用户设置 WakeNet 为 enable 状态，也会进行唤醒词的检测

其中 `afe->feed()` 和 `afe->fetch()` 对用户可见，`Internal BSS Task` 对用户不可见。

> AEC 在 afe->feed() 函数中运行；  
> BSS 为 AFE 内部独立 Task 进行处理；  
> VAD 和 WakeNet 的结果通过 afe->fetch() 函数中获取。

### 选择 AFE handle

目前 AFE 支持单麦和双麦两种应用场景，单麦场景内部 Task 为 NS 处理，双麦场景内部 Task 为 BSS 处理。

- 单麦

		esp_afe_sr_iface_t *afe_handle = &esp_afe_sr_1mic;

- 双麦

		esp_afe_sr_iface_t *afe_handle = &esp_afe_sr_2mic;

### 选择 AFE mode

- 单麦

    乐鑫 AFE 单麦场景目前支持 2 种工作模式，分别为：SR_MODE_MONO_LOW_COST, SR_MODE_MONO_MEDIUM_COST.  
详细可见 afe_sr_mode_t 结构体。

  - SR_MODE_MONO_LOW_COST  
 
    适用于单通道音频数据+一路回采数据，具有很低的内存消耗和 CPU 资源消耗，此时运行低复杂度 AEC 和低复杂度降噪算法。

  - SR_MODE_MONO_MEDIUM_COST

    适用于单通道音频数据+一路回采数据，具有较低的内存消耗和 CPU 资源消耗，此时运行低复杂度 AEC 和中等复杂度降噪算法。

- 双麦

    乐鑫 AFE 双麦场景目前支持 3 种工作模式，分别为：SR_MODE_STEREO_LOW_COST, SR_MODE_STEREO_MEDIUM, SR_MODE_STEREO_HIGH_PERF.  
详细可见 afe_sr_mode_t 结构体。

  - SR_MODE_STEREO_LOW_COST

    适用于双通道音频数据 + 一路回采数据，AEC 采用复杂度较低的算法， BSS 采用低复杂度算法
 
  - SR_MODE_STEREO_MEDIUM

    适用于双通道音频数据 + 一路回采数据，AEC 采用复杂度较高的算法， BSS 采样低复杂度算法
 
  - SR_MODE_STEREO_HIGH_PERF

    适用于双通道音频数据 + 一路回采数据，AEC 和 BSS 均采用复杂度较高的模式

### 输入音频

- 当 AFE 单麦场景

  - 输入音频格式为 16KHz, 16bit, 双通道（1个通道为 mic 数据，另一个通道为参考回路）
  - 数据帧长为 16ms, 用户可以使用 `afe->get_feed_chunksize` 来获取需要的采样点数目（采样点数据类型为 int16）
     
   注意：此处得到数据量大小为单通道音频。
 
 数据排布如下：
 
 <img src="../img/AFE_mode_0.png" height = "100" align=center />  

- 当 AFE 双麦场景

 - 输入音频格式为 16KHz, 16bit, 三通道
 - 数据帧长为 32ms, 用户可以使用 `afe->get_feed_chunksize` 来获取需要填充的数据量

 数据排布如下：
 
  <img src="../img/AFE_mode_other.png" height = "70" align=center />  

### AEC 简介

AEC (Acoustic Echo Cancellation) 算法最多支持双通道处理，能够有效的去除 mic 输入信号中的自身播放回声。从而可以在自身播放音乐的情况下进行很好的语音识别等应用。

### NS 简介

NS (Noise Suppression) 算法支持单通道处理，能够对单通道音频中的非人声噪声进行抑制，尤其针对稳态噪声，具有很好的抑制效果。

### BSS 简介

BSS (Blind Source Separation) 算法支持双通道处理，能够很好的将目标声源和其余干扰音进行盲源分离，从而提取出有用音频信号，保证了后级语音的质量。

### VAD 简介

VAD (Voice Activity Detection) 算法支持实时输出当前帧的语音活动状态。

### WakeNet or Bypass 简介

用户可以选择是否在 AFE 中进行唤醒词的识别。当用户调用 `afe->disable_wakenet(afe_data)` 后，则进入 Bypass 模式，AFE 模块不会进行唤醒词的识别。

### 输出音频

AFE 的输出音频为单通道数据，在 WakeNet 开启的情况下，AFE 会输出有目标人声的单通道数据。

---

## 快速开始

### 1. 定义 afe_handle

`afe_handle` 是用户后续使用 afe 操作的相关句柄。用户需要根据单麦和双麦场景选择对应的 `afe_handle`。

单麦场景：

	esp_afe_sr_iface_t *afe_handle = &esp_afe_sr_1mic;

双麦场景：

	esp_afe_sr_iface_t *afe_handle = &esp_afe_sr_2mic;

### 2. 创建 afe_handle

用户使用 `afe_handle->create()` 函数来初始化在第一步中创建的 `afe_handle`。

```
typedef esp_afe_sr_data_t* (*esp_afe_sr_iface_op_create_t)(afe_sr_mode_t mode, int perferred_core);

- param mode              The mode of AFE_SR 
- param perferred_core    The perferred core to be pinned for BSS Task. 
- returns Handle to the AFE_SR data
```

调用 `afe_handle->create()` 时使用的两个形参如上。用户可以根据实际应用的需求来设置不同的 AFE 模式和 AFE 内部 BSS Task 运行的 CPU 核数。

注意：ESP32 系列的音频开发板，例如 ESP32-LyraT-Mini，AFE 模式只能选择 `SR_MODE_MONO_LOW_COST` 或者 `SR_MODE_MONO_MEDIUM_COST`， 即单通道模式。

### 3. 设置 WakeNet

对用户而言，设置 WakeNet 可以分为两步：  
- 使用 `make menuconfig` 来选择相应的唤醒模型，详见：[WakeNet](https://github.com/espressif/esp-sr/tree/b9504e35485b60524977a8df9ff448ca89cd9d62/wake_word_engine)

- 调用 `afe_handle->set_wakenet(afe_data, &WAKENET_MODEL, &WAKENET_COEFF);` 来初始化 WakeNet.

### 4. feed 音频数据

在初始化 AFE 和 WakeNet 完成后，用户需要将音频数据使用 `afe->feed()` 函数输入到 AFE 中进行处理。

输入的音频大小和排布格式可以参考 **输入音频** 这一步骤。

```
typedef int (*esp_afe_sr_iface_op_feed_t)(esp_afe_sr_data_t *afe, const int16_t* in);

- param afe   The AFE_SR object to queryq
- param in    The input microphone signal, only support signed 16-bit @ 16 KHZ. The frame size can be queried by the `get_samp_chunksize`. The channel number can be queried `get_channel_num`.
- return      The size of input

```

获取音频通道数：

使用 `afe->get_channel_num()` 函数可以获取需要传入 `afe->feed()` 函数的 mic 数据通道数。（不含参考回路通道）

```
typedef int (*esp_afe_sr_iface_op_get_channel_num_t)(esp_afe_sr_data_t *afe);
- param afe The AFE_SR object to query
- return The amount of samples to feed the fetch function
```

### 5. fetch 音频数据

用户调用 `afe->fetch()` 函数可以获取处理完成的单通道音频。  

fetch 的数据采样点数目（采样点数据类型为 int16）可以通过 `afe->get_fetch_chunksize` 获取。

```
typedef int (*esp_afe_sr_iface_op_get_samp_chunksize_t)(esp_afe_sr_data_t *afe);
- param afe The AFE_SR object to query
- param out   The output enhanced signal. The frame size can be queried by the `get_samp_chunksize`.
- return      The style of output, -1: noise, 0: speech, 1: wake word 1, 2: wake word 2, ...
```

用户需要注意 `afe->fetch()` 的返回值：
- -1: noise
- 0: speech
- 1: wake word 1
- 2: wake word 2
- ...

```
typedef int (*esp_afe_sr_iface_op_fetch_t)(esp_afe_sr_data_t *afe, int16_t* out);
- param afe   The AFE_SR object to query
- param out   The output enhanced signal. The frame size can be queried by the `get_samp_chunksize`.
- return      The style of output, -1: noise, 0: speech, 1: wake word 1, 2: wake word 2, ...
```

### 6. WakeNet 使用

用户使用 AFE 中 WakeNet 大体可以分为以下三种情况：

- 不使用 WakeNet

当用户不使用 WakeNet 时可以选择不初始化 WakeNet，即不需要调用：  

	afe_handle->set_wakenet(afe_data, &WAKENET_MODEL, &WAKENET_COEFF);

- 使用 WakeNet

用户使用 WakeNet 则需要先使用 `make menuconfig` 来配置相应的唤醒词信息。然后调用：  

	afe_handle->set_wakenet(afe_data, &WAKENET_MODEL, &WAKENET_COEFF);
    
则可以通过 `afe->fetch()` 函数来获取是否识别到唤醒词。

- 使用 WakeNet 但是在唤醒后暂时停止 WakeNet

当用户在唤醒后需要进行其他操作，比如离线或在线语音识别，这时候可以暂停 WakeNet 的运行，从而减轻 CPU 的资源消耗。  

用户可以调用 `afe->disable_wakenet(afe_data)` 来停止 WakeNet。 当后续应用结束后又可以调用 `afe->enable_wakenet(afe_data)` 来开启 WakeNet。

### 7. AEC 使用

AEC 的使用和 WakeNet 相似，用户可以根据自己的需求来停止或开启 AEC。

- 停止 AEC

	afe->disable_aec(afe_data);
    
- 开启 AEC

	afe->enable_aec(afe_data);
