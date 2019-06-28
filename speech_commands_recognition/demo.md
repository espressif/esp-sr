# Recognizing Speech Commands with ESP32-LyraT-Mini

Currently, Espressif's ESP32-based speech command recognition model [MultiNet](README.md) supports up to 100 Chinese speech commands (We will add supports for English speech commands in the next release of [esp-sr](../README.md)). 

This demo demonstrates the basic process of recognizing Chinese speech commands with ESP32-LyraT-Mini. Please also see a flow diagram below.

![speech-commands-recognition-system](../img/speechs_commands_workflow.png)  

For more information about ESP32-LyraT-Mini, please see [ESP32-LyraT-Mini Getting Started Guide]().
 
# 1. Quick Start

### 1.1 Basic Configuration

Go to `make menuconfig`, and complete the following configuration:

- Basic hardware configuration  

  Navigate to `Audio Media HAL`, and configure the following parameters as instructed.  
  - `Audio hardware board`: select `ESP32-Lyrat Mini V1.1`;  
  - `Audio codec chip`: select `CODEC IS ES8311`;  
  - `use external adc`: select `use es7243`;
  - `Audio DSP chip`: select `No DSP chip`.

  ![speech-commands-recognition-system](../img/specch_commands_config1.png)  

- Basic software configuration

    Navigate to `ESP32 Hotword Detection`, and configure the following parameters as instructed.
  - `Speech recognition audio source`: select `Live microphone on LyraT-board`;
  - `wake word model to use`: select `WakeNet 6 (quantized)`;
  - `wake word name`: select `hilexin (WakeNet6)`;
  - `LVCSR model to use`: select `MultiNet 1 (quantized)`;
  - `langugae`: select `chinese (MultiNet1)`
 
  ![speech-commands-recognition-system](../img/specch_commands_config2.png)  

Then save the configuration and exit.

### 1.2 Compiling and Running

Run `make flash monitor` to compile, flash and run this example, and check the output log:

```
...
I (126) MSC_DSP: CONFIG_CODEC_CHIP_IS_ES8311
wake word number = 1, word1 name = hilexin
-----------awaits to be waken up-----------
```

### 1.3 Waking up the Board

Find the pre-defined wake-up word of the board in the printed log. In this example, the wake-up word is “Hi Lexin" [Ləsɪ:n]. 

Then, say “Hi Lexin" ([Ləsɪ:n]) to wake up the board, which then wakes up and prints the following log:

```
hilexin DETECTED.
-----------LISTENING-----------
```

### 1.4 Recognizing Speech Commands

Then, the board enters the Listening status, waiting for new speech commands.

Currently, the MultiNet model already defined 20 speech commands, which can be seen in [MultiNet](README.md). 

Now, you can give one speech command, for example, "turn on the air conditioner",

* If this command exists in the supported speech command list, the board prints out the command id of this command in its log: 

	```
	-----------LISTENING-----------
	phrase:d a k ai k ong ti ao, prob:0.423639
	command_id:0
	--------------END--------------

	```
* If this command does not exist in the supported speech command list, the board prints an error message of "can not recognize any speech commands" in its log: 


	```
	-----------LISTENING-----------
	can not recognize any speech commands
	--------------END--------------

	```

Also, the board prints `--------------END--------------` when it ends the current recognition cycle and re-enters the Waiting-for-Wakeup status.

**Notices:** 
 
The board can only stay in the Listening status for up to six seconds. After that, it ends the current recognition cycle and re-enters the Waiting-for-wakeup status. Therefore, you must give speech commands in six seconds after the board wakes up.

### 1.5 Adding Customized Speech Commands

Now, the MultiNet model supports 20 pre-defined speech commands, and also allows more customized speech commands by providing users an easy-to-use `add_speech_commands` API. 

Note that you should use mandarin syllables when creating your speech commands, and each syllable should be provided to the API in the form of **one Type A element** and **one Type B element**, which can be seen below:

* Type A element: `b bi c ch chu cu d di du f g gu h hu j ji ju k ku l li lu m mi n ni nu p pi q qi qu r ru s sh shu su t ti tu w
x xi xu y yu z zhu zu`

* Type B element: `a ai an ang ao e ei en eng er i ie in ing iu o ong ou u ue ui un v ve`
      

For example, the Type A and Type B elements for "tiao" are "ti" and "ao", and the syllable "tiao" should provided to the API as "ti ao". Similarly, the command of "dai kai kong tiao", which means turn on the air conditioner, should be provided to the API as "d ai k ai k ong ti ao".
    
For details on how to use API `add_speech_commands`, please click [Here](./README.md).


# 2. Workflow Walkthrough
### 2.1 Hardware Initialization

You don't need any special-purpose boards to run the **WakeNet** and **MultiNet** examples. Currently, Espressif has launched several audio boards and one of them is ESP32-LyraT-Mini, which is what we use in this example.

For details on the initialization of the ESP32-LyraT-Mini board, please see codes in `components/hardware_driver`.
 
If you want to choose other development boards other than ESP32-LyraT-Mini, please go to [esp-adf](https://github.com/espressif/esp-adf), which is Espressif's development framework for building audio applications based on ESP32 products, for more detailed information on hardware drivers.

### 2.2 Wake-up by Keyword

The board enters the Waiting-for-wakeup status after waking up, during which the board will pick up audio data with the on-board microphone, and feed them to the **WakeNet** model frame by frame (30 ms, 16 KHz, 6 bit, mono).

Currently, you cannot customize wake-up word yourself. Therefore, please contact us for such requests.

### 2.3 Recognizing Speech Commands 

During the recognition, the board feeds data frame by frame (30 ms, 16 KHz, 16 bit, mono) to the **MultiNet** model for six seconds. Then, the model compares the speech command received against the pre-defined commands in the list, and return the command id or an error message depending on the recognition result.  

Please see section 1.5 on how to customize your speech command.