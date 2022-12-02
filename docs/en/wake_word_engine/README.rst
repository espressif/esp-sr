wakeNet
========

:link_to_translation:`zh_CN:[中文]`

wakeNet, which is a wake word engine built upon neural network, is specially designed for low-power embedded MCUs. Now, the wakeNet model supports up to 5 wake words.

Overview
--------

Please see the flow diagram of wakeNet below:

.. figure:: ../../_static/wakenet_workflow.png
    :alt: overview

.. raw:: html

    <center>

.. raw:: html

    </center>

-  speech features:

    We use the `MFCC <https://en.wikipedia.org/wiki/Mel-frequency_cepstrum>`__ method to extract speech spectrum features. The sampling rate of the input audio file is 16KHz, mono, and the encoding mode is signed 16-bit. The window width and step size of each frame are 30ms.
    We use `MFCC <https://en.wikipedia.org/wiki/Mel-frequency_cepstrum>`__ method to extract the speech spectrum features. The input audio file has a sample rate of 16KHz, mono, and is encoded as signed 16-bit. each frame has a window width and step size of 30ms.

-  Speech Feature:

    The wakeNet uses `MFCC <https://en.wikipedia.org/wiki/Mel-frequency_cepstrum>`__ to obtain the features of the input audio clip (16 KHz, 16 bit, single track). The window width and step width of each frame of the audio clip are both 30 ms.

-  Neural Network:
    
    Now, the neural network structure has been updated to the ninth edition, among which:

    -  wakeNet1,wakeNet2,wakeNet3,wakeNet4,wakeNet6,wakeNet7 had been out of use.
    -  wakeNet5 only support ESP32 chip.
    -  wakeNet8,wakeNet9 only support ESP32S3 chip, which are built upon the `Dilated Convolution <https://arxiv.org/pdf/1609.03499.pdf>`__ structure. 

    .. note:: text
        The network structure of wakeNet5,wakeNet5X2 and wakeNet5X3 is same, but the parameter of wakeNetX2 and wakeNetX3 is more than wakeNet5. Please refer to `Performance Test <#performance-test>`__ for details.

-  Keyword Triggering Method:

    For continuous audio stream, we calculate the average recognition results (M) for several frames and generate a smoothing prediction result, to improve the accuracy of keyword triggering. Only when the M value is larger than the set threshold, a triggering command is sent.

+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| Chip            | ESP32                                 | ESP32S3                                       |
+=================+===========+=============+=============+===========+===========+===========+===========+
| model           | WakeNet 5                             | WakeNet 8             | WakeNet 9             |
|                 +-----------+-------------+-------------+-----------+-----------+-----------+-----------+
|                 | WakeNet 5 | WakeNet 5X2 | WakeNet 5X3 | Q16       | Q8        | Q16       | Q8        |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| Hi,Lexin        | √         | √           | √           |           |           |           | √         |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| nihaoxiaozhi    | √         |             | √           |           |           |           | √         |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| nihaoxiaoxin    |           |             | √           |           |           |           |           |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| xiaoaitongxue   |           |             |             |           |           |           | √         |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| Alexa           |           |             |             | √         |           |           | √         |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| Hi,ESP          |           |             |             |           |           |           | √         |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+
| Customized word |           |             |             |           |           |           | √         |
+-----------------+-----------+-------------+-------------+-----------+-----------+-----------+-----------+

Use wakeNet
-----------

-  How to select the wakeNet model

    Please refer to `flash model  <../flash_model/README.rst>`__.

-  How to run wakeNet

    wakeNet is currently included in the `AFE <../audio_front_end/README.rst>`__, which is running by default, and returns the detect results through the AFE fetch interface.

    If users do not wants to initialize WakeNet, please use:

    ::

        afe_config.wakeNet_init = False.

    If users want to close/open WakeNet temporarily, plese use:

    ::

        afe_handle->disable_wakenet(afe_data)
        afe_handle->enable_wakenet(afe_data)

Performance Test
----------------

Please refer to `Performance Test <../benchmark/README.rst>`__.

Wake Word Customization
-----------------------

For details on how to customize your wake words, please see `Espressif Speech Wake Word Customization Process <ESP_Wake_Words_Customization.rst>`__.
