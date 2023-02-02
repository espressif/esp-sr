Command Word
============

:link_to_translation:`zh_CN:[中文]`

MultiNet Command Word Recognition Model
---------------------------------------

MultiNet is a lightweight model designed to recognize multiple speech command words offline based on {IDF_TARGET_NAME}. Currently, up to 200 speech commands, including customized commands, are supported.

.. list::

    :esp32s3: - Support Chinese and English speech commands recognition
    :esp32: - Support Chinese speech commands recognition
    - Support user-defined commands
    - Support adding / deleting / modifying commands during operation
    - Up to 200 commands are supported
    - It supports single recognition and continuous recognition
    - Lightweight and low resource consumption
    - Low delay, within 500ms
    :esp32s3: - Support online Chinese and English model switching (esp32s3 only)
    - The model is partitioned separately to support users to apply OTA

The MultiNet input is the audio processed by the audio-front-end algorithm (AFE), with the format of 16 KHz, 16 bit and mono. By recognizing the audio signals, speech commands can be recognized.

The following table shows the models supported by Espressif SoCs:

+---------+-----------+-------------+---------------+-------------+
|  Chip   |   ESP32   |   ESP32S3                                 |
+=========+===========+=============+===============+=============+
| Model   | MultiNet2 | MultiNet4.5 | MultiNet4.5Q8 | MultiNet5Q8 |
+---------+-----------+-------------+---------------+-------------+
| Chinese | √         | √           | √             | √           |
+---------+-----------+-------------+---------------+-------------+
| English |           |             |               | √           |
+---------+-----------+-------------+---------------+-------------+

For details on flash models, see Section :doc:`Flashing Models <../flash_model/README>` .

.. note::
    Models ending with Q8 represents the 8 bit version of the model, which is more lightweight.

Commands Recognition Process
----------------------------

Please see the flow diagram for commands recognition below:

.. figure:: ../../_static/multinet_workflow.png
    :alt: speech_command-recognition-system

    speech_command-recognition-system

.. _command-requirements:

Requirements of Speech Commands
-------------------------------

Currently, MultiNet supports up to **200** commands. There are some limitation when designing speech commands:

-  Chinese

    Use Pinyin for Chinese speech commands, and add a space in between. For example, the Chinese speech command for turning on the air conditioner is "da kai kong tiao"; the Chinese speech command for turning on the green light is "da kai lv se deng".

-  English

    Use phonetic symbols for English speech commands, and add a space in between. For example, the English speech command for turnning on the light is "TkN nN jc LiT". Users can use the tool provided by us to do the convention. To find this tool, go to :project_file:`tool/multinet_g2p.py` .


Suggestions on Customizing Speech Commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When customizing speech command words, please pay attention to the following suggestions:

.. list::

    -  The recommended length of Chinese speech commands is generally 4-6 Chinese characters. Too short leads to high false recognition rate and too long is inconvenient for users to remember
    :esp32s3: -  The recommended length of English speech commands is generally 4-6 words
    -  Mixed Chinese and English is not supported in command words
    -  The command word cannot contain Arabic numerals and special characters
    -  Avoid common command words like "hello"
    -  The greater the pronunciation difference of each Chinese character / word in the command words, the better the performance

Speech Commands Customization Methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Multinet supports flexible methods to customize speech commands. Users can do it either online or offline and can also add/delete/modify speech commands dynamically.

.. only:: latex

    .. figure:: ../../_static/QR_multinet_g2p.png
        :alt: menuconfig_add_speech_commands

Customize Speech Commands Offline
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are two methods for users to customize speech commands offline:

-  Via ``menuconfig``

    1. Navigate to ``idf.py menuconfig`` > ``ESP Speech Recognition`` > ``Add Chinese speech commands/Add English speech commands`` to add speech commands. For details, please refer to the example in ESP-Skainet.

    .. figure:: ../../_static/menuconfig_add_speech_commands.png
        :alt: menuconfig_add_speech_commands

        menuconfig_add_speech_commands

    Please note that a single ``Command ID`` can correspond to more than one commands. For example, "da kai kong tiao" and "kai kong tiao" have the same meaning. Therefore, users can assign the same command id to these two commands and separate them with "," (no space required before and after).

    1. Call the following API:

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

