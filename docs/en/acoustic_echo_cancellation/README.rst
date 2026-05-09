Acoustic Echo Cancellation (AEC)
================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ESP-SR AEC (Acoustic Echo Cancellation) module provides high-performance acoustic echo cancellation, effectively removing the echo of speaker playback captured by the microphone. It is widely used in scenarios such as voice wake-up, voice calls, and full-duplex human-machine interaction.

AEC provides three different implementations covering three application scenarios:

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - Application Scenario
     - Mode
     - Description
   * - Speech Recognition (SR)
     - ``AEC_MODE_SR_LOW_COST``, ``AEC_MODE_SR_HIGH_PERF``
     - Low-cost mode with linear filtering only, small memory footprint and fast speed
   * - Full-Duplex Conversation (FD)
     - ``AEC_MODE_FD_LOW_COST``, ``AEC_MODE_FD_HIGH_PERF``
     - Low-cost full-duplex mode, includes linear filtering + nonlinear processing, suitable for Full-Duplex dialogue scenarios
   * - Voice over IP (VOIP)
     - ``AEC_MODE_VOIP_LOW_COST``, ``AEC_MODE_VOIP_HIGH_PERF``
     - Low-cost call mode, supports 8 kHz / 16 kHz, suitable for ordinary voice calls

.. note::

   Users should select the appropriate mode based on the actual application scenario, resource budget, and performance requirements. It is generally recommended to choose ``AEC_MODE_FD_LOW_COST`` for the best balance between performance and resource consumption.

Usage
-----

The AEC module provides two integration methods:

Method 1: Directly Call the AEC API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Suitable for scenarios requiring fine-grained control over the AEC module. The header file is :project_file:`include/esp32s3/esp_aec.h`.

**Basic Flow:**

1. **Create an AEC instance**

   .. code-block:: c

      #include "esp_aec.h"

      aec_handle_t *aec = aec_create(
          16000,              // Sample rate (Hz), currently only 16000 is supported
          4,                  // Filter length, recommended value is 4, larger values consume more resources
          1,                  // Number of microphone channels
          AEC_MODE_SR_LOW_COST // Working mode
      );

   Or use advanced configuration:

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

2. **Get frame length**

   .. code-block:: c

      int frame_size = aec_get_chunksize(aec);

3. **Allocate audio buffers**

   .. code-block:: c

      int16_t *mic  = heap_caps_aligned_alloc(16, frame_size * sizeof(int16_t), MALLOC_CAP_8BIT);
      int16_t *ref  = heap_caps_aligned_alloc(16, frame_size * sizeof(int16_t), MALLOC_CAP_8BIT);
      int16_t *out  = heap_caps_aligned_alloc(16, frame_size * sizeof(int16_t), MALLOC_CAP_8BIT);

   .. warning::

      All input/output buffers must be **16-bit signed integers (int16_t)**, and it is recommended to allocate them with 16-byte alignment using ``heap_caps_aligned_alloc(16, ...)``.

4. **Process audio frames**

   Full processing (linear filtering + nonlinear processing):

   .. code-block:: c

      aec_process(aec, mic, ref, out);

5. **Release resources**

   .. code-block:: c

      aec_destroy(aec);
      free(mic); free(ref); free(out);

Method 2: Use via the AFE Module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Suitable for scenarios requiring multiple audio front-end algorithms such as AEC, NS (noise suppression), VAD (voice activity detection), and WakeNet (wake word detection) simultaneously. Please refer to the :doc:`Audio Front End <../audio_front_end/README>` for details.


NLP Level Description
---------------------

Nonlinear processing (NLP) is used to further suppress residual echo and can be configured via ``aec_nlp_level_t``. Currently, it is only effective for FD mode:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Level
     - Description
   * - ``AEC_NLP_LEVEL_NORMAL``
     - Normal level, moderate echo suppression, less damage to speech
   * - ``AEC_NLP_LEVEL_AGGR``
     - Aggressive level (default), stronger echo suppression, may affect near-end speech quality to some extent
   * - ``AEC_NLP_LEVEL_VERYAGGR``
     - Very aggressive level, strongest echo suppression, may have a greater impact on near-end speech quality


Resource Consumption
--------------------

The following table shows typical resource usage and performance data for each mode (16 kHz sample rate, single channel):

.. only:: esp32s3

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - Mode
        - Internal RAM (KB)
        - PSRAM (KB)
        - Time per Frame (ms)
        - CPU Usage (%)
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

      - SR/FD mode frame length is 32 ms, VOIP mode frame length is 16 ms.
      - Test setting: ESP32-P4 @ 240 MHz, CONFIG_ESP32S3_DATA_CACHE_64KB=y,CONFIG_ESP32S3_DATA_CACHE_LINE_64B=y.
      - Actual resource consumption may vary slightly depending on the chip model, compiler optimization level, and specific configuration.

.. only:: esp32p4

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - Mode
        - Internal RAM (KB)
        - PSRAM (KB)
        - Time per Frame (ms)
        - CPU Usage (%)
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

      - SR/FD mode frame length is 32 ms, VOIP mode frame length is 16 ms.
      - Test setting: ESP32-P4 @ 400 MHz, CONFIG_CACHE_L2_CACHE_256KB=y, CONFIG_CACHE_L2_CACHE_LINE_128B=y.
      - Actual resource consumption may vary slightly depending on the chip model, compiler optimization level, and specific configuration.

Test Audio Resources
--------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - File Name
     - Description
   * - :download:`aec_in_far.wav <../../_static/aec_in_far.wav>`
     - Far-end signal (speaker playback reference signal)
   * - :download:`aec_in_near.wav <../../_static/aec_in_near.wav>`
     - Near-end signal (microphone signal containing echo)
   * - :download:`aec_test_sr.wav <../../_static/aec_test_sr.wav>`
     - SR mode test audio
   * - :download:`aec_test_voip.wav <../../_static/aec_test_voip.wav>`
     - VOIP mode test audio
   * - :download:`aec_test_fd.wav <../../_static/aec_test_fd.wav>`
     - FD mode test audio
