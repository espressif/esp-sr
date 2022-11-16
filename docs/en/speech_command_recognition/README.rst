MultiNet Introduction
=====================

:link_to_translation:`zh_CN:[中文]`

MultiNet is a lightweight model designed to realize speech commands
recognition offline on ESP32 series. Now, up to 200 speech commands,
including customized commands, are supported.

    * Support Chinese and English speech commands recognition (esp32s3 is required for English speech commands recognition)
    * Support user-defined commands
    * Support adding / deleting / modifying commands during operation
    * Up to 200 commands are supported
    * It supports single recognition and continuous recognition
    * Lightweight and low resource consumption
    * Low delay, within 500ms
    * Support online Chinese and English model switching (esp32s3 only)
    * The model is partitioned separately to support users to apply OTA

Overview
-----------

The MultiNet input is the audio processed by the audio-front-end
algorithm (AFE), with the format of 16KHz, 16bit and mono. By
recognizing the audio, you can correspond to the corresponding Chinese
characters or English words.

The following table shows the model support of Espressif SoCs:

+---------+-----------+-------------+---------------+-------------+
|  Chip   |   ESP32   |   ESP32S3                                 |
+=========+===========+=============+===============+=============+
| Model   | MultiNet2 | MultiNet4.5 | MultiNet4.5Q8 | MultiNet5Q8 |
+---------+-----------+-------------+---------------+-------------+
| Chinese | √         | √           | √             | √           |
+---------+-----------+-------------+---------------+-------------+
| English |           |             |               | √           |
+---------+-----------+-------------+---------------+-------------+

.. note:: 
    Note: the model ending with Q8 represents the 8bit version of the model, means more lightweight.

Commands Recognition Process
-------------------------------

Please see the flow diagram below:

.. figure:: ../../_static/multinet_workflow.png
    :alt: speech_command-recognition-system

    speech_command-recognition-system

User Guide
-------------

Requirements of speech commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  The recommended length of Chinese is generally 4-6 Chinese characters. Too short leads to high false recognition rate and too long is inconvenient for users to remember
-  The recommended length of English is generally 4-6 words
-  Mixed Chinese and English is not supported in command words
-  Currently, up to 200 command words are supported
-  The command word cannot contain Arabic numerals and special characters
-  Avoid common command words like "hello"
-  The greater the pronunciation difference of each Chinese character / word in the command word, the better the performance

Speech commands customization method
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   * Support a variety of speech commands customization methods
   * Support dynamic addition / deletion / modification of speech commands

Format of Speech commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Speech commands string need to meet specific formats, as follows:

-  Chines

    Chinese speech commands need to use Chinese Pinyin, and there should be a space between the Pinyin spelling of each word.

    In addition, we also provide corresponding tools for users to convert Chinese characters into pinyin. See details:

-  English

    English speech commands need to be represented by specific phonetic symbols. The phonetic symbols of each word are separated by spaces, such as "turn on the light", which needs to be written as "TkN nN jc LiT".

   **We provide specific conversion rules and tools. For details, please refer to the English G2P** `tool <../../tool/multinet_g2p.py>`__.

Set speech commands offline
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Multinet supports flexible speech commands setting methods. No matter which way users set speech commands (code / network / file), they only need to call the corresponding API.

Here we provide two methods of adding speech commands:

-  Use ``menuconfig``

    Users can refer to the example in ESP-Skainet, users can define their own speech commands by ``idf.py menuconfig -> ESP Speech Recognition-> Add Chinese speech commands/Add English speech commands``.

    .. figure:: ../../_static/menuconfig_add_speech_commands.png
        :alt: menuconfig_add_speech_commands

        menuconfig_add_speech_commands

    Please note that a single ``Command ID`` can support multiple phrases. For example, "da kai kong tiao" and "kai kong tiao" have the same meaning, you can write them in the entry corresponding to the same command ID, and separate the adjacent entries with the English character "," without spaces before and after ",".

    Then call the following API:

    ::

        /**
        * @brief Update the speech commands of MultiNet by menuconfig
        *
        * @param multinet            The multinet handle
        *
        * @param model_data          The model object to query
        *
        * @param langugae            The language of MultiNet
        *
        * @return
        *     - ESP_OK                  Success
        *     - ESP_ERR_INVALID_STATE   Fail
        */
        esp_err_t esp_mn_commands_update_from_sdkconfig(esp_mn_iface_t *multinet, const model_iface_data_t *model_data);

