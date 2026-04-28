声学回声消除 (AEC)
====================

:link_to_translation:`en:[English]`

概述
----

ESP-SR AEC（Acoustic Echo Cancellation）模块提供高性能声学回声消除功能，可有效消除扬声器播放声音在麦克风中的回声，广泛应用于语音唤醒、语音通话、全双工人机交互等场景。

AEC 提供三种不同实现，涵盖三种应用场景：

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - 应用场景
     - 模式
     - 说明
   * - 语音识别 (SR)
     - ``AEC_MODE_SR_LOW_COST``, ``AEC_MODE_SR_HIGH_PERF``
     - 低成本模式，仅包含线性滤波，内存占用小、速度快，适用于对资源敏感的唤醒词识别场景
   * - 全双工对话 (FD)
     - ``AEC_MODE_FD_LOW_COST``, ``AEC_MODE_FD_HIGH_PERF``
     - 低成本全双工模式，包含线性滤波 + 非线性处理，适用于人机对话场景
   * - 语音通话 (VOIP)
     - ``AEC_MODE_VOIP_LOW_COST``, ``AEC_MODE_VOIP_HIGH_PERF``
     - 低成本通话模式，支持 8 kHz / 16 kHz。 后续将不在支持，建议使用 FD 模式替代

.. note::

   用户应根据实际应用场景、资源预算和效果要求选择合适的模式。一般建议选择 AEC_MODE_FD_LOW_COST 模式以获得性能和资源消耗的最佳平衡。

使用方式
--------

AEC 模块提供两种集成方式：

方式一：直接调用 AEC API
^^^^^^^^^^^^^^^^^^^^^^^^

适用于需要对 AEC 模块进行细粒度控制的场景。头文件为 :project_file:`include/esp32s3/esp_aec.h`。

**基本流程：**

1. **创建 AEC 实例**

   .. code-block:: c

      #include "esp_aec.h"

      aec_handle_t *aec = aec_create(
          16000,              // 采样率 (Hz)，目前仅支持 16000
          4,                  // 滤波器长度，推荐值 4，数值越大资源消耗越多
          1,                  // 麦克风通道数
          AEC_MODE_SR_LOW_COST // 工作模式
      );

   或使用高级配置：

   .. code-block:: c

      aec_config_t config = {
          .mic_num       = 1,
          .ref_num       = 1,
          .out_num       = 1,
          .filter_length = 4,
          .sample_rate   = 16000,
          .caps          = MALLOC_CAP_PSRAM | MALLOC_CAP_8BIT,
          .mode          = AEC_MODE_SR_LOW_COST,
          .nlp_level     = AEC_NLP_LEVEL_AGGR,
      };
      aec_handle_t *aec = aec_create_from_config(&config);

2. **获取帧长度**

   .. code-block:: c

      int frame_size = aec_get_chunksize(aec);

3. **分配音频缓冲区**

   .. code-block:: c

      int16_t *mic  = heap_caps_aligned_alloc(16, frame_size * sizeof(int16_t), MALLOC_CAP_8BIT);
      int16_t *ref  = heap_caps_aligned_alloc(16, frame_size * sizeof(int16_t), MALLOC_CAP_8BIT);
      int16_t *out  = heap_caps_aligned_alloc(16, frame_size * sizeof(int16_t), MALLOC_CAP_8BIT);

   .. warning::

      所有输入/输出缓冲区必须为 **16 位有符号整数 (int16_t)**，并建议使用 ``heap_caps_aligned_alloc(16, ...)`` 进行 16 字节对齐分配。

4. **处理音频帧**

   完整处理（线性滤波 + 非线性处理）：

   .. code-block:: c

      aec_process(aec, mic, ref, out);

   或分步处理：

   .. code-block:: c

      aec_linear_process(aec, mic, ref, out);  // 线性滤波
      aec_nlp_process(aec, out);                // 非线性后处理（可选），仅对AEC_MODE_FD_LOW_COST或AEC_MODE_FD_HIGH_PERF模式有效

5. **释放资源**

   .. code-block:: c

      aec_destroy(aec);
      free(mic); free(ref); free(out);

方式二：通过 AFE 模块使用
^^^^^^^^^^^^^^^^^^^^^^^^^

