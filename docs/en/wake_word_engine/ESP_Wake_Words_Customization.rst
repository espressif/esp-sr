Espressif Speech Wake-up Solution Customization Process
========================================================

:link_to_translation:`zh_CN:[中文]`

Wake Word Customization Process
-------------------------------

Espressif provides users with the **wake word customization** :

#. Espressif has already opened some wake words for customers' commercial use, such as "HI Leixi", or "Nihao Xiaoxin".

    -  Espressif also plans to provide more wake words that are free for commercial use soon.

#. Offline wake word customization can also be provided by Espressif:

    -  Training corpus provided by customer

        -  Customer must provide at least 20,000 qualified corpus entries. See detailed requirements in Section :ref:`corpus-requirement` .
        -  It usually takes two to three weeks for Espressif to train and optimize the received corpus.
        -  A fee will be charged for training and optimizing the corpus.

    -  Training corpus provided by Espressif

        -  Espressif provides all the corpus required for training.
        -  The time required to collect corpus needs to be discussed separately. After the corpus is ready, it usually takes two to three weeks for Espressif to train and optimize the received corpus.
        -  A fee will be charged for training and optimizing the corpus. A separate fee will be changed for collecting the corpus.

    -  The actual fee and time for your customization depend on the **number of wake words you need** and the **scale of your mass production**. For details, please contact our `sales person <sales@espressif.com>`_ .

#. About Espressif wake word engine WakeNet:

    - Currently, up to 5 wake words are supported by each WakeNet model.
    - A wake word usually consists of 3 to 6 symbols, such as "Hi Leixin", "xiaoaitongxue", "nihaotianmao".
    - More than one WakeNet models can be used together. However, more resource will be consumed when you use more models.
    - For more details, see Section :doc:`WakeNet Wake Word Model <README>` .

.. _corpus-requirement:

Requirements on Corpus
--------------------------

As mentioned above, customers can provide Espressif with training corpus collected themselves or purchased from a third party. However, there are some limitations:

- Audio file format

    - Sample rate: 16 kHz
    - Encoding: 16-bit signed int
    - Channel: mono
    - Format: WAV

#. Sampling requirement

    - Number of samples: more than 500 people, including men and women of all ages and at least 100 children.
    - Sampling environment: a quiet room (< 40 dB). It is recommended to use a professional audio room.
    - Recording device: high-fidelity microphone.
    - How to sample:
        - At 1 meters away from the microphone: each person speaks the wake word out loud for 15 times (5 times in fast speed, 5 times in normal speed, 5 times in slow speed).
        - At 3 meters away from the microphone: each person speaks the wake word out loud for 15 times (5 times in fast speed, 5 times in normal speed, 5 times in slow speed).
    - File name: it is recommended to name the samples according to the age, gender, and quantity of the collected samples, such as ``female_age_fast_id.wav``. Or you can use a separate file to present such information.

Hardware Design and Test
------------------------

The voice wake-up performance heavily depends on the hardware design and cavity structure. Therefore, please pay special attention to the following requirements:

#.  Hardware Design

    - Speaker designs: customers can make their own designs by modifying the reference designs (schematic/PCB) provided by Espressif. Also, Espressif can also review customers' speaker designs to avoid some common design issues.

    - Cavity structure: cavity should be designed by acoustic specialists. Espressif does not provide ID design reference. Customers can refer to other mainstream speaker cavity designs on the market, such as Tmall Genie, Xiaodu Smart Speaker, and Google Smart Speaker, etc.

#. Customers can perform the following tests to verify the hardware designs. Note that it's suggested to perform the following tests in a professional audio room. Customers can adjust the actual tests based on their actual testing environment.

    - Recording test to verify the gain and distortion of mic and codec

        - Play the sample (90 dB, 0.1 meter away from the mic), and adjust the gain to ensure that the recording is not saturated.
        - Use a sweep file of 0~20 KHz, and start recording using the sampling rate of 16 KHz. The recording should not have obvious frequency aliasing.
        - Record 100 samples, and feed these samples to open cloud voice recognition API. A certain recognition rate must be reached.

    - Playback test to verify the distortion of power amplifier (PA) and speaker

        - Test PA power @ 1% Total Harmonic Distortion (THD)

    - Speech algorithms test to verify the AEC, BFM and NS models

        - Adjust the delays of the reference signals based on the different requirements of different AEC algorithms.
        - Test the product based on the actual use scenario. For example, play ``85DB-90DB Dreamer.wav`` (a song) and record.
        - Analyze the processed signals to evaluate the performance of AEC, BFM, NS, etc.

    -  DSP performance test to identify the correct DSP parameter and minimize the nonlinear distortion in the DSP algorithm

        -  Noise Suppression
        -  Acoustic Echo Cancellation
        -  Speech Enhancement

#. Customers can also **send** 1 or 2 pieces of hardware to Espressif and ask us to optimize the product for better wakeup performance.