-  Add speech commands in the code

    Users can refer to example in ESP-Skainet for this method of adding speech commands.

    In this method, users directly set the speech command words in the code and transmits them to multinet. In the actual development and products, the user can transmit the required speech commands through various possible ways such as network / UART / SPI and change the speech commands.

Set speech commands online
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

MultiNet supports online dynamic addition / deletion / modification of speech commands during operation, without changing models or adjusting parameters. For details, please refer to the example in ESP-Skainet.

Please refer to
`esp_mn_speech_commands <../../src/esp_mn_speech_commands.c>`__ for
details of APIs:

Run speech commands recognition
----------------------------------

Speech commands recognition needs to be run together with the audio front-end (AFE) in esp-sr (WakeNet needs to be enabled in AFE). For the use of AFE, please refer to the document:

`AFE Introduction and Use <../audio_front_end/README_CN.md>`__

MultiNet Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Initialize multinet model

-  Set speech commands

    Please refer #3.

Run MultiNet
~~~~~~~~~~~~~

When users uses AFE and enables wakenet, then can use MultiNet. And
there are the following requirements:

   * The frame length of MultiNet is equal to the AFE fetch frame length
   * The audio format supported is 16KHz, 16bit, mono. The data obtained by AFE fetch is also in this format

-  Get the frame length that needs to be passed into MultiNet

    ::

        int mu_chunksize = multinet->get_samp_chunksize(model_data);

-  MultiNet detect

    We send the data from AFE fetch to the following API:

    ::

        esp_mn_state_t mn_state = multinet->detect(model_data, buff);

   The lengthof ``buff`` is ``mu_chunksize * sizeof(int16_t)``.

The detect result of MultiNet
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Speech commands recognition supports two basic modes:

   * Single recognition
   * Continuous recognition

Speech command recognition must be used with WakeNet. After wake-up, MultiNet detection can be run.

When the MultiNet is running, it will return the recognition status of the current frame in real time ``mn_state``, which is currently divided into the following identification states:

-  ESP_MN_STATE_DETECTING

    This status indicates that the MultiNet is detecting but target
    speech command word has not been recognized.

-  ESP_MN_STATE_DETECTED

    This status indicates that the target speech command has been recognized. At this time, the user can call ``get_results`` interface obtains the identification results.

    ::

      esp_mn_results_t *mn_result = multinet->get_results(model_data);

    The information identifying the result is stored in the return value of the ``get_result`` API, the data type of the return value is as follows:

    ::

        typedef struct{
        esp_mn_state_t state;
         int num;                // The number of phrase in list, num<=5. When num=0, no phrase is recognized.
         int phrase_id[ESP_MN_RESULT_MAX_NUM];      // The list of phrase id.
         float prob[ESP_MN_RESULT_MAX_NUM];         // The list of probability.
        } esp_mn_results_t;

    -  ``state`` is the recognition status of the current frame
    -  ``num`` means the number of recognized commands, ``num`` <= 5, up to 5 possible results are returned
    -  ``phrase_id`` means the Phrase ID of speech commands
    -  ``prob`` meaNS the recognition probability of the recognized entries, which is arranged from large to small

    Users can use ``phrase_id[0]`` and ``prob[0]`` get the recognition result with the highest probability.

    -  ESP_MN_STATE_TIMEOUT

    This status means that the speech commands has not been detected for a long time and will exit automatically Wait for the next wake-up.

* Therefore:
* Exit the speech recognition when the return status is ``ESP_MN_STATE_DETECTED``, it is single recognition mode;
* Exit the speech recognition when the return status is ``ESP_MN_STATE_TIMEOUT``, it is continuous recognition mode;

Other configurations
-----------------------

Threshold setting
~~~~~~~~~~~~~~~~~~~~~

::

    This function is still under development.
