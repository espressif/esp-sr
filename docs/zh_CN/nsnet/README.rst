噪声抑制模型 (NSNet)
====================

:link_to_translation:`en:[English]`

NSNet 是一个基于深度学习的噪声抑制模型，面向低功耗嵌入式 MCU。

概述
----

NSNet 目前提供两个模型：**追求极致的人工智能语音降噪效果建议使用 nsnet2，降噪与语音失真达到平衡建议使用 nsnet3。**

``nsnet2`` 是一个量化后的神经网络降噪模型，具有以下特点：

- 采样率：16 kHz，16-bit PCM
- 帧长：1024 个采样点（64 ms），帧移：512 个采样点（32 ms）
- 基于 ERB 的频谱掩码：网络估计时频掩码并应用于带噪频谱
- 支持单通道处理，并支持多通道共享掩码模式（见下文 **多通道共享掩码处理**）
- 支持芯片：ESP32-S3、ESP32-S31 和 ESP32-P4

``nsnet3`` 是一个 float32 的降噪模型，具有以下特点：

- 采样率：16 kHz，16-bit PCM
- 帧长：512 个采样点（32 ms），帧移：256 个采样点（16 ms），端到端延迟更低
- 基于 ERB 的复数比掩码（CRM）：float32 推理，无量化损失，网络仅 48.2K 参数（权重约 100 KB）
- 支持单通道处理，并支持多通道共享掩码模式（与 nsnet2 相同的 ``create_mc()`` / ``process_mc()`` 语义）
- 支持芯片：ESP32-P4 和 ESP32-S31（均已实测；接口同样面向 ESP32-S3 目标编译）
- 单核 CPU 占用约 20%（ESP32-P4 @ 400 MHz 实测）

.. note::

   通过 ``idf.py menuconfig`` -> ``ESP Speech Recognition`` -> ``Select noise suppression model`` -> ``Deep noise suppression v2 (nsnet2)`` 或 ``Deep noise suppression v3 (nsnet3)`` 选择模型。

使用 NSNet
----------

接口定义在 ``esp_nsn_iface.h`` 中。所有操作都通过 ``esp_nsn_iface_t`` 函数表完成，函数表由模型名获取。nsnet2 与 nsnet3 实现**完全相同**的接口，应用层无需区分模型。

**基本流程（单通道）：**

1. **获取模型接口并创建实例**

   .. code-block:: c

      #include "esp_nsn_iface.h"
      #include "esp_nsn_models.h"
      #include "model_path.h"

      srmodel_list_t *models = esp_srmodel_init("model");
      char *model_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);
      const esp_nsn_iface_t *nsnet = esp_nsnet_handle_from_name(model_name);
      esp_nsn_data_t *nsnet_data = nsnet->create(model_name);

2. **处理音频帧**

   每次调用 ``process()`` 消耗并返回 ``get_samp_chunksize()`` 个采样点——帧移取决于所选模型（nsnet2 为 512 个采样点即 32 ms，nsnet3 为 256 个采样点即 16 ms），缓冲区大小请按返回值分配：

   .. code-block:: c

      int chunk = nsnet->get_samp_chunksize(nsnet_data);  // nsnet2: 512, nsnet3: 256
      int16_t in[chunk], out[chunk];
      nsnet->process(nsnet_data, in, out);

3. **释放资源**

   .. code-block:: c

      nsnet->destroy(nsnet_data);

多通道共享掩码处理
------------------

对于多通道输入（例如麦克风阵列），``esp_nsn_iface_t`` 提供 ``create_mc()`` / ``process_mc()``：ERB 掩码只从参考通道估计**一次**，并应用于所有通道，因此每增加一路通道只需付出加窗/FFT/掩码施加/IFFT/重叠相加的开销，而无需再跑一次完整的网络。nsnet2 与 nsnet3 均支持该模式，语义完全一致。

.. code-block:: c

   /* 4 通道，以通道 0 作为掩码估计的参考通道 */
   esp_nsn_data_t *nsnet_data = nsnet->create_mc(model_name, 4, 0);

   int16_t *in[4]  = {ch0_in, ch1_in, ch2_in, ch3_in};   /* 每个指针指向 chunk 个采样点 */
   int16_t *out[4] = {ch0_out, ch1_out, ch2_out, ch3_out};
   nsnet->process_mc(nsnet_data, in, out);

约束与兼容性说明：

- ``channel_num`` 取值范围为 1 至 8；``ref_channel`` 必须小于 ``channel_num``。
- ``create_mc()`` / ``process_mc()`` 追加在 ``esp_nsn_iface_t`` 末尾，不支持多通道处理的模型中这两个指针为 ``NULL``，使用前请先检查（``examples/nsnet`` 应用中即是如此）。
- 单通道 ``create()`` 等价于 ``create_mc(model_name, 1, 0)``；单通道行为与之前版本保持 bit 级一致，多通道运行的通道 0 输出与相同输入的单通道运行结果 bit 级一致。

