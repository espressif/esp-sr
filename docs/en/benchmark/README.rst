Benchmark
==========

:link_to_translation:`zh_CN:[中文]`

AFE
---

Resource Consumption
~~~~~~~~~~~~~~~~~~~~


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

    .. list-table:: AFE configuration and pipeline
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
       * - MR, FD, LOW_COST
         - ``|AEC(FD_LOW_COST)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MR, FD, HIGH_PERF
         - ``|AEC(FD_HIGH_PERF)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MMNR, FD, LOW_COST
         - ``|AEC(FD_LOW_COST)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MMNR, FD, HIGH_PERF
         - ``|AEC(FD_HIGH_PERF)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``

    .. note::

        -  **MR:** one microphone channel and one playback channel
        -  **MMNR:** two microphone channels and one playback channels
        -  **Models:** nsnet2, vadnet1_medium, wn9_hilexin

    .. list-table:: AFE configuration and Performance
       :widths: 20 15 15 15 15
       :header-rows: 1

       * - Config
         - Internal RAM (KB)
         - PSRAM (KB)
         - Feed CPU usage (1 core,%)
         - Fetch CPU usage (1 core,%)
       * - MR, SR, LOW_COST
         - 60.1
         - 739.7
         - 8.8
         - 9.8
       * - MR, SR, HIGH_PERF
         - 49.1
         - 775.8
         - 9.3
         - 9.8
       * - MR, FD, LOW_COST
         - 60.2
         - 777.7
         - 12.1
         - 9.8
       * - MR, FD, HIGH_PERF
         - 49.2
         - 813.8
         - 12.5
         - 9.8
       * - MR, VC, LOW_COST
         - 48.7
         - 819.7
         - 30.6
         - 4.7
       * - MR, VC, HIGH_PERF
         - 91.1
         - 822.2
         - 32.2
         - 4.7
       * - MMNR, SR, LOW_COST
         - 79.1
         - 1153.7
         - 23.7
         - 22.9
       * - MMNR, SR, HIGH_PERF
         - 68.1
         - 1200.4
         - 24.9
         - 22.9
       * - MMNR, FD, LOW_COST
         - 79.2
         - 1191.7
         - 29.4
         - 22.9
       * - MMNR, FD, HIGH_PERF
         - 68.1
         - 1238.5
         - 30.4
         - 22.9


.. only:: esp32p4

    .. list-table:: AFE configuration and pipeline
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
       * - MR, FD, LOW_COST
         - ``|AEC(FD_LOW_COST)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MR, FD, HIGH_PERF
         - ``|AEC(FD_HIGH_PERF)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MMNR, FD, LOW_COST
         - ``|AEC(FD_LOW_COST)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``
       * - MMNR, FD, HIGH_PERF
         - ``|AEC(FD_HIGH_PERF)| -> |SE(BSS)| -> |VAD(vadnet1_medium)| -> |WakeNet(wn9_hilexin,)|``


    .. note::

        -  **MR:** one microphone channel and one playback channel
        -  **MMNR:** two microphone channels and one playback channels
        -  **Models:** nsnet2, vadnet1_medium, wn9_hilexin
        -  **Setting:** ESP32-P4 @ 400 MHz, CONFIG_CACHE_L2_CACHE_256KB=y, CONFIG_CACHE_L2_CACHE_LINE_128B=y.

    .. list-table:: AFE configuration and Performance
       :widths: 20 15 15 15 15
       :header-rows: 1

       * - Config
         - Internal RAM (KB)
         - PSRAM (KB)
         - Feed CPU usage (1 core,%)
         - Fetch CPU usage (1 core,%)
       * - MR, SR, LOW_COST
         - 60.1
         - 739.7
         - 8.8
         - 9.8
       * - MR, SR, HIGH_PERF
         - 49.1
         - 775.8
         - 9.3
         - 9.8
       * - MR, FD, LOW_COST
         - 60.2
         - 777.7
         - 12.1
         - 9.8
       * - MR, FD, HIGH_PERF
         - 49.2
         - 813.8
         - 12.5
         - 9.8
       * - MR, VC, LOW_COST
         - 48.7
         - 819.7
         - 30.6
         - 4.7
       * - MR, VC, HIGH_PERF
         - 91.1
         - 822.2
         - 32.2
         - 4.7
       * - MMNR, SR, LOW_COST
         - 79.1
         - 1153.7
         - 23.7
         - 22.9
       * - MMNR, SR, HIGH_PERF
         - 68.1
         - 1200.4
         - 24.9
         - 22.9
       * - MMNR, FD, LOW_COST
         - 79.2
         - 1191.7
         - 29.4
         - 22.9
       * - MMNR, FD, HIGH_PERF
         - 68.1
         - 1238.5
         - 30.4
         - 22.9

WakeNet
-------

.. _resource-occupancyesp32-1:

Resource Consumption
~~~~~~~~~~~~~~~~~~~~

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

Performance Test
~~~~~~~~~~~~~~~~

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

False triggering rate: once in 12 hours

.. note::

    The above test results are based on the ESP32-S3-Korvo V4.0 development board and the WakeNet9 (Alexa) model.

MultiNet
--------

.. _resource-occupancyesp32-2:

Resource Consumption
~~~~~~~~~~~~~~~~~~~~

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
    | MultiNet 7  | 18 KB       | 2920 KB     | 11 ms       | 32 ms       |
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

Word Error Rate Performance Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+-------------+-------------+-------------+
| Model       | librispeech | librispeech |
| Type        | test-clean  | test-other  |
+=============+=============+=============+
| MultiNet5-en| 16.5%       | 41.4%       |
+-------------+-------------+-------------+
| MultiNet6-en| 9.0%        | 21.3%       |
+-------------+-------------+-------------+
| MultiNet7-en| 8.5%        | 21.3%       |
+-------------+-------------+-------------+


Speech Commands Performance Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------+-----------+----------+------------+-------------+
| Model     | Distance  | Quiet    | Stationary | Speech      |
| Type      |           |          | Noise      | Noise       |
|           |           |          | (SNR=5~10dB| (SNR=5~10dB |
|           |           |          | dB)        | dB)         |
+===========+===========+==========+============+=============+
| MultiNet  | 3 m       |  95.4%   |   85.9%    |    82.7%    |
| 5_en      |           |          |            |             |
+-----------+-----------+----------+------------+-------------+
| MultiNet  | 3 m       |  96.8%   |   87.9%    |    85.5%    |
| 6_en      |           |          |            |             |
+-----------+-----------+----------+------------+-------------+
| MultiNet  | 3 m       |  97.2%   |   92.3%    |    90.6%    |
| 7_en      |           |          |            |             |
+-----------+-----------+----------+------------+-------------+

TTS
---

Resource Consumption
~~~~~~~~~~~~~~~~~~~~

Flash image size: 2.2 MB

RAM runtime: 20 KB


Performance Test
~~~~~~~~~~~~~~~~

CPU loading test (ESP32 @240 MHz):

+------------------------------+------+------+------+------+------+------+
| Speech Rate                  | 0    | 1    | 2    | 3    | 4    | 5    |
+==============================+======+======+======+======+======+======+
| Times faster than real time  | 4.5  | 3.2  | 2.9  | 2.5  | 2.2  | 1.8  |
+------------------------------+------+------+------+------+------+------+
