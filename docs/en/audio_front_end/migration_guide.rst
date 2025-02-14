Migration from V1.* to V2.*
===========================

:link_to_translation:`zh_CN:[中文]`

Configuration and Initialization
--------------------------------

- 1. The legacy configuration initialization method AFE_CONFIG_DEFAULT() has been removed. Please use ``afe_config_init`` to initialize configurations:
   
   .. code-block:: c

      afe_config_t *afe_config = afe_config_init("MMNR", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
      afe_config_print(afe_config); // print all configurations

- 2. ESP_AFE_SR_HANDLE and ESP_AFE_VC_HANDLE have been removed. Use ``esp_afe_handle_from_config`` to create instances:

   .. code-block:: c

      esp_afe_sr_iface_t *afe_handle = esp_afe_handle_from_config(afe_config);

Input Data Format Changes
---------------------------

The new version supports more flexible input formats via the ``input_format`` parameter. This parameter defines the arrangement of audio channels in the input data.

So You just need to provide the correct ``input_format`` and do not need to rearrange audio data. Each character in the string represents a channel type:

+-----------+---------------------+
| Character | Description         |
+===========+=====================+
| ``M``     | Microphone channel  |
+-----------+---------------------+
| ``R``     | Playback reference  |
|           | channel             |
+-----------+---------------------+
| ``N``     | Unused or unknown   |
|           | channel             |
+-----------+---------------------+

**Example:**  
``MMNR`` indicates four channels, ordered as: microphone channel, microphone channel, unused channel, playback reference channel.

.. note::

   AFE v2.0 introduces additional configuration options. For details, refer to :doc:`AFE <../audio_front_end/README>` and :doc:`VAD <../vadnet/README>`.
