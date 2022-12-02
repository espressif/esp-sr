:link_to_translation:`zh_CN:[中文]`

This is LEXIN `ESP-SR <https://github.com/espressif/esp-sr>` This document will introduce LEXIN's AI voice solution based on ESP32 series chip. From front-end audio processing, to voice command word recognition, from hardware design suggestions, to performance testing methods, it is a comprehensive introduction to Loxin's systematic work on AI speech, and provides a strong reference for users to build AIoT applications on Loxin ESP32 series chips and development boards.

Lexin AFE algorithm has passed the Software Audio Front-End certification for Amazon Alexa built-in devices. The built-in wake-up module in AFE algorithm can realize local voice wake-up function and support wake-up word customization. Lexin's voice command word recognition model can support up to 200 English and Chinese command words, and the command words can be modified during operation, bringing great flexibility to the application.

Based on years of hardware design and development experience, Loxin can provide voice development board review service for customers, and will be happy to test and tune the development board for customers to show the optimal performance of the algorithm. Customers can also conduct in-depth evaluation of the development board and the whole product according to the test methods and self-test results provided by Loxin.

.. only:: html

    **This document only contains the ESP-AT usage** for the chip. For other chips, please select your target chip from the drop-down menu at the top left of the page.

.. only:: latex

    **This document contains ESP-AT usage** for the chip only.

.. toctree::
    :hidden:

    AFE acoustic front-end algorithm <audio_front_end/README>
    Wake word model <wake_word_engine/README>
    Customized wake words <wake_word_engine/ESP_Wake_Words_Customization>
    Speech commands <speech_command_recognition/README>
    Model loading method <flash_model/README>
    Microphone Design Guidelines <audio_front_end/Espressif_Microphone_Design_Guidelines>
    Test Reports <test_report/README>
    Performance Testing <benchmark/README>

Translated with www.DeepL.com/Translator (free version)