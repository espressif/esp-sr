GSC 波束形成
==============

:link_to_translation:`en:[English]`

概述
----

ESP-SR GSC（Generalized Sidelobe Canceller，广义旁瓣抵消器）模块是面向麦克风阵列的自适应波束形成器。在给定目标说话人方向的条件下，它可以增强该方向的语音，同时抑制其他方向的干扰和噪声，输出单通道增强信号。它通常与 :doc:`DOA 模块 <../direction_of_arrival/README>` 级联使用，由 DOA 实时提供目标方向，广泛应用于远场语音交互、会议转写等场景。

.. note::

   GSC 模块目前支持 ESP32-P4 和 ESP32-S31。

GSC 模块具有以下特点：

- 支持任意阵列几何形状：2 个及以上麦克风，麦克风坐标在运行时配置
- 帧长：16 kHz 采样率下每通道 128 个采样点（每帧 8 ms）
- 输入为多通道 16-bit PCM（planar 排布），输出为单通道 16-bit PCM
- 定向方向可按帧更新，支持移动说话人

使用方式
--------

头文件为 :project_file:`include/esp32p4/esp_gsc.h`。

**基本流程：**

1. **定义麦克风阵列几何坐标**

   麦克风坐标以米为单位，采用右手坐标系，每个麦克风一项。各坐标项可以是任意顺序，但传入 ``esp_gsc_process()`` 的第 ``i`` 路音频必须始终来自 ``mic_coord[i]`` 对应的麦克风。

   .. code-block:: c

      #include "esp_gsc.h"

      /* 4 麦均匀圆阵，半径 5 cm；第 i 个麦克风位于 i*90 度方向 */
      PlaneCoord mic_coords[4] = {
          { 0.05f, 0.0f, 0.0f},
          { 0.0f,  0.05f, 0.0f},
          {-0.05f, 0.0f, 0.0f},
          { 0.0f, -0.05f, 0.0f},
      };

2. **创建 GSC 实例**

   .. code-block:: c

      gsc_handle_t *gsc = esp_gsc_create(mic_coords, 4);

3. **处理音频帧**

   输入为 ``mic_num`` 通道 16-bit PCM 音频，**planar（分通道）** 排布（``[ch0_0..ch0_127, ch1_0..ch1_127, ...]``），每帧每通道 128 个采样点。``loc_phi`` 为目标说话人方向，单位为度，范围 0–360，定义在阵列绝对坐标系中（0° = x 轴正方向，逆时针）。输出为 128 个采样点的单通道 16-bit PCM。

   .. code-block:: c

      int16_t mic_data[128 * 4];  // 4 通道，planar 排布
      int16_t out_data[128];
      esp_gsc_process(gsc, mic_data, 0.0f, out_data);  // 定向到 0 度

   .. note::

      自适应滤波器需要一定时间（约 1 秒）收敛。实例创建或定向方向改变后，增强效果会逐渐建立。

4. **释放资源**

   .. code-block:: c

      esp_gsc_destroy(gsc);

与 DOA 级联
-----------

在典型应用中，DOA 模块逐帧估计说话人方向，GSC 模块将波束对准该方向：

.. code-block:: c

    /* 两个模块传入相同的坐标数组 */
    esp_doa_capon_embedded_handle_t *doa =
        esp_doa_capon_embedded_create(mic_coords, 4);
    gsc_handle_t *gsc = esp_gsc_create(mic_coords, 4);

    while (1) {
        /* mic_planar：planar 排布的 4 通道 x 128 采样点（DOA 与 GSC 共用） */
        float angle = esp_doa_capon_embedded_process(doa, mic_planar, vad);
        esp_gsc_process(gsc, mic_planar, angle, out_data);
    }

.. warning::

   两个模块必须使用相同的麦克风坐标数组，否则估计出的角度会对应错误的通道。

内存配置
--------

- 在 ESP32-P4 上，GSC 核心的内部缓冲区默认分配在 PSRAM 中。如需改用内部 RAM，可定义编译选项 ``GSC_P4_INTERNAL_RAM``。
- 在包含 ``esp_gsc.h`` 之前定义 ``ESP_GSC_DISABLE_PSRAM``，只会将少量的封装层缓冲区（句柄和帧转换缓冲区）移到内部 RAM，不影响 GSC 核心缓冲区。

性能测试
--------

测试程序 ``test_apps/esp-sr-gsc-stream`` 配合主机脚本 ``host_gsc_stream.py`` 评估 GSC 的增强性能：

- 主机通过 USB-serial-JTAG 控制台，以二进制锁步协议将多通道含噪混合音频逐帧发送到芯片。
- 芯片使用 ``esp_gsc_process()`` 对每帧做波束形成（定向到已知的目标方向），并将单通道输出回传。
- 主机分别计算输入混合音频和波束形成输出相对于干净目标参考的 SNR（信噪比），并报告 SNR 提升量。前 125 帧（1 秒）不计入指标，以等待自适应滤波器收敛。

测试结果：

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - 信噪比
     - 提升 (dB)
   * - -5 dB
     - +13.3
   * - 0 dB
     - +12.1
   * - +5 dB
     - +10.6
   * - 总体
     - +12.0

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
        - 3.5
        - 84.6
        - 1.83 / 8
        - 22.9

    .. note::

      - 帧长为 8 ms（16 kHz 采样率下每通道 128 个采样点）。
      - 测试条件：ESP32-P4 @ 400 MHz，HEX PSRAM @ 250 MHz，4 麦均匀圆阵（半径 5 cm）。
      - 若将缓冲区全部放在内部 RAM（定义编译选项 ``GSC_P4_INTERNAL_RAM``），CPU 占用为 18.2%。
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
        - 2.1
        - 84.6
        - 2.28 / 8
        - 28.5

    .. note::

      - 帧长为 8 ms（16 kHz 采样率下每通道 128 个采样点）。
      - 测试条件：ESP32-S31 @ 320 MHz，OCT PSRAM @ 250 MHz，4 麦均匀圆阵（半径 5 cm）。
      - 实际资源消耗可能因麦克风数量、编译器优化等级和具体配置略有差异。
