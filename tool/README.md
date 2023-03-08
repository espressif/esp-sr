## MultiNet6

#### Step 1. Data preparation

For English, words are used as units. Please prepare a list of commands written in a text file `commands_en.txt` of the following format:

```
# command_id command_sentence
1 TELL ME A JOKE
2 MAKE A COFFEE
```

For Chinese, pinyin are used as units. [multinet_pinyin.py](./multinet_pinyin.py) help to get Pinyin of Chinese. Please prepare a list of commands written in a text file `commands_cn.txt` of the following format:
```
# command_id command_sentence
1 da kai kong tiao
2 guan bi kong tiao
```

#### Step 2. Move created files

1. Move your `commands_en.txt` or `commands_cn.txt` to `/model/multinet_model/fst/`

## MultiNet5
####  1. Install g2p_en, please refer to https://pypi.org/project/g2p-en/ 

```
pip install g2p_en
```

#### 2. Run multinet_g2p.py

```
python multinet_g2p.py -t "hello world,hi ESP;turn on the light;turn off the light"

------
in: hello world,hi ESP;turn on the light;turn off the light
out: hcLb WkLD,hi fST;TkN nN jc LiT;TkN eF jc LiT;
```

#### 3. Add speech commands 

##### 3.1 add speech commands by menuconfig

```
idf.py menuconfig
ESP Speech Recognition -> Add speech commands
```



##### 3.2 add speech commands by reset function

```
// Function definition
// typedef void (*esp_mn_iface_op_reset_t)(model_iface_data_t *model_data, char *command_str, char *err_phrase_id);

// "," is used to split different phrase with same command id
// ";" is used to split different command id
char *new_commands_str="hcLb WkLD,hi fST;TkN nN jc LiT;TkN eF jc LiT;"  //
char err_id[256];
multinet->reset(model_data, new_commands_str, err_id);
// hello world,hi ESP -> commond id=0
// turn on the light -> commond id=1
// turn off the light -> commond id=2
```

