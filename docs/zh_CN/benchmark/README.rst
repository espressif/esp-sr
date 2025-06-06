性能测试结果
==============

:link_to_translation:`en:[English]`

AFE
---

资源消耗
~~~~~~~~

.. only:: esp32

    +-----------------+-----------------+-----------------+-----------------+
    | Algorithm Type  | RAM             | Average cpu     | Frame Length    |
    |                 |                 | loading(compute |                 |
    |                 |                 | with 2 cores)   |                 |
    +=================+=================+=================+=================+
    | AEC(HIGH_PERF)  | 114 KB          | 11%             | 32 ms           |
    +-----------------+-----------------+-----------------+-----------------+
    | NS              | 27 KB           | 5%              | 10 ms           |
    +-----------------+-----------------+-----------------+-----------------+
    | AFE Layer       | 73 KB           |                 |                 |
    +-----------------+-----------------+-----------------+-----------------+

.. only:: esp32s3

    .. list-table:: AFE 配置和算法流程
        :widths: 25 75
        :header-rows: 1

        * - Config
          - Pipeline
        * - MR, SR, LOW_COST
          - ``|AEC(SR_LOW_COST)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
        * - MR, SR, HIGH_PERF
          - ``|AEC(SR_HIGH_PERF)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
        * - MR, VC, LOW_COST
          - ``|AEC(VOIP_LOW_COST)| -> |NS(nsnet2)| -> |VAD(vadnet1_medium)|``
        * - MR, VC, HIGH_PERF
          - ``|AEC(VOIP_HIGH_PERF)| -> |NS(nsnet2)| -> |VAD(vadnet1_medium)|``
        * - MMNR, SR, LOW_COST
          - ``|AEC(SR_LOW_COST)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
        * - MMNR, SR, HIGH_PERF
          - ``|AEC(SR_HIGH_PERF)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``


    .. note::

        -  **MR:** 一个麦克风通道和一个播放通道
        -  **MMNR:** 两个麦克风通道和一个播放通道
        -  **Models:** nsnet2, vadnet1_medium, wn9_hilexin

    .. list-table:: AFE 配置和性能
       :widths: 25 15 15 20 20
       :header-rows: 1

       * - Config
         - Internal RAM (KB)
         - PSRAM (KB)
         - Feed CPU usage (1 core,%)
         - Fetch CPU usage (1 core,%)
       * - MR, SR, LOW_COST
         - 72.3
         - 732.7
         - 8.4
         - 15.0
       * - MR, SR, HIGH_PERF
         - 78.0
         - 734.7
         - 9.4
         - 14.9
       * - MR, VC, LOW_COST
         - 50.3
         - 821.4
         - 60.0
         - 8.2
       * - MR, VC, HIGH_PERF
         - 93.7
         - 824.0
         - 64.0
         - 8.2
       * - MMNR, SR, LOW_COST
         - 76.6
         - 1173.9
         - 36.6
         - 30.0
       * - MMNR, SR, HIGH_PERF
         - 99.0
         - 1173.7
         - 38.8
         - 30.0


.. only:: esp32p4

    .. list-table:: AFE 配置和算法流程
       :widths: 25 75
       :header-rows: 1

       * - Config
         - Pipeline
       * - MR, SR, LOW_COST
         - ``|AEC(SR_LOW_COST)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MR, SR, HIGH_PERF
         - ``|AEC(SR_HIGH_PERF)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MR, VC, LOW_COST
         - ``|AEC(VOIP_LOW_COST)| -> |NS(nsnet2)| -> |VAD(vadnet1_medium)|``
       * - MR, VC, HIGH_PERF
         - ``|AEC(VOIP_HIGH_PERF)| -> |NS(nsnet2)| -> |VAD(vadnet1_medium)|``
       * - MMNR, SR, LOW_COST
         - ``|AEC(SR_LOW_COST)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MMNR, SR, HIGH_PERF
         - ``|AEC(SR_HIGH_PERF)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``


    .. note::

        -  **MR:** 一个麦克风通道和一个播放通道
        -  **MMNR:** 两个麦克风通道和一个播放通道
        -  **Models:** nsnet2, vadnet1_medium, wn9_hilexin

    .. list-table:: AFE 配置和性能
       :widths: 25 15 15 20 20
       :header-rows: 1

       * - Config
         - Internal RAM (KB)
         - PSRAM (KB)
         - Feed CPU usage (1 core,%)
         - Fetch CPU usage (1 core,%)
       * - MR, SR, LOW_COST
         - 73.6
         - 733.2
         - 10.6
         - 11.2
       * - MR, SR, HIGH_PERF
         - 73.3
         - 733.2
         - 10.6
         - 11.2
       * - MR, VC, LOW_COST
         - 74.4
         - 821.3
         - 40.2
         - 5.7
       * - MR, VC, HIGH_PERF
         - 116.7
         - 823.9
         - 42.4
         - 5.7
       * - MMNR, SR, LOW_COST
         - 78.0
         - 1173.0
         - 28.2
         - 24.8
       * - MMNR, SR, HIGH_PERF
         - 78.0
         - 1173.0
         - 28.2
         - 24.8

