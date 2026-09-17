声源定向 (DOA)
================

:link_to_translation:`en:[English]`

概述
----

ESP-SR DOA（Direction of Arrival，声源定向）模块用于估计声源相对于麦克风阵列的方位角。该模块基于 Capon/MVDR（最小方差无失真响应）算法，并针对嵌入式平台做了优化，广泛应用于说话人定位、摄像头转向等场景，也可作为波束形成的前级模块（参见 :doc:`GSC 波束形成 <../gsc_beamforming/README>`）。

.. note::

   本文档介绍的嵌入式 DOA 模块目前支持 ESP32-P4 和 ESP32-S31。

嵌入式 DOA 模块具有以下特点：

- 支持任意阵列几何形状：2 至 8 个麦克风，麦克风坐标在运行时配置
- 帧长：16 kHz 采样率下每通道 128 个采样点（每帧 8 ms）
- FFT 点数：256
- 处理频带：默认 1500–4500 Hz（针对语音优化）；可通过 ``esp_doa_capon_embedded_create_with_band()`` 在运行时配置（见下文 **自定义处理频带** 一节），默认频带也可通过 ``DOA_LOW_FREQ`` / ``DOA_HIGH_FREQ`` / ``DOA_FREQ_SPACING`` 宏在编译时覆盖
- 角度分辨率：10 度（36 个候选角度：0°、10°、…、350°）
- 仅使用单精度浮点运算
- 处理过程中零动态内存分配（所有缓冲区在创建时预分配）

使用方式
--------

头文件为 :project_file:`include/esp32p4/esp_doa_capon_embedded.h`。

**基本流程：**

1. **定义麦克风阵列几何坐标**

   麦克风坐标以米为单位，采用右手坐标系，每个麦克风一项。各坐标项可以是任意顺序，但传入 ``esp_doa_capon_embedded_process()`` 的第 ``i`` 路音频必须始终来自 ``mic_coord[i]`` 对应的麦克风。

   .. code-block:: c

      #include "esp_doa_capon_embedded.h"

      /* 4 麦均匀圆阵，半径 5 cm；第 i 个麦克风位于 i*90 度方向 */
      PlaneCoord mic_coords[4] = {
          { 0.05f, 0.0f, 0.0f},
          { 0.0f,  0.05f, 0.0f},
          {-0.05f, 0.0f, 0.0f},
          { 0.0f, -0.05f, 0.0f},
      };

2. **创建 DOA 实例**

   所有内存（句柄和内部内存池）由模块自行分配，并由 ``esp_doa_capon_embedded_destroy()`` 释放。缓冲区默认从 PSRAM 分配，参见下文 **内存配置** 一节。

   .. code-block:: c

      esp_doa_capon_embedded_handle_t *doa =
          esp_doa_capon_embedded_create(mic_coords, 4);

3. **处理音频帧**

   输入为 ``mic_num`` 通道 16-bit PCM 音频，**planar（分通道）** 排布（``[ch0_0..ch0_127, ch1_0..ch1_127, ...]``），每帧每通道 128 个采样点。

   .. code-block:: c

      int16_t audio_frame[128 * 4];  // 4 通道，planar 排布
      int vad = 1;                   // 1 = 语音，0 = 噪声/静音
      float angle = esp_doa_capon_embedded_process(doa, audio_frame, vad);

   返回角度单位为度，范围 0–360，定义在阵列绝对坐标系中（0° = x 轴正方向，逆时针），与 ``mic_coord`` 中麦克风的排列顺序无关。出错时返回 ``-1.0f``。

   .. note::

      - 当 ``vad_result`` 为 0 时，所有自适应状态（协方差递推、矩阵求逆、空间谱）都会被冻结，并直接返回上一次估计的角度。建议接入 AFE 模块的 VAD 结果，避免纯噪声帧破坏估计。
      - 协方差递推需要若干帧才能收敛，创建（或复位）后最初几帧的估计结果应丢弃。

4. **（可选）复位处理器状态**

   复位协方差矩阵和平滑滤波器，适用于长时间静音后重新开始估计等场景：

   .. code-block:: c

      esp_doa_capon_embedded_reset(doa);

5. **释放资源**

   ``esp_doa_capon_embedded_destroy()`` 释放 ``esp_doa_capon_embedded_create()`` 分配的全部资源（传入 NULL 会被安全忽略）：

   .. code-block:: c

      esp_doa_capon_embedded_destroy(doa);

.. warning::

   将 DOA 与 :doc:`GSC 波束形成 <../gsc_beamforming/README>` 级联使用时，两个模块必须传入**相同的**麦克风坐标数组，否则估计出的角度会对应错误的通道。

