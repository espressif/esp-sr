# WakeNet  

WakeNet, which is a wake word engine built upon neural network, is specially designed for low-power embedded MCUs. Now, the WakeNet model supports up to 5 wake words.

## Overview

Please see the flow diagram of WakeNet below:

<center>
<img src="../img/wakenet_workflow.png" width = "800" />
</center>

- Speech Feature:  
  The WakeNet uses [MFCC](https://en.wikipedia.org/wiki/Mel-frequency_cepstrum) to obtain the features of the input audio clip (16 KHz, 16 bit, single track). The window width and step width of each frame of the audio clip are both 30 ms. 
      
- Neural Network:  
  Now, the neural network structure has been updated to the sixth edition, among which,  
  - WakeNet1 and WakeNet2 had been out of use.
  - WakeNet3 and WakeNet4 had been out of use.
  - WakeNet5(WakeNet5X2,WakeNetX3), WakeNet7, WakeNet8 are built upon the [Dilated Convolution](https://arxiv.org/pdf/1609.03499.pdf) structure. 
  Note that，The network structure of WakeNet5,WakeNet5X2 and WakeNet5X3 is same, but the parameter of WakeNetX2 and WakeNetX3 is more than WakeNet5. Please refer to [Resource Occupancy](#performance-test) for details.

         
- Keyword Triggering Method  
  For continuous audio stream, we calculate the average recognition results (M) for several frames and generate a smoothing prediction result, to improve the accuracy of keyword triggering. Only when the M value is larger than the set threshold, a triggering command is sent.  

The following table shows the model support of Espressif SoCs:

![wakent_model](../img/WakeNet_model.png)

## Use WakeNet

- How to select the WakeNet model  
  
  Please refer to  [Flash model 介绍](../flash_model/README.md).
  
- How to run WakeNet

  WakeNet is currently included in the [AFE](../audio_front_end/README.md), which is running by default, and returns the detect results through the AFE fetch interface.
  
  If users wants to close WakeNet, please use:
  
  ```
  afe_config.wakenet_init = False.
  ```

## Performance Test

Please refer to [Performance_test](../performance_test/README.md).

## Wake Word Customization

For details on how to customize your wake words, please see [Espressif Speech Wake Word Customization Process](ESP_Wake_Words_Customization.md).
