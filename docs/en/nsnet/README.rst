Noise Suppression Model (NSNet)
================================

:link_to_translation:`zh_CN:[中文]`

NSNet is a deep-learning-based noise suppression model for low-power embedded MCUs.

Overview
--------

NSNet currently offers two models: **for maximum AI noise-suppression performance, use nsnet2; for a balance between noise reduction and speech distortion, use nsnet3.**

``nsnet2`` is a quantized neural noise suppression model with the following features:

- Sample rate: 16 kHz, 16-bit PCM
- Frame length: 1024 samples (64 ms), frame shift: 512 samples (32 ms)
- ERB-based spectral masking: the network estimates a time-frequency mask which is applied to the noisy spectrum
- Single-channel processing, plus a multi-channel shared-mask mode (see **Multi-Channel Shared-Mask Processing** below)
- Supported chips: ESP32-S3, ESP32-S31 and ESP32-P4

``nsnet3`` is a float32 noise suppression model with the following features:

- Sample rate: 16 kHz, 16-bit PCM
- Frame length: 512 samples (32 ms), frame shift: 256 samples (16 ms), for lower end-to-end latency
- ERB-based complex ratio mask (CRM): float32 inference with no quantization loss, and only 48.2K parameters (about 100 KB of weights)
- Single-channel processing, plus the same multi-channel shared-mask mode as nsnet2 (identical ``create_mc()`` / ``process_mc()`` semantics)
- Supported chips: ESP32-P4 and ESP32-S31 (both measured; the interface also builds for the ESP32-S3 target)
- About 20% single-core CPU usage (measured on ESP32-P4 @ 400 MHz)

.. note::

   Select the model via ``idf.py menuconfig`` -> ``ESP Speech Recognition`` -> ``Select noise suppression model`` -> ``Deep noise suppression v2 (nsnet2)`` or ``Deep noise suppression v3 (nsnet3)``.

Use NSNet
---------

The interface is defined in ``esp_nsn_iface.h``. All operations go through the ``esp_nsn_iface_t`` function table, which is obtained from the model name. nsnet2 and nsnet3 implement **exactly the same** interface, so the application layer does not need to distinguish between them.

**Basic Flow (single channel):**

1. **Get the model interface and create an instance**

   .. code-block:: c

      #include "esp_nsn_iface.h"
      #include "esp_nsn_models.h"
      #include "model_path.h"

      srmodel_list_t *models = esp_srmodel_init("model");
      char *model_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);
      const esp_nsn_iface_t *nsnet = esp_nsnet_handle_from_name(model_name);
      esp_nsn_data_t *nsnet_data = nsnet->create(model_name);

2. **Process audio frames**

   Each call to ``process()`` consumes and returns ``get_samp_chunksize()`` samples — **the frame shift depends on the selected model** (512 samples = 32 ms for nsnet2, 256 samples = 16 ms for nsnet3); allocate buffers according to the returned value:

   .. code-block:: c

      int chunk = nsnet->get_samp_chunksize(nsnet_data);  // nsnet2: 512, nsnet3: 256
      int16_t in[chunk], out[chunk];
      nsnet->process(nsnet_data, in, out);

3. **Release resources**

   .. code-block:: c

      nsnet->destroy(nsnet_data);

Multi-Channel Shared-Mask Processing
------------------------------------

For multi-channel inputs (e.g., a microphone array), ``esp_nsn_iface_t`` provides ``create_mc()`` / ``process_mc()``: the ERB mask is estimated **once** from the reference channel and applied to every channel, so each additional channel only costs windowing/FFT/mask-application/IFFT/overlap-add instead of a full network pass. Both nsnet2 and nsnet3 support this mode with identical semantics.

.. code-block:: c

   /* 4 channels, channel 0 as the reference for mask estimation */
   esp_nsn_data_t *nsnet_data = nsnet->create_mc(model_name, 4, 0);

   int16_t *in[4]  = {ch0_in, ch1_in, ch2_in, ch3_in};   /* each entry: chunk samples */
   int16_t *out[4] = {ch0_out, ch1_out, ch2_out, ch3_out};
   nsnet->process_mc(nsnet_data, in, out);

Constraints and compatibility notes:

