Migration from V1.* to V2.*
===========================

:link_to_translation:`zh_CN:[中文]`

Input Data Format Changes
---------------------------

The new version use ``input_format`` string parameter to define the arrangement of audio channels in the input data. Each character in the string represents a channel type:

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
``MMNR`` indicates four channels, ordered as: microphone channel, microphone channel, unused or unknown channel, playback reference channel.


Configuration and Initialization
--------------------------------

- 1. The configuration initialization method AFE_CONFIG_DEFAULT() has been removed. Please use ``afe_config_init`` to initialize configurations:
   
   .. code-block:: c

      afe_config_t *afe_config = afe_config_init("MMNR", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
      afe_config_print(afe_config); // print all configurations

- 2. ESP_AFE_SR_HANDLE and ESP_AFE_VC_HANDLE have been removed. Use ``esp_afe_handle_from_config`` to create instances:

   .. code-block:: c

      esp_afe_sr_iface_t *afe_handle = esp_afe_handle_from_config(afe_config);


.. note::

   AFE v2.0 introduces additional configuration options. For details, please refer to :doc:`AFE <../audio_front_end/README>` and :doc:`VAD <../vadnet/README>`.
