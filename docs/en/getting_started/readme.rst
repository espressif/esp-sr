Getting Started
================

:link_to_translation:`zh_CN:[中文]`

Espressif `ESP-SR <https://github.com/espressif/esp-sr>`__ helps you build AI voice solution based on ESP32 or ESP32-S3 chips. This document introduces the algorithms and models in ESP-SR via some simple examples.

Overview
--------

ESP-SR includes the following modules:

* :doc:`Audio Front-end AFE <../audio_front_end/README>`
* :doc:`Wake Word Engine WakeNet <../wake_word_engine/README>`
* :doc:`Speech Command Word Recognition MultiNet <../speech_command_recognition/README>`
* :doc:`Speech Synthesis (only supports Chinese language) <../speech_synthesis/readme>`

What You Need
-------------

Hardware
~~~~~~~~

.. list::

   :esp32s3: - an audio development board. Recommendation: ESP32-S3-Korvo-1 or ESP32-S3-Korvo-2
   :esp32: - an audio development board. Recommendation: ESP32-Korvo
   - USB 2.0 cable (USB A / micro USB B)
   - PC (Linux)

.. note::
   Some development boards currently have the Type C interface. Make sure you use the proper cable to connect the board!

Software
~~~~~~~~

* Download `ESP-SKAINET <https://github.com/espressif/esp-skainet>`__, which also downloads ESP-SR as a component.
* Install the ESP-IDF version recommended in ESP-SKAINET. For detailed steps, please see Section `Getting Started <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html>`__ in `ESP-IDF Programming Guide <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/index.html>`__.


Compile an Example
------------------

* Navigate to `ESP-SKAINET/examples/en_speech_commands_recognition <https://github.com/espressif/esp-skainet/tree/master/examples/en_speech_commands_recognition>`__ .
* Compile and run an example following the instructions.
* The example only supports commands in English. Users can wake up the chip by using wake word "Hi ESP". Note that the chip stops listening to commands if the users wake up the chip and do not give any commands for some time. In this case, just wake up the chip again by saying the wake word.