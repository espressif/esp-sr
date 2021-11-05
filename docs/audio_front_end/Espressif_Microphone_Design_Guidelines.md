# Espressif Microphone Design Guidelines
> This document provides microphone design guidelines and suggestions for the ESP32-S3 series of audio development boards.

###Electrical Performance
1. Type: omnidirectional MEMS microphone
2. Sensitivity
	- Under 1 Pa sound pressure, it should be no less than -38 dBV for analog microphones, and -26 dB for digital microphones. 
	- The tolerance should be controlled within ±2 dB, and within ±1 dB for microphone arrays.
3. Signal-to-noise ratio (SNR)
	- No less than 62 dB. Higher than 64 dB is recommended.
	- Frequency response fluctuates within ±3 dB from 50 to 16 kHz. 
	- PSRR should be larger than 55 dB for MEMS microphones.
		
---
###Structure Design
1. The aperture or width of the microphone hole is recommended to be greater than 1 mm, the pickup pipe should be as short as possible, and the cavity should be as small as possible to ensure that the resonance frequency of the microphone and structural components is above 9 kHz.
2. The depth and diameter of the pickup hole are less than 4:1, and the thickness of the shell is recommended to be 1 mm. If the shell is too thick, the opening area must be increased.
3. The microphone hole must be protected by an anti-dust mesh.
4. Silicone sleeve or foam must be added between the microphone and the device shell for sealing and shockproofing, and an interference fit design is required to ensure the tightness of the microphone.
5. The microphone hole cannot be blocked. The bottom microphone hole needs to be increased in structure to prevent it from being blocked by the desktop.
6. The microphone should be placed far away from the speaker and other objects that can produce noise or vibration, and be isolated and buffered by rubber pads from the speaker sound cavity.

---
###Microphone Array Design
1. Type: omnidirectional MEMS microphone. Use the same models from the same manufacturer for the array. Not recommended mixing different microphones.
2. The sensitivity difference among microphones in the array is within 3 dB.
3. The phase difference among the microphones in the array is controlled within 10°.
4. It is recommended to keep the structural design of each microphone in the array the same to ensure consistency.
5. Two-microphone solution: the distance between the microphones should be 4 ~ 6.5 cm, the axis connecting them should be parallel to the horizontal line, and the center of the two microphones should be horizontally as close as possible to the center of the product.
6. Three-microphone solution: the microphones are equally spaced and distributed in a perfect circle with the angle 120 degrees from each other, and the spacing should be 4 ~ 6.5 cm.

---
###Microphone Structure Tightness
Use plasticine or other materials to seal the microphone pickup hole and compare how much the signals collected by the microphone decrease by before and after the seal. 25 dB is qualified, and 30 dB is recommended. Below are the test procedures.

1. Play white noise at 0.5 meters above the microphone, and keep the volume at the microphone 90 dB. 
2. Use the microphone array to record for more than 10 s, and store it as recording file A.
3. Use plasticine or other materials to block the microphone pickup hole, record for more than 10 s, and store it as recording file B.
4. Compare the frequency spectrum of the two files and make sure that the overall attenuation in the 100 ~ 8 kHz frequency band is more than 25 dB.

---
###Echo Reference Signal Design
1. It is recommended that the echo reference signal be as close to the speaker side as possible, and recover from the DAC post-stage and PA pre-stage.
2. When the speaker volume is at its maximum, the echo reference signal input to the microphone should not have saturation distortion. At the maximum volume, the speaker amplifier output THD is less than 10% at 100 Hz, less than 6% at 200 Hz, and less than 3% above 350 Hz.
3. When the speaker volume is at its maximum, the sound pressure picked up by the microphone does not exceed 102 dB @ 1 kHz.
4. The echo reference signal voltage does not exceed the maximum allowed input voltage of the ADC. If it is too high, an attenuation circuit should be added.
5. A low-pass filter should be added to introduce the reference echo signal from the output of the Class D power amplifier. The cutoff frequency of the filter is recommended to be more than 22 kHz.
6. When the volume is played at the maximum, the recovery signal peak value is -3 to -5 dB.   

---                                                                                                                                          
###Microphone Array Consistency
It is required that the difference between the sampled signals of each microphone is less than 3 dB. Below are the test procedures.

1. Play white noise at 0.5 meters above the microphone, and keep the volume at the microphone 90 dB.
2. Use the microphone array to record for more than 10 s, and check whether the recording amplitude and audio sampling rate of each microphone are consistent.