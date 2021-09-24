**multinet_g2p.py** is used to convert English phrase into phonemes which can be recognized by multinet．



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


