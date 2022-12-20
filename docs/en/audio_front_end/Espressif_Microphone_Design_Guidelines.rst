Espressif Microphone Design Guidelines
=======================================

:link_to_translation:`zh_CN:[中文]`

This document provides microphone design guidelines and suggestions for the {IDF_TARGET_NAME} series of audio development boards.

Microphone Electrical Performance Requirement
---------------------------------------------

- Type: omnidirectional MEMS microphone
- Sensitivity

    - Under 1 Pa sound pressure, the sensitivity should be no less than -38 dBV for analog microphones and -26 dB for digital microphones.
    - Tolerance should be within ±2 dB for microphones. And tolerance for microphone arrays should be within ±1 dB.

- Signal-to-noise ratio (SNR)

    - SNR: No less than 62 dB. Higher than 64 dB is recommended.
    - Frequency response should only fluctuate within ±3 dB from 50 to 16 kHz.
    - PSRR should be larger than 55 dB for microphones.

Microphone Structure Design Suggestion
--------------------------------------

- The aperture or width of the microphone hole is recommended to be greater than 1 mm, the pickup pipe should be as short as possible, and the cavity should be as small as possible. All to ensure that the resonance frequency of the microphone and structural components is above 9 kHz.
- The depth and diameter of the pickup hole are less than 2:1, and the thickness of the shell is recommended to be 1 mm. Increase the hole size of microphone if the shell is too thick.
- The microphone hole must be protected by an anti-dust mesh.
- Silicone sleeve or foam must be added between the microphone and the device shell for sealing and damping, and an interference fit design is required to ensure the leakproofness of the microphone.
- The microphone hole cannot be covered. The microphone in the bottom must keep some clearance from the surfaces such as desktop. Therefore, it's suggested to design some legs for the product to provide such clearance.
- The microphone should be placed far away from the speaker and other objects that can produce noise or vibration, and be isolated and buffered by rubber pads from the speaker sound cavity.

Microphone Array Design Suggestion
----------------------------------

Customers can design two or three microphones in an array:

- Two-microphone solution: the distance between the microphones should be 4 ~ 6.5 cm, the axis connecting them should be parallel to the horizontal line, and the center of the two microphones should be horizontally as close as possible to the center of the product.
- Three-microphone solution: the microphones are equally spaced and distributed in a perfect circle with the angle 120 °C from each other, and the spacing should be 4 ~ 6.5 cm.

There are some limitations when selecting microphones for the same array:

- Type: omnidirectional MEMS microphone. Use the same microphone models from the same manufacturer for the array. It's not recommended to use different microphone models in the same array.
- The sensitivity difference of all the microphones in the same array should be within 3 dB.
- The phase difference of all the microphones in the same array should be within 10°.
- It is recommended to use the same structural design for all the microphones in the same array to ensure consistency.


Microphone Leakproofness Suggestion
-----------------------------------

Use plasticine or similar materials to seal the microphone pickup hole and compare how much the signals collected by the microphone decrease by before and after the seal. 25 dB is qualified, and 30 dB is recommended. Below are the test procedures:

#. Play white noise at 0.5 meters above the microphone, and keep the volume at the microphone 90 dB.
#. Use the microphone array to record for more than 10 s, and store the recording as recording file A.
#. Use plasticine or similar materials to block the microphone pickup hole, record for more than 10 s, and store it as recording file B.
#. Compare the frequency spectrum of the two files and make sure that the overall attenuation in the 100 ~ 8 kHz frequency band is more than 25 dB.

Echo Reference Signal Design Suggestion
---------------------------------------

- It is recommended that the echo reference signal be as close to the speaker side as possible, and recover from the DA post-stage and PA pre-stage.
- When the speaker volume is at its maximum, the echo reference signal input to the microphone should not have saturation distortion. At the maximum volume, the speaker amplifier output Total Harmonic Distortion (THD) is less than 10% at 100 Hz, less than 6% at 200 Hz, and less than 3% above 350 Hz.
- When the speaker volume is at its maximum, the sound pressure picked up by the microphone does not exceed 102 dB @ 1 kHz.
- The echo reference signal voltage does not exceed the maximum allowed input voltage of the ADC. If it is too high, an attenuation circuit should be added.
- A low-pass filter should be added to introduce the reference echo signal from the output of the Class D power amplifier. The cutoff frequency of the filter is recommended to be more than 22 kHz.
- When the volume is played at the maximum, the recovery signal peak value is -3 to -5 dB.

Microphone Array Consistency Verification
-----------------------------------------

It is required that the difference between the sampled signals of each microphone in the same array is less than 3 dB. Below are the test procedures.

#. Play white noise at 0.5 meters above the microphone, and keep the volume at the microphone 90 dB.
#. Use the microphone array to record for more than 10 s, and check whether the recording amplitude and audio sampling rate of each microphone are consistent.