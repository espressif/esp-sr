NN Beamforming
==============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ESP-SR NN Beamforming module is an end-to-end deep-learning beamforming algorithm: the multi-channel microphone audio is jointly spatial-filtered and spectrally enhanced by a neural network, which directly produces a single-channel enhanced speech output. A single algorithm covers the array noise-reduction needs of products such as conference systems, voice recorders, and recording cards — no array calibration or multi-algorithm cascading required.

NN Beamforming has the following features:

- Sample rate: 16 kHz, 16-bit PCM
- Frame shift: 256 samples (16 ms)
- Multi-channel input (2–8 channels, configured at runtime), single-channel output
- End-to-end deep learning: no microphone array geometry configuration required, adapts to arbitrary array shapes
- End-to-end algorithmic latency: 48 ms
- Per-frame hot data (SCMs, spectrum buffers, etc.) resides in internal RAM; large tables such as model weights stay in PSRAM; zero malloc on the processing path
- Supported chip: ESP32-P4

Usage
-----

The interface is defined in ``esp_nnbf.h``. Before use, load the neural network model and obtain the model name via ``esp_srmodel_init()`` / ``esp_srmodel_filter()``, then configure the pipeline with ``esp_nnbf_config_t``.

**Basic flow:**

1. **Load the model and fill in the configuration**

   .. code-block:: c

      #include "esp_nsn_models.h"
      #include "model_path.h"
      #include "esp_nnbf.h"

      srmodel_list_t *models = esp_srmodel_init("model");
      char *model_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);

      esp_nnbf_config_t cfg;
      esp_nnbf_default_config(&cfg);   /* default: 4 channels, ref channel 0 */

2. **Create a beamforming instance**

   .. code-block:: c

      esp_nnbf_data_t *bf = esp_nnbf_create(model_name, &cfg);

3. **Process audio frames**

   Each call to ``esp_nnbf_process()`` consumes ``channel_num`` inputs of ``esp_nnbf_get_samp_chunksize()`` samples each (256, i.e. 16 ms) and produces 256 single-channel samples. The input layout is planar (per-channel):

   .. code-block:: c

      int chunk = esp_nnbf_get_samp_chunksize(bf);  /* 256 */
      int16_t *in[4] = {ch0, ch1, ch2, ch3};        /* each entry: chunk samples */
      int16_t out[chunk];
      esp_nnbf_process(bf, in, out);

4. **Release resources**

   .. code-block:: c

      esp_nnbf_destroy(bf);

Common ``esp_nnbf_config_t`` parameters:

.. list-table::
   :header-rows: 1
   :widths: 22 14 64

   * - Parameter
     - Default
     - Description
   * - ``channel_num``
     - 4
     - Number of input channels, 2–8 (``ESP_NNBF_MAX_CHANNELS``)
   * - ``ref_channel``
     - 0
     - Reference channel; must be smaller than ``channel_num``

.. note::

   The remaining fields of the configuration structure are algorithm tuning parameters; keeping the default values is recommended.

Examples
--------

The ``examples/nn_beamforming`` application demonstrates streaming multi-channel audio between a host PC and the chip over USB_SERIAL_JTAG: the host sends 16 kHz 16-bit multi-channel PCM frame by frame, and the chip returns the single-channel beamformed output. On the host side, run:

.. code-block:: bash

   cd examples/nn_beamforming
   idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32p4" build flash -p /dev/ttyACM1
   python3 stream_host.py --port /dev/ttyACM1 --in test_4ch_in.wav --out out.wav

   # batch-process a directory of multi-channel WAVs
   python3 stream_host.py --port /dev/ttyACM1 --batch <wav_dir> --out_dir <out_dir>

When streaming finishes, the host script prints the average per-frame time and CPU usage statistics.

Resource Consumption
--------------------

Measured on ESP32-P4 @ 400 MHz (HEX PSRAM @ 250 MHz, flash @ 80 MHz, 4-channel input):

.. list-table::
   :header-rows: 1
   :widths: 30 32 26

   * - Scenario
     - Time per Frame (ms / 16 ms)
     - CPU Usage (%)
   * - Real-audio streaming
     - 9.98
     - 62.4
   * - Silence input
     - 8.93
     - 55.8

.. note::

   - At runtime the module occupies about 370 KB of PSRAM (including about 100 KB of model weights, loaded when the first instance is created; about 237 KB with the post-filter disabled) plus about 91 KB of internal RAM for per-frame hot data (spatial covariance matrices, spectrum buffers, etc.); buffers automatically fall back to PSRAM when internal RAM is short.
   - Unit test (create/destroy ×7 loop) result: 0 B PSRAM leak, internal RAM does not grow with iterations; all test cases pass.

Noise Reduction Performance
---------------------------

Two objective evaluations were performed with on-device streaming on ESP32-P4 @ 400 MHz (HEX PSRAM @ 250 MHz), using the DNSMOS objective score (OVL/SIG/BAK, higher is better). The reference baseline is a single-channel deep-learning noise suppression algorithm evaluated on the reference channel (ch0).

**Synthetic dataset (54 samples)**

4-channel synthetic samples covering 18 noise types × 3 SNRs (-5/0/+5 dB):

.. list-table::
   :header-rows: 1
   :widths: 40 15 15 15

   * - Configuration
     - OVL
     - SIG
     - BAK
   * - Raw noisy input
     - 1.934
     - 3.200
     - 2.008
   * - Single-channel deep-learning noise suppression baseline
     - 2.688
     - 3.207
     - 3.827
   * - NN Beamforming
     - 2.884
     - 3.401
     - 3.928

NN Beamforming improves the mean OVL by +0.196 over the baseline. The per-SNR gains are +0.238 at -5 dB, +0.190 at 0 dB, and +0.159 at +5 dB — the stronger the noise, the larger the advantage.

**Real-world recordings (15 samples)**

4-channel real-world recordings covering 7 noise scenes at two incidence angles (0°/45°), plus one surround-noise recording (15–50 s each):

.. list-table::
   :header-rows: 1
   :widths: 40 15 15 15

   * - Configuration
     - OVL
     - SIG
     - BAK
   * - Raw noisy input
     - 1.455
     - 1.863
     - 1.846
   * - Single-channel deep-learning noise suppression baseline
     - 2.490
     - 2.984
     - 3.982
   * - NN Beamforming
     - 2.620
     - 3.096
     - 4.026

NN Beamforming improves the mean OVL by +0.129 over the baseline, with consistent gains at both incidence angles (+0.155 at 0°, +0.131 at 45°).