WakeNet
-------

.. _resource-occupancyesp32-1:

资源消耗
~~~~~~~~

.. only:: esp32

    +-------------+-------------+-------------+-------------+-------------+
    | Model Type  | Parameter   | RAM         | Average     | Frame       |
    |             | Num         |             | Running     | Length      |
    |             |             |             | Time per    |             |
    |             |             |             | Frame       |             |
    +=============+=============+=============+=============+=============+
    | Quantised   | 41 K        | 15 KB       | 5.5 ms      | 30 ms       |
    | WakeNet5    |             |             |             |             |
    +-------------+-------------+-------------+-------------+-------------+
    | Quantised   | 165 K       | 20 KB       | 10.5 ms     | 30 ms       |
    | WakeNet5X2  |             |             |             |             |
    +-------------+-------------+-------------+-------------+-------------+
    | Quantised   | 371 K       | 24 KB       | 18 ms       | 30 ms       |
    | WakeNet5X3  |             |             |             |             |
    +-------------+-------------+-------------+-------------+-------------+

.. _resource-occupancyesp32s3-1:

.. only:: esp32s3

    +----------------+-------+---------+----------------+--------------+
    | Model Type     | RAM   | PSRAM   | Average        | Frame Length |
    |                |       |         | Running Time   |              |
    |                |       |         | per Frame      |              |
    +================+=======+=========+================+==============+
    | Quantised      | 50 KB | 1640 KB | 10.0 ms        | 32 ms        |
    | WakeNet8 @ 2   |       |         |                |              |
    | channel        |       |         |                |              |
    +----------------+-------+---------+----------------+--------------+
    | Quantised      | 16 KB | 324 KB  | 3.0 ms         | 32 ms        |
    | WakeNet9 @ 2   |       |         |                |              |
    | channel        |       |         |                |              |
    +----------------+-------+---------+----------------+--------------+
    | Quantised      | 20 KB | 347 KB  | 4.3 ms         | 32 ms        |
    | WakeNet9 @ 3   |       |         |                |              |
    | channel        |       |         |                |              |
    +----------------+-------+---------+----------------+--------------+

.. only:: esp32p4

    +----------------+-------+---------+----------------+--------------+
    | Model Type     | RAM   | PSRAM   | Average        | Frame Length |
    |                |       |         | Running Time   |              |
    |                |       |         | per Frame      |              |
    +================+=======+=========+================+==============+
    | Quantised      | 16 KB | 324 KB  | 2.6 ms         | 32 ms        |
    | WakeNet9 @ 2   |       |         |                |              |
    | channel        |       |         |                |              |
    +----------------+-------+---------+----------------+--------------+
    | Quantised      | 20 KB | 347 KB  | 3.1 ms         | 32 ms        |
    | WakeNet9 @ 3   |       |         |                |              |
    | channel        |       |         |                |              |
    +----------------+-------+---------+----------------+--------------+

性能测试
~~~~~~~~

+-------------+-------------+-------------+-------------+-------------+
| Distance    | Quiet       | Stationary  | Speech      | AEC         |
|             |             | Noise (SNR  | Noise (SNR  | I           |
|             |             | = 4 dB)     | = 4 dB)     | nterruption |
|             |             |             |             | (-10 dB)    |
+=============+=============+=============+=============+=============+
| 1 m         | 98%         | 96%         | 94%         | 96%         |
+-------------+-------------+-------------+-------------+-------------+
| 3 m         | 98%         | 96%         | 94%         | 94%         |
+-------------+-------------+-------------+-------------+-------------+

误触发率：12 小时 1 次

.. note::

    以上测试结果基于 ESP32-S3-Korvo V4.0 开发板和 WakeNet9(Alexa) 模型。

MultiNet
--------

.. _resource-occupancyesp32-2:

资源消耗
~~~~~~~~