适用于需要同时使用 AEC、NS（降噪）、VAD（语音检测）、WakeNet（唤醒词）等多种音频前端算法的场景。具体使用方法请参考 :doc:`Audio Front End <../audio_front_end/README>` 模块文档。

NLP 级别说明
------------

非线性处理（NLP）用于进一步抑制残余回声，可通过 ``aec_nlp_level_t`` 配置, 目前只对FD模式有效：

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 级别
     - 说明
   * - ``AEC_NLP_LEVEL_NORMAL``
     - 普通级别，回声抑制一般，对语音损伤较小
   * - ``AEC_NLP_LEVEL_AGGR``
     - 激进级别（默认），回声抑制更强，可能会对近端语音质量产生一定影响
   * - ``AEC_NLP_LEVEL_VERYAGGR``
     - 非常激进级别，回声抑制最强，可能对近端语音质量有较大影响

资源消耗
--------

下表为各模式的典型资源占用和性能数据（16 kHz 采样率，单通道）：

.. only:: esp32s3

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - 模式
        - 内部 RAM (KB)
        - PSRAM (KB)
        - 每帧耗时 (ms)
        - CPU 占用 (%)
      * - SR_LOW_COST
        - 18.8
        - 64.0
        - 2.29 / 32
        - 7.2
      * - SR_HIGH_PERF
        - 8.2
        - 100.1
        - 4.51 / 32
        - 14.1
      * - VOIP_LOW_COST
        - 26.9
        - 64.1
        - 4.37 / 16
        - 27.3
      * - VOIP_HIGH_PERF
        - 69.2
        - 66.6
        - 5.05 / 16
        - 31.6
      * - FD_LOW_COST
        - 30.9
        - 90.0
        - 6.28 / 32
        - 19.6
      * - FD_HIGH_PERF
        - 20.3
        - 126.2
        - 8.08 / 32
        - 25.3

    .. note::

      - SR/FD 模式帧长为 32 ms，VOIP 模式帧长为 16 ms。
      - 测试条件：ESP32-S3 @ 240 MHz，CONFIG_ESP32S3_DATA_CACHE_64KB=y,CONFIG_ESP32S3_DATA_CACHE_LINE_64B=y。
      - 实际资源消耗可能因芯片型号、编译优化等级和具体配置略有差异。

.. only:: esp32p4

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - 模式
        - 内部 RAM (KB)
        - PSRAM (KB)
        - 每帧耗时 (ms)
        - CPU 占用 (%)
      * - SR_LOW_COST
        - 18.8
        - 64.0
        - 2.66 / 32
        - 8.3
      * - SR_HIGH_PERF
        - 8.2
        - 100.1
        - 2.72 / 32
        - 8.5
      * - VOIP_LOW_COST
        - 26.9
        - 64.1
        - 2.34 / 16
        - 14.6
      * - VOIP_HIGH_PERF
        - 69.4
        - 66.6
        - 2.60 / 16
        - 16.3
      * - FD_LOW_COST
        - 18.9
        - 102.1
        - 3.69 / 32
        - 11.5
      * - FD_HIGH_PERF
        - 8.3
        - 138.2
        - 3.73 / 32
        - 11.7

    .. note::

      - SR/FD 模式帧长为 32 ms，VOIP 模式帧长为 16 ms。
      - 测试条件：ESP32-P4 @ 400 MHz，CONFIG_CACHE_L2_CACHE_256KB=y，CONFIG_CACHE_L2_CACHE_LINE_128B=y。
      - 实际资源消耗可能因芯片型号、编译优化等级和具体配置略有差异。


测试音频资源
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 文件名
     - 说明
   * - `aec_in_far.wav <docs/_static/aec_in_far.wav>`_
     - 远端信号（扬声器播放参考信号）
   * - `aec_in_near.wav <docs/_static/aec_in_near.wav>`_
     - 近端信号（麦克风采集含回声的信号）
   * - `aec_test_sr.wav <docs/_static/aec_test_sr.wav>`_
     - SR 模式测试音频
   * - `aec_test_voip.wav <docs/_static/aec_test_voip.wav>`_
     - VOIP 模式测试音频
   * - `aec_test_fd.wav <docs/_static/aec_test_fd.wav>`_
     - FD 模式测试音频

