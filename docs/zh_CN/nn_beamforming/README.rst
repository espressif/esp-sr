深度学习波束形成 (NN Beamforming)
==================================

:link_to_translation:`en:[English]`

概述
----

ESP-SR NN Beamforming（神经网络波束形成）模块是一种端到端的深度学习波束形成算法：多路麦克风音频经神经网络联合完成空间滤波与频谱增强，直接输出单通道增强语音。单一算法即可满足会议系统、录音笔、录音卡等产品的阵列降噪需求，无需额外的阵列标定或多算法级联。

NN Beamforming 具有以下特点：

- 采样率：16 kHz，16-bit PCM
- 帧移：256 个采样点（16 ms）
- 多通道输入（2~8 通道，运行时配置），单通道输出
- 端到端深度学习：无需配置麦克风阵列几何坐标，适配任意阵列形状
- 端到端算法延迟：48 ms
- 每帧热数据（SCM、谱缓冲等）驻留内部 RAM，模型权重等大表在 PSRAM，处理路径零 malloc
- 支持芯片：ESP32-P4

使用方式
--------

接口定义在 ``esp_nnbf.h`` 中。使用前先通过 ``esp_srmodel_init()`` / ``esp_srmodel_filter()`` 加载神经网络模型并获取模型名，再以 ``esp_nnbf_config_t`` 完成配置。

**基本流程：**

1. **加载模型并填写配置**

   .. code-block:: c

      #include "esp_nsn_models.h"
      #include "model_path.h"
      #include "esp_nnbf.h"

      srmodel_list_t *models = esp_srmodel_init("model");
      char *model_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);

      esp_nnbf_config_t cfg;
      esp_nnbf_default_config(&cfg);   /* 默认 4 通道、参考通道 0 */

2. **创建波束形成实例**

   .. code-block:: c

      esp_nnbf_data_t *bf = esp_nnbf_create(model_name, &cfg);

3. **处理音频帧**

   每次调用 ``esp_nnbf_process()`` 消耗 ``channel_num`` 路、每路 ``esp_nnbf_get_samp_chunksize()`` 个采样点（256，即 16 ms），输出单通道 256 个采样点。输入为 planar（分通道）排布：

   .. code-block:: c

      int chunk = esp_nnbf_get_samp_chunksize(bf);  /* 256 */
      int16_t *in[4] = {ch0, ch1, ch2, ch3};        /* 每个指针指向 chunk 个采样点 */
      int16_t out[chunk];
      esp_nnbf_process(bf, in, out);

4. **释放资源**

   .. code-block:: c

      esp_nnbf_destroy(bf);

``esp_nnbf_config_t`` 常用参数：

.. list-table::
   :header-rows: 1
   :widths: 22 14 64

   * - 参数
     - 默认值
     - 说明
   * - ``channel_num``
     - 4
     - 输入通道数，2~8（``ESP_NNBF_MAX_CHANNELS``）
   * - ``ref_channel``
     - 0
     - 参考通道，必须小于 ``channel_num``

.. note::

   配置结构体中的其余参数为算法调优参数，建议使用默认值。

示例
----

``examples/nn_beamforming`` 应用演示了通过 USB_SERIAL_JTAG 在 PC 主机与芯片之间流式传输多通道音频：主机逐帧下发 16 kHz 16-bit 多通道 PCM，板端波束形成后回传单通道结果。在主机侧运行：

.. code-block:: bash

   cd examples/nn_beamforming
   idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32p4" build flash -p /dev/ttyACM1
   python3 stream_host.py --port /dev/ttyACM1 --in test_4ch_in.wav --out out.wav

   # 批量处理目录中的多通道 WAV
   python3 stream_host.py --port /dev/ttyACM1 --batch <wav_dir> --out_dir <out_dir>

推流结束后，主机端会打印每帧平均耗时与 CPU 占用统计。

资源消耗
--------

在 ESP32-P4 @ 400 MHz 上实测（HEX PSRAM @ 250 MHz，flash @ 80 MHz，4 通道输入）：

.. list-table::
   :header-rows: 1
   :widths: 30 32 26

   * - 场景
     - 每帧耗时 (ms / 16 ms)
     - CPU 占用 (%)
   * - 真实音频推流
     - 9.98
     - 62.4
   * - 静音输入
     - 8.93
     - 55.8

.. note::

   - 运行时占用 PSRAM 约 370 KB（含模型权重约 100 KB，于首个实例创建时加载；关闭后置滤波时约 237 KB），内部 RAM 约 91 KB（空间协方差矩阵、谱缓冲等每帧热数据）；内部 RAM 不足时自动回退 PSRAM。
   - 单元测试（create/destroy 循环 ×7）结论：PSRAM 泄漏 0 B，内部 RAM 不随次数增长；全部测试用例通过。

降噪效果
--------

在 ESP32-P4 @ 400 MHz（HEX PSRAM @ 250 MHz）上以实板推流方式完成两组客观评测，指标为 DNSMOS 客观评分（OVL/SIG/BAK，越高越好），对比基线为单通道深度学习降噪（取参考通道 ch0 输出）。

**合成数据集（54 组样本）**

覆盖 18 种噪声类型 × 3 个信噪比（-5/0/+5 dB）的 4 通道合成样本：

.. list-table::
   :header-rows: 1
   :widths: 40 15 15 15

   * - 配置
     - OVL
     - SIG
     - BAK
   * - 原始含噪输入（raw）
     - 1.934
     - 3.200
     - 2.008
   * - 单通道深度学习降噪基线
     - 2.688
     - 3.207
     - 3.827
   * - NN Beamforming
     - 2.884
     - 3.401
     - 3.928

NN Beamforming 平均 OVL 较基线提升 +0.196；分信噪比提升为 -5 dB 时 +0.238、0 dB 时 +0.190、+5 dB 时 +0.159，噪声越强优势越明显。

**真实录音（15 组样本）**

覆盖 7 种噪声场景 × 0°/45° 两个入射角度、另有 1 组环绕噪声的 4 通道真实录音（每组 15~50 s）：

.. list-table::
   :header-rows: 1
   :widths: 40 15 15 15

   * - 配置
     - OVL
     - SIG
     - BAK
   * - 原始含噪输入（raw）
     - 1.455
     - 1.863
     - 1.846
   * - 单通道深度学习降噪基线
     - 2.490
     - 2.984
     - 3.982
   * - NN Beamforming
     - 2.620
     - 3.096
     - 4.026

NN Beamforming 平均 OVL 较基线提升 +0.129，且在 0° 与 45° 两个入射角度均稳定提升（+0.155 / +0.131）。
