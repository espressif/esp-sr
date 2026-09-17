GSC Beamforming
===============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ESP-SR GSC (Generalized Sidelobe Canceller) module is an adaptive beamformer for microphone arrays. Given the direction of the target speaker, it enhances the speech from that direction while suppressing interference and noise from other directions, and outputs a single-channel enhanced signal. It is typically chained with the :doc:`DOA module <../direction_of_arrival/README>`, which provides the target direction in real time, and is widely used in scenarios such as far-field voice interaction and meeting transcription.

.. note::

   The GSC module is supported on ESP32-P4 and ESP32-S31.

The GSC module has the following features:

- Arbitrary microphone array geometry: any array shape with 2 or more microphones, microphone coordinates are configured at runtime
- Frame size: 128 samples per channel at 16 kHz (8 ms per frame)
- Input: multi-channel 16-bit PCM in planar layout; output: single-channel 16-bit PCM
- The steering direction is updated per frame, supporting moving speakers

Usage
-----

The header file is :project_file:`include/esp32p4/esp_gsc.h`.

**Basic Flow:**

1. **Define the microphone array geometry**

   Microphone coordinates are given in meters, in a right-hand coordinate system, one entry per microphone. The entries may be in any order, but audio channel ``i`` passed to ``esp_gsc_process()`` must always come from the microphone at ``mic_coord[i]``.

   .. code-block:: c

      #include "esp_gsc.h"

      /* 4-mic uniform circular array, radius 5 cm; mic i sits at i*90 degrees */
      PlaneCoord mic_coords[4] = {
          { 0.05f, 0.0f, 0.0f},
          { 0.0f,  0.05f, 0.0f},
          {-0.05f, 0.0f, 0.0f},
          { 0.0f, -0.05f, 0.0f},
      };

2. **Create a GSC instance**

   .. code-block:: c

      gsc_handle_t *gsc = esp_gsc_create(mic_coords, 4);

3. **Process audio frames**

   The input is ``mic_num``-channel 16-bit PCM audio in **planar** layout (``[ch0_0..ch0_127, ch1_0..ch1_127, ...]``), 128 samples per channel per frame. ``loc_phi`` is the direction of the target speaker in degrees, range 0–360, defined in the absolute array coordinate system (0° = positive x-axis, counter-clockwise). The output is 128 samples of single-channel 16-bit PCM.

   .. code-block:: c

      int16_t mic_data[128 * 4];  // 4 channels, planar layout
      int16_t out_data[128];
      esp_gsc_process(gsc, mic_data, 0.0f, out_data);  // steer to 0 degrees

   .. note::

      The adaptive filters need some time (about 1 s) to converge. The enhancement effect builds up gradually after the instance is created or the steering direction changes.

4. **Release resources**

   .. code-block:: c

      esp_gsc_destroy(gsc);

Chaining with DOA
-----------------

In a typical application, the DOA module estimates the speaker direction frame by frame, and the GSC module steers the beam towards that direction:

.. code-block:: c

    /* Pass the SAME coordinate array to both modules */
    esp_doa_capon_embedded_handle_t *doa =
        esp_doa_capon_embedded_create(mic_coords, 4);
    gsc_handle_t *gsc = esp_gsc_create(mic_coords, 4);

    while (1) {
        /* mic_planar: planar 4ch x 128 samples (shared by DOA and GSC) */
        float angle = esp_doa_capon_embedded_process(doa, mic_planar, vad);
        esp_gsc_process(gsc, mic_planar, angle, out_data);
    }

.. warning::

   Both modules must use the same microphone coordinate array, otherwise the estimated angle refers to the wrong channels.

Memory Configuration
--------------------

- On ESP32-P4, the GSC core's internal buffers are allocated in PSRAM by default. To place them in internal RAM instead, define ``GSC_P4_INTERNAL_RAM`` as a compile definition.
- Defining ``ESP_GSC_DISABLE_PSRAM`` before including ``esp_gsc.h`` only moves the small wrapper buffers (handle and frame conversion buffers) to internal RAM; it does not affect the GSC core buffers.

Performance Evaluation
----------------------

The test application ``test_apps/esp-sr-gsc-stream`` evaluates the GSC enhancement performance with the help of the host script ``host_gsc_stream.py``:

- The host streams multi-channel noisy mixture WAV files to the chip frame by frame over the USB-serial-JTAG console (binary lockstep protocol).
- The chip beamforms each frame with ``esp_gsc_process()`` (steered to the known target direction) and sends the mono output back.
- The host computes the SNR (Signal-to-Noise Ratio) of both the input mixture and the beamformed output against the clean target reference, and reports the SNR improvement. The first 125 frames (1 s) are excluded from the metrics to let the adaptive filters converge.

Test results:

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - SNR
     - Improvement (dB)
   * - -5 dB
     - +13.3
   * - 0 dB
     - +12.1
   * - +5 dB
     - +10.6
   * - Overall
     - +12.0

Resource Consumption
--------------------

The following table shows typical resource usage and performance data (16 kHz sample rate):

.. only:: esp32p4

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - Microphones
        - Internal RAM (KB)
        - PSRAM (KB)
        - Time per Frame (ms)
        - CPU Usage (%)
      * - 4
        - 3.5
        - 84.6
        - 1.83 / 8
        - 22.9

    .. note::

      - Frame length is 8 ms (128 samples per channel at 16 kHz).
      - Test setting: ESP32-P4 @ 400 MHz, HEX PSRAM @ 250 MHz, 4-mic uniform circular array (radius 5 cm).
      - If all buffers are placed in internal RAM (by defining the compile option ``GSC_P4_INTERNAL_RAM``), the CPU usage is 18.2%.
      - Actual resource consumption may vary slightly depending on the number of microphones, compiler optimization level, and specific configuration.

.. only:: esp32s31

    .. list-table::
      :header-rows: 1
      :widths: 20 15 15 20 20

      * - Microphones
        - Internal RAM (KB)
        - PSRAM (KB)
        - Time per Frame (ms)
        - CPU Usage (%)
      * - 4
        - 2.1
        - 84.6
        - 2.28 / 8
        - 28.5

    .. note::

      - Frame length is 8 ms (128 samples per channel at 16 kHz).
      - Test setting: ESP32-S31 @ 320 MHz, OCT PSRAM @ 250 MHz, 4-mic uniform circular array (radius 5 cm).
      - Actual resource consumption may vary slightly depending on the number of microphones, compiler optimization level, and specific configuration.