-  Via modifying code

    Users directly customize the speech commands in the code and pass these commands to the MultiNet. In the actual user scenarios, users can pass these commands via various interfaces including network / UART / SPI. For details, see the example described in ESP-Skainet.

Customize speech commands online
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

MultiNet allows users to add/delete/modify speech commands dynamically during the operation, without the need to change models or modifying parameters. For details, see the example described in ESP-Skainet.

For detailed description of APIs, please refer to :project_file:`src/esp_mn_speech_commands.c` .

Use MultiNet
------------

MultiNet speech commands recognition must be used together with audio front-end (AFE) in ESP-SR (What's more, AFE must be used together with WakeNet). For details, see Section :doc:`AFE Introduction and Use <../audio_front_end/README>` .

After configuring AFE, users can follow the steps below to configure and run MultiNet.

Initialize MultiNet
~~~~~~~~~~~~~~~~~~~

-  Load and initialize MultiNet. For details, see Section :doc:`flash_model <../flash_model/README>`

-  Customize speech commands. For details, see Section :ref:`command-requirements`

Run MultiNet
~~~~~~~~~~~~~

Users can start MultiNet after enabling AFE and WakeNet, but must pay attention to the following limitations:

* The frame length of MultiNet must be equal to the AFE fetch frame length
* The audio format supported is 16 KHz, 16 bit, mono. The data obtained by AFE fetch is also in this format

-  Get the length of frame that needs to pass to MultiNet

    ::

        int mu_chunksize = multinet->get_samp_chunksize(model_data);

    ``mu_chunksize`` describes the ``short`` of each frame passed to MultiNet. This size is exactly the same as the number of data points per frame obtained in AFE.

-  Start the speech recognition

    We send the data from AFE ``fetch`` to the following API:

    ::

        esp_mn_state_t mn_state = multinet->detect(model_data, buff);

   The length of ``buff`` is ``mu_chunksize * sizeof(int16_t)``.

MultiNet Output
~~~~~~~~~~~~~~~

Speech commands recognition supports two basic modes:

   * Single recognition
   * Continuous recognition

Speech command recognition must be used with WakeNet. After wake-up, MultiNet detection can start.

Afer running, MultiNet returns the recognition output of the current frame in real time ``mn_state``, which is currently divided into the following identification states:

-  ESP_MN_STATE_DETECTING

    Indicates that the MultiNet is detecting but the target speech command word has not been recognized.

-  ESP_MN_STATE_DETECTED

    Indicates that the target speech command has been recognized. At this time, the user can call ``get_results`` interface to obtain the recognition results.

    ::

      esp_mn_results_t *mn_result = multinet->get_results(model_data);

    The recognition result is stored in the return value of the ``get_result`` API in the following format:

    ::

        typedef struct{
        esp_mn_state_t state;
         int num;                // The number of phrase in list, num<=5. When num=0, no phrase is recognized.
         int phrase_id[ESP_MN_RESULT_MAX_NUM];      // The list of phrase id.
         float prob[ESP_MN_RESULT_MAX_NUM];         // The list of probability.
        } esp_mn_results_t;

    where,

    -  ``state`` is the recognition status of the current frame
    -  ``num`` means the number of recognized commands, ``num`` <= 5, up to 5 possible results are returned
    -  ``phrase_id`` means the Phrase ID of speech commands
    -  ``prob`` means the recognition probability of the recognized entries, which is arranged from large to small

    Users can use ``phrase_id[0]`` and ``prob[0]`` get the recognition result with the highest probability.

    -  ESP_MN_STATE_TIMEOUT

    Indicates the speech commands has not been detected for a long time and will exit automatically and wait to be waked up again.

Therefore:
* Single recognition mode: exit the speech recognition when the return status is ``ESP_MN_STATE_DETECTED``
* Continuous recognition: exit the speech recognition when the return status is ``ESP_MN_STATE_TIMEOUT``

Resource Occupancy
------------------

For the resource occupancy for this model, see :doc:`Resource Occupancy <../benchmark/README>`.


Other configurations
-----------------------

Threshold setting
~~~~~~~~~~~~~~~~~

::

    More functions are still under development.