- ``channel_num`` ranges from 1 to 8; ``ref_channel`` must be smaller than ``channel_num``.
- ``create_mc()`` / ``process_mc()`` are appended at the end of ``esp_nsn_iface_t`` and are ``NULL`` for models that do not support multi-channel processing — check them before use (as done in the ``examples/nsnet`` application).
- The single-channel ``create()`` is equivalent to ``create_mc(model_name, 1, 0)``; single-channel behavior is bit-exact with previous versions, and channel 0 of a multi-channel run is bit-identical to a single-channel run of the same input.

nsnet3 Weight Loading
---------------------

nsnet3 ships its weights as a private binary in the ``model`` partition (``nsnet3_data`` / ``nsnet3_index``, packaged and flashed when the ``SR_NSN_NSNET3`` Kconfig option is selected). The first instance's ``create()`` allocates one memory block and copies the weights into it tensor by tensor (with per-tensor name and length verification); the block is freed again when the last instance is destroyed, so a program that never uses nsnet3 pays nothing. The weights, streaming state and activation buffers all live in **PSRAM** (about 240 KB total) and use almost no internal SRAM. Targets without PSRAM automatically use internal SRAM.

Examples
--------

The ``examples/nsnet`` application demonstrates both interfaces:

- **SD card test**: reads a 16 kHz 16-bit WAV file from the SD card, processes **all** input channels (using ``create_mc()`` / ``process_mc()`` when the input has more than one channel, with channel 0 as the reference), writes the enhanced multi-channel WAV back, and reports CPU load, real-time factor and memory usage (also written to a performance log file on the SD card).
- **USB_SERIAL_JTAG streaming test**: streams audio between a host PC and the chip over the USB serial interface. On the host side, run:

  .. code-block:: bash

     python3 stream_host.py --port /dev/ttyACM1 --in test_4ch_in.wav --out out.wav

  The host sends interleaved 16 kHz 16-bit frames (``get_samp_chunksize()`` samples per channel per frame) and reads back the enhanced frames; the output WAV keeps the processed channel count.

Resource Consumption
--------------------

Measured on ESP32-P4 @ 400 MHz (OCT PSRAM @ 250 MHz, flash @ 80 MHz):

.. list-table::
   :header-rows: 1
   :widths: 20 16 18 22 24

   * - Model (frame shift)
     - Channels
     - Memory
     - Time per Frame (us)
     - CPU Usage (%)
   * - nsnet2 (32 ms)
     - 1
     - internal SRAM
     - 4534
     - 14.2
   * - nsnet2 (32 ms)
     - 4 (shared mask)
     - internal SRAM
     - 6476
     - 20.2
   * - nsnet3 (16 ms)
     - 1
     - PSRAM
     - 3418
     - 21.37
   * - nsnet3 (16 ms)
     - 1
     - internal SRAM
     - 3206
     - 20.04
   * - nsnet3 (16 ms)
     - 4 (shared mask)
     - PSRAM
     - 4318
     - 26.99
   * - nsnet3 (16 ms)
     - 4 (shared mask)
     - internal SRAM
     - 4018
     - 25.11

.. note::

   - With the shared-mask mode, each additional channel costs far less than an independent instance (about +1.7 CPU percentage points per channel for nsnet3).
   - nsnet3 scores a sample-by-sample SNR of 44.55 dB against the official golden streaming output.
   - For general model resource occupancy, see :doc:`Resource Occupancy <../benchmark/README>`.

Measured on ESP32-S31 @ 320 MHz, OCT PSRAM @ 250 MHz (nsnet3, 16 ms frame shift, flash @ 80 MHz):

.. list-table::
   :header-rows: 1
   :widths: 20 16 18 22 24

   * - Model (frame shift)
     - Channels
     - Memory
     - Time per Frame (us)
     - CPU Usage (%)
   * - nsnet3 (16 ms)
     - 1
     - PSRAM
     - 5680
     - 35.50
   * - nsnet3 (16 ms)
     - 1
     - internal SRAM
     - 4353
     - 27.21
   * - nsnet3 (16 ms)
     - 4 (shared mask)
     - PSRAM
     - 6815
     - 42.59
   * - nsnet3 (16 ms)
     - 4 (shared mask)
     - internal SRAM
     - 5387
     - 33.67

.. note::

   - nsnet3 also scores a sample-by-sample SNR of 44.55 dB on ESP32-S31 (against the official golden streaming output, identical to ESP32-P4).
   - At runtime nsnet3 occupies about 240 KB of PSRAM (about 100 KB of weights copied dynamically from the model partition on the first instance's ``create()`` and freed on the last ``destroy()``) and almost no internal SRAM; CPU usage follows the table above.
