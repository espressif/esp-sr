# MultiNet Introduction

MultiNet is a lightweight model specially designed based on [CRNN](https://arxiv.org/pdf/1703.05390.pdf) and [CTC](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.75.6306&rep=rep1&type=pdf) for the implementation of multi-command recognization with ESP32. Now, up to 100 speech commands, including customized commands, are supported. 

## Overview

MultiNet uses the **MFCC features** of an audio clip as input, and the **phonemes** (Chinese or English) as output. By comparing the output phonemes, the relevant Chinese or English word is identified.

## Commands Recognition Process

1. Add customized commands to the speech command queue.
2. Prepare an audio clip of 30 ms (16 KHz, 6 bit, mono).
3. Input this audio to the MFCC model and get its **MFCC features**.
4. Input the obtained **MFCC features** to MultiNet and get the output **phoneme**.
5. Input the obtained **phoneme** to the Language model and get the output.
6. Compare the output against the existing command words one by one, and output the Command ID of the matching command (if any).

Please see the flow diagram below:

![speech_command-recognition-system](../img/multinet_workflow.png)


## User Guide

### User-defined Command

Currently, users can define their own command words in the code. You can refer to the method of adding command words in `multinet_test.c`, there are already 20 commands pre-stored in `multinet_test.c`.

|Command ID|Command|Command ID|Command|Command ID|Command|Command ID|Command|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|0|打开空调 (Turn on the air conditioner)|5|降低一度 (Decrease by one degree)|10| 关闭节能模式 (Disable power-saving mode)|15| 播放歌曲 (Play a song)
|1|关闭空调 (Turn on the air conditioner)|6|制热模式 (Heating mode)|11| 除湿模式 (Dehumidifying mode)|16| 暂停播放 (Pause playing)
|2|增大风速 (Give me more wind)|7|制冷模式 (Cooling mode)|12| 关闭除湿模式 (Disable dehumidifying mode)|17| 定时一小时 (Set timer to 1 hour)
|3|减少风速 (Give me less wind)|8|送风模式 (Ventilating mode)|13| 打开蓝牙 (Enable the Bluetooth)|18| 打开电灯 (Turn on the light)
|4| 升高一度 (Increase by one degree)|9|节能模式 (Power-saving mode)|10| 关闭节能模式 (Disable power-saving mode)|19| 关闭电灯 (Turn off the light)

 
MultiNet supports user-defined commands. You can add your own commands to MultiNet. Note that the newly added command should obtain its command ID before it can be recognized by MultiNet. 

### Basic Configuration

Define the following two variables before using the command recognition model:

1. Model version
	The model version has been configured in `menuconfig` to facilitate your development. Please configure in `menuconfig` and add the following line in your code:  
	
	`static const esp_mn_iface_t *multinet = &MULTINET_MODEL;`
	
2. Model parameter
	The language supported and the effectiveness of the model is determined by model parameters. Now only commands in Chinese are supported. Please configure the `MULTINET_COEFF` option in `menuconfig` and add the following line in your code to generate the model handle.
	   
	`model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF);`
		
### API Reference

#### Header   
- esp_mn_iface.h
- esp_mn_models.h

#### Function

- `typedef model_iface_data_t* (*esp_mn_iface_op_create_t)(const model_coeff_getter_t *coeff, int sample_length);`  

  **Definition**  
   
 	Easy function type to initialize a model instance with a coefficient.
    
  **Parameter**  
   
 	* coeff: The coefficient for speech commands recognition.  
 	* sample_length Audio length for speech recognition, in ms. The range of sample_length is 0~6000.
    
  **Return**  
 	  
 	Handle to the model data.

- `typedef int (*esp_mn_iface_op_get_samp_chunksize_t)(model_iface_data_t *model);`

   **Definition**  
   
	 Callback function type to fetch the amount of samples that need to be passed to the detection function. Every speech recognition model processes a certain number of samples at the same time. This function can be used to query the amount. Note that the returned amount is in 16-bit samples, not in bytes.
       
  **Parameter**  
   
 	model: The model object to query
  
  **Return**
  
    The amount of samples to feed the detect function



- `typedef int (*esp_mn_iface_op_get_samp_chunknum_t)(model_iface_data_t *model);`

   **Definition**  
   
	 Callback function type to fetch the number of frames recognized by the command word.
       
  **Parameter**  
   
 	model: The model object to query
  
  **Return**
  
    The number of the frames recognized by the command word  
    
- `typedef int (*esp_mn_iface_op_get_samp_rate_t)(model_iface_data_t *model);`

   **Definition**  
   
 	Get the sample rate of the samples to feed to the detection function

  **Parameter**  
  
 	model: The model object to query
 
  **Return**  
  
 	The sample rate, in Hz

- `typedef int (*esp_mn_iface_op_add_speech_commands_t)(model_iface_data_t *model, int command_id, char *phrase_spelling, char *phrase_str);`

   **Definition**  
   
 	Add a command word and set its command ID. 
 
  **Parameters**  
   
 	* model: The model object to query  
 	
 	* command_id: The command ID of this word  

 	* phrase_spelling: The speech command in Chinese spelled using prescribed rules  
 	
    * phrase_str: Auxiliary information of words  
	
   **Return**  
  
 	1: Setting success.
 
  **Note**  
  
    The `phrase_spelling` is the mandarin syllables provided one by one in the form of **one Type A element** and **one Type B element**, which can be seen below: 
 
	 * Type A element: `b bi c ch chu cu d di du f g gu h hu j ji ju k ku l li lu m mi n ni nu p pi q qi qu r ru s sh shu su t ti tu w
x xi xu y yu z zhu zu`
	
	 * Type B element: `a ai an ang ao e ei en eng er i ie in ing iu o ong ou u ue ui un v ve`
 
  For example, the Type A and Type B elements for "tiao" are "ti" and "ao", and the syllable "tiao" should provided to the API as "ti ao". Similarly, the command of "dai kai kong tiao", which means turn on the air conditioner, should be provided to the API as "d ai k ai k ong ti ao".

- `typedef float* (*esp_mn_iface_op_detect_t)(model_iface_data_t *model, int16_t *samples);`  

   **Definition**
 
    Easy function type to initialize a model instance with a coefficient.
    
  **Parameter**  

    coeff: The coefficient for speech commands recognition.  
    
  **Return**  
   
 	* The command id, if a matching command is found
 	* -1, if no matching command is found
 
- `typedef void (*esp_mn_iface_op_destroy_t)(model_iface_data_t *model);`  

   **Definition**  
  
   Destroy a voiceprint recognition model
 
  **Parameters**  
  
  model: Model object to destroy