.. tip::

   ``esp_doa_capon_embedded_print_info(doa)`` 可打印处理器配置（帧长、FFT 点数、处理频带等），用于调试。

自定义处理频带
----------------

默认情况下，Capon 空间谱在 1500–4500 Hz 范围内以 100 Hz 间隔（31 个频点）计算。如需在运行时调整频带，请使用 ``esp_doa_capon_embedded_create_with_band()`` 代替 ``esp_doa_capon_embedded_create()`` 创建实例：

.. code-block:: c

   esp_doa_capon_embedded_handle_t *doa =
       esp_doa_capon_embedded_create_with_band(mic_coords, 4,
                                               2000,  /* low_freq (Hz) */
                                               6000,  /* high_freq (Hz) */
                                               200);  /* freq_spacing (Hz) */

频带在创建时校验，违反以下任一约束时返回 ``NULL``：

- ``0 < low_freq < high_freq <= 8000`` （16 kHz 采样率下的奈奎斯特频率）
- ``freq_spacing >= 63`` Hz（FFT 频点分辨率为 62.5 Hz）
- ``(high_freq - low_freq) / freq_spacing + 1 <= 129`` 个频点

频带选择指导（声速 c = 340 m/s）：

- ``low_freq >= c / (2 * 阵列孔径)``，保证有效的指向性
- ``high_freq <= c / (2 * 最小麦克风间距)``，避免栅瓣

.. note::

   处理时间与参与计算的频点数量成线性关系。

``esp_doa_capon_embedded_create()`` 使用的默认频带也可在编译时通过定义 ``DOA_LOW_FREQ`` 、 ``DOA_HIGH_FREQ`` 和 ``DOA_FREQ_SPACING`` （例如作为编译选项）修改。

内存配置
--------

- 在 ESP32-P4 上，DOA 内存池分为两部分（4 麦共约 206 KB）：

  - 每帧热缓冲（约 57 KB）始终分配在内部 RAM 中，以保证访问速度；
  - 只读的导向矢量表（约 149 KB）默认分配在 PSRAM 中。

- 如需全部改用内部 RAM，可在包含 ``esp_doa_capon_embedded.h`` 之前定义 ``ESP_DOA_DISABLE_PSRAM``，或将其作为编译选项定义。

精度测试
--------

测试程序 ``test_apps/esp-sr-gsc-doa`` 在芯片端评估 DOA 估计精度。测试数据集 ``data_4mic_r5cm_quite`` 为仿真的 4 麦均匀圆阵（半径 5 cm）纯净语音，声源位于半径 2 m 的圆周上，角度从 0° 到 330°、步进 30°（从 +x 轴逆时针计量）。

测试方法：

- 对 12 个角度，每个角度向 ``esp_doa_capon_embedded_process()`` 送入 64 帧（VAD 强制为语音）。
- 跳过前 10 帧，等待协方差递推收敛。
- 将剩余帧的估计结果与真实角度比较，分别统计完全命中率和误差在一个网格步长（±10°）以内的准确率。

测试结果：

.. list-table::
   :header-rows: 1
   :widths: 40 30 30

   * - 指标
     - 结果
     - 说明
   * - 完全命中率
     - 100% (648/648)
     - 估计角度与真实角度完全一致
   * - ±10° 内准确率
     - 100% (648/648)
     - 误差在一个网格步长以内

资源消耗
--------

下表为典型的资源占用与性能数据（16 kHz 采样率）：

.. only:: esp32p4

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - 麦克风数量
        - 内部 RAM (KB)
        - PSRAM (KB)
        - 每帧耗时 (ms)
        - CPU 占用 (%)
      * - 4
        - 57
        - 149
        - 0.65 / 8
        - 8.1

    .. note::

      - 帧长为 8 ms（16 kHz 采样率下每通道 128 个采样点）。
      - 测试条件：ESP32-P4 @ 400 MHz，HEX PSRAM @ 250 MHz，4 麦均匀圆阵（半径 5 cm）。
      - 实际资源消耗可能因麦克风数量、编译器优化等级和具体配置略有差异。

.. only:: esp32s31

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - 麦克风数量
        - 内部 RAM (KB)
        - PSRAM (KB)
        - 每帧耗时 (ms)
        - CPU 占用 (%)
      * - 4
        - 58.9
        - 148.3
        - 0.82 / 8
        - 10.3

    .. note::

      - 帧长为 8 ms（16 kHz 采样率下每通道 128 个采样点）。
      - 测试条件：ESP32-S31 @ 320 MHz，OCT PSRAM @ 250 MHz，4 麦均匀圆阵（半径 5 cm）。
      - 实际资源消耗可能因麦克风数量、编译器优化等级和具体配置略有差异。
