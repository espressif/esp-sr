# wakeNet  

wakeNet, which is a wake word engine built upon neural network, is specially designed for low-power embedded MCUs. Now, the wakeNet model supports up to 5 wake words.

## Overview

Please see the flow diagram of wakeNet below:

<center>
<img src="../img/wakenet_workflow.png" width = "800" />
</center>

- Speech Feature:  
  The wakeNet uses [MFCC](https://en.wikipedia.org/wiki/Mel-frequency_cepstrum) to obtain the features of the input audio clip (16 KHz, 16 bit, single track). The window width and step width of each frame of the audio clip are both 30 ms. 
- Neural Network:  
  Now, the neural network structure has been updated to the sixth edition, among which,  
  - wakeNet1,wakeNet2,wakeNet3,wakeNet4,wakeNet6,wakeNet7 had been out of use.
  - wakeNet5 only support ESP32 chip.
  - wakeNet8,wakeNet9 only support ESP32S3 chip, which are built upon the [Dilated Convolution](https://arxiv.org/pdf/1609.03499.pdf) structure. 
  Note that，The network structure of wakeNet5,wakeNet5X2 and wakeNet5X3 is same, but the parameter of wakeNetX2 and wakeNetX3 is more than wakeNet5. Please refer to [Resource Occupancy](#performance-test) for details.

         
- Keyword Triggering Method  
  For continuous audio stream, we calculate the average recognition results (M) for several frames and generate a smoothing prediction result, to improve the accuracy of keyword triggering. Only when the M value is larger than the set threshold, a triggering command is sent.  

The following table shows the models supported by Espressif SoCs:   
|                          SoCs                              |   wakeNet5   |  wakeNet8   |  wakeNet9   |
| :-------------------------------------------------------- | :----------: | :---------: | :---------: |
|ESP32|Yes|No|No| 
|ESP32S3|No|Yes|Yes| 


## Use wakeNet

- How to select the wakeNet model  
  
  Please refer to  [Flash model 介绍](../flash_model/README.md).
  
- How to run wakeNet

  wakeNet is currently included in the [AFE](../audio_front_end/README.md), which is running by default, and returns the detect results through the AFE fetch interface.
  
  If users wants to close wakeNet, please use:
  
  ```
  afe_config.wakeNet_init = False.
  ```

## Performance Test

Please refer to [Performance_test](../performance_test/README.md).

## Wake Word Customization

For details on how to customize your wake words, please see [Espressif Speech Wake Word Customization Process](ESP_Wake_Words_Customization.md).
