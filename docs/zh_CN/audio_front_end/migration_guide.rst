从 V1.* 迁移到 V2.*
===========================

:link_to_translation:`en:[English]`

输入数据格式修改
---------------------------

新版本通过 ``input_format`` 参数定义了输入数据中音频通道的排列方式。字符串中的每个字符代表一种通道类型：

+-----------+---------------------+
| 字符      | 描述                |
+===========+=====================+
| ``M``     | 麦克风通道          |
+-----------+---------------------+
| ``R``     | 播放参考通道        |
+-----------+---------------------+
| ``N``     | 未使用或未知通道    |
+-----------+---------------------+

**示例：**  
``MMNR`` 表示四个通道，依次为：麦克风通道、麦克风通道、未使用通道、播放参考通道。


配置和初始化
--------------------------------

- 1. 旧的配置初始化方法 AFE_CONFIG_DEFAULT() 已被移除。请使用 ``afe_config_init`` 来初始化配置：

   .. code-block:: c

      afe_config_t *afe_config = afe_config_init("MMNR", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
      afe_config_print(afe_config); // print all configurations

- 2. ESP_AFE_SR_HANDLE 和 ESP_AFE_VC_HANDLE 已被移除。使用 ``esp_afe_handle_from_config`` 来创建实例：

   .. code-block:: c

      esp_afe_sr_iface_t *afe_handle = esp_afe_handle_from_config(afe_config);

.. note::

   AFE v2.0 引入了额外的配置选项。详细信息请参阅 :doc:`AFE <../audio_front_end/README>` 和 :doc:`VAD <../vadnet/README>`。