.. only:: esp32

    +-------------+-------------+-------------+-------------+-------------+
    | Model Type  | Internal    | PSRAM       | Average     | Frame       |
    |             | RAM         |             | Running     | Length      |
    |             |             |             | Time per    |             |
    |             |             |             | Frame       |             |
    +=============+=============+=============+=============+=============+
    | MultiNet 2  | 13.3 KB     | 9KB         | 38 ms       | 30 ms       |
    +-------------+-------------+-------------+-------------+-------------+

.. _resource-occupancyesp32s3-2:

.. only:: esp32s3

    +-------------+-------------+-------------+-------------+-------------+
    | Model Type  | Internal    | PSRAM       | Average     | Frame       |
    |             | RAM         |             | Running     | Length      |
    |             |             |             | Time per    |             |
    |             |             |             | Frame       |             |
    +=============+=============+=============+=============+=============+
    | MultiNet 4  | 16.8KB      | 1866 KB     | 18 ms       | 32 ms       |
    +-------------+-------------+-------------+-------------+-------------+
    | MultiNet 4  | 10.5 KB     | 1009 KB     | 11 ms       | 32 ms       |
    | Q8          |             |             |             |             |
    +-------------+-------------+-------------+-------------+-------------+
    | MultiNet 5  | 16 KB       | 2310 KB     | 12 ms       | 32 ms       |
    | Q8          |             |             |             |             |
    +-------------+-------------+-------------+-------------+-------------+
    | MultiNet 6  | 32 KB       | 4100 KB     | 12 ms       | 32 ms       |
    +-------------+-------------+-------------+-------------+-------------+

.. only:: esp32p4

    +-------------+-------------+-------------+-------------+-------------+
    | Model Type  | Internal    | PSRAM       | Average     | Frame       |
    |             | RAM         |             | Running     | Length      |
    |             |             |             | Time per    |             |
    |             |             |             | Frame       |             |
    +=============+=============+=============+=============+=============+
    | MultiNet 7  | 18 KB       | 2920 KB     | 8 ms        | 32 ms       |
    +-------------+-------------+-------------+-------------+-------------+

Word Error Rate 性能测试
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------+-----------+
| Model     | aishell   |
| Type      | test      |
+===========+===========+
| MultiNet  |    9.5%   |
| 5_cn      |           |
+-----------+-----------+
| MultiNet  |    5.2%   |
| 6_cn      |           |
+-----------+-----------+

.. note::

    中文使用没有声调的拼音单元去计算 WER。

Speech Commands 性能测试（空调控制场景）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------+-----------+----------+------------+-------------+
| Model     | Distance  | Quiet    | Stationary | Speech      |
| Type      |           |          | Noise      | Noise       |
|           |           |          | (SNR=5~10dB| (SNR=5~10dB |
|           |           |          | dB)        | dB)         |
+===========+===========+==========+============+=============+
| MultiNet  | 3 m       |  88.9%   |   66.1%    |   67.5%     |
| 5_cn      |           |          |            |             |
+-----------+-----------+----------+------------+-------------+
| MultiNet  | 3 m       |  98.8%   |   88.3%    |   88.0%     |
| 6_cn      |           |          |            |             |
+-----------+-----------+----------+------------+-------------+
| MultiNet  | 3 m       |  97.1%   |   95.1%    |   96.8%     |
| 6_cn_ac   |           |          |            |             |
+-----------+-----------+----------+------------+-------------+

.. note::

    MultiNet6_cn_ac在空调场景数据集上进行了进一步的微调，所以在空调控制场景具有更好的性能。


TTS
---

资源消耗
~~~~~~~~

Flash image size: 2.2 MB

RAM runtime: 20 KB


性能测试
~~~~~~~~

CPU 负载测试（ESP32 @240 MHz）：

+------------------------------+------+------+------+------+------+------+
| Speech Rate                  | 0    | 1    | 2    | 3    | 4    | 5    |
+==============================+======+======+======+======+======+======+
| Times faster than real time  | 4.5  | 3.2  | 2.9  | 2.5  | 2.2  | 1.8  |
+------------------------------+------+------+------+------+------+------+


NSNET
-----

性能测试
~~~~~~~~
数据集：array_onemic_nnoise_20230608(按照亚马逊声学认证标准录制测试集)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+------------------+--------+
|                  | dnsmos |
+==================+========+
| nsnet1           | 2.4    |
+------------------+--------+
| nsnet2           | 2.71   |
+------------------+--------+