nsnet3 权重加载
---------------

nsnet3 的权重以私有二进制形式存放在 ``model`` 分区（``nsnet3_data`` / ``nsnet3_index``，由 Kconfig 选项 ``SR_NSN_NSNET3`` 触发打包烧录）。``create()`` 时将其逐张量拷贝出来（拷贝时按名字与长度逐条校验）。默认配置（``SR_NSNET3_MEM_PSRAM=y``）下，权重、流式状态与激活缓冲全部位于 PSRAM（约 240 KB），几乎不占用内部 SRAM；关闭该选项则全部位于内部 SRAM（约 270 KB），权重读取与片上 rodata 同速。无 PSRAM 的目标自动使用内部 SRAM。

示例
----

``examples/nsnet`` 应用演示了两种接口的用法：

- **SD 卡测试**：从 SD 卡读取 16 kHz 16-bit WAV 文件，处理**所有**输入通道（当输入多于一个通道时使用 ``create_mc()`` / ``process_mc()``，以通道 0 为参考通道），将降噪后的多通道 WAV 写回，并报告 CPU 占用、实时率和内存使用情况（同时写入 SD 卡上的性能日志文件）。
- **USB_SERIAL_JTAG 流式测试**：通过 USB 串口在 PC 主机与芯片之间流式传输音频。在主机侧运行：

  .. code-block:: bash

     python3 stream_host.py --port /dev/ttyACM1 --in test_4ch_in.wav --out out.wav

  主机发送交织排布的 16 kHz 16-bit 音频帧（每帧每通道 ``get_samp_chunksize()`` 个采样点），并读回降噪后的帧；输出 WAV 保留处理后的通道数。

资源消耗
--------

在 ESP32-P4 @ 400 MHz 上实测（OCT PSRAM @ 250 MHz，flash @ 80 MHz）：

.. list-table::
   :header-rows: 1
   :widths: 20 16 18 22 24

   * - 模型（帧移）
     - 通道数
     - 内存位置
     - 每帧耗时 (us)
     - CPU 占用 (%)
   * - nsnet2（32 ms）
     - 1
     - 内部 SRAM
     - 4534
     - 14.2
   * - nsnet2（32 ms）
     - 4（共享掩码）
     - 内部 SRAM
     - 6476
     - 20.2
   * - nsnet3（16 ms）
     - 1
     - PSRAM（默认）
     - 3418
     - 21.37
   * - nsnet3（16 ms）
     - 1
     - 内部 SRAM
     - 3206
     - 20.04
   * - nsnet3（16 ms）
     - 4（共享掩码）
     - PSRAM（默认）
     - 4318
     - 26.99
   * - nsnet3（16 ms）
     - 4（共享掩码）
     - 内部 SRAM
     - 4018
     - 25.11

.. note::

   - 采用共享掩码模式时，每增加一路通道的开销远小于运行一个独立实例（nsnet3 每通道约 +1.7 个百分点 CPU）。
   - nsnet3 对官方金标准音频的逐样本信噪比为 44.55 dB（与离线参考流式输出比对）。
   - 模型的一般资源占用情况参见 :doc:`资源占用 <../benchmark/README>`。

在 ESP32-S31 @ 320 MHz、OCT PSRAM @ 250 MHz 上实测（nsnet3，16 ms 帧移，flash @ 80 MHz）：

.. list-table::
   :header-rows: 1
   :widths: 20 16 18 22 24

   * - 模型（帧移）
     - 通道数
     - 内存位置
     - 每帧耗时 (us)
     - CPU 占用 (%)
   * - nsnet3（16 ms）
     - 1
     - PSRAM（默认）
     - 5680
     - 35.50
   * - nsnet3（16 ms）
     - 1
     - 内部 SRAM
     - 4353
     - 27.21
   * - nsnet3（16 ms）
     - 4（共享掩码）
     - PSRAM（默认）
     - 6815
     - 42.59
   * - nsnet3（16 ms）
     - 4（共享掩码）
     - 内部 SRAM
     - 5387
     - 33.67

.. note::

   - nsnet3 在 ESP32-S31 上的逐样本信噪比同样为 44.55 dB（与离线参考流式输出比对，与 ESP32-P4 一致）。
   - 默认配置（``SR_NSNET3_MEM_PSRAM=y``）下 nsnet3 运行时占用 PSRAM 约 240 KB、内部 SRAM 几乎为 0；关闭该选项则占用内部 SRAM 约 270 KB（权重约 100 KB 于 ``create()`` 时从 model 分区拷入），CPU 占用如上表"内部 SRAM"行。

