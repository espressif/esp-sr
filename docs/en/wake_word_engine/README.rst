WakeNet Wake Word Model
=======================

:link_to_translation:`zh_CN:[中文]`

WakeNet is a wake word engine built upon neural network for low-power embedded MCUs. Currently, WakeNet supports up to 5 wake words.

Overview
--------

Please see the flow diagram of WakeNet below:

.. figure:: ../../_static/wakenet_workflow.png
    :alt: overview

.. raw:: html

    <center>

.. raw:: html

    </center>

-  Speech Feature
    We use `MFCC <https://en.wikipedia.org/wiki/Mel-frequency_cepstrum>`__ method to extract the speech spectrum features. The input audio file has a sample rate of 16KHz, mono, and is encoded as signed 16-bit. Each frame has a window width and step size of 30ms.

.. only:: latex

    .. figure:: ../../_static/QR_MFCC.png
        :alt: overview

-  Neural Network
    Now, the neural network structure has been updated to the ninth edition, among which:

    -  WakeNet1, WakeNet2, WakeNet3, WakeNet4, WakeNet6, and WakeNet7 had been out of use.
    -  WakeNet5 only supports ESP32 chip.
    -  WakeNet8 and WakeNet9 only support ESP32-S3 chip, which are built upon the `Dilated Convolution <https://arxiv.org/pdf/1609.03499.pdf>`__ structure.

.. only:: latex

    .. figure:: ../../_static/QR_Dilated_Convolution.png
        :alt: overview

    The network structure of WakeNet5, WakeNet5X2 and WakeNet5X3 is the same, but WakeNetX2 and WakeNetX3 have more parameters than WakeNet5. Please refer to :doc:`Resource Consumption <../benchmark/README>` for details.

-  Keyword Triggering Method:
    For continuous audio stream, we calculate the average recognition results (M) for several frames and generate a smoothing prediction result, to improve the accuracy of keyword triggering. Only when the M value is larger than the set threshold, a triggering command is sent.

Use WakeNet
-----------

-  Select WakeNet model

    To select WakeNet model, please refer to Section :doc:`Flashing Models <../flash_model/README>` .

    To customize wake words, please refer to Section :doc:`Espressif Speech Wake-up Solution Customization Process <ESP_Wake_Words_Customization>`

-  Run WakeNet

    WakeNet is currently included in the :doc:`AFE <../audio_front_end/README>`, which is enabled by default, and returns the detection results through the AFE fetch interface.

    If users do not need WakeNet, please use:

    ::

        afe_config->wakeNet_init = False.

    If users want to enable/disable WakeNet temporarily, please use:

    ::

        afe_handle->disable_wakenet(afe_data)
        afe_handle->enable_wakenet(afe_data)

Resource Occupancy
------------------

For the resource occupancy for this model, see :doc:`Resource Occupancy <../benchmark/README>`.