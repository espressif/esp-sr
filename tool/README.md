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

## MultiNet6


The FST (Finite State Transducer) is used to save a list of commands.

#### Step 1. Data preparation

Requirements:
- python>3.8
- sentencepiece

To create a FST from a list of commands, two files are needed:
- commands.txt: maps a command id to subwords
- tokens.txt: maps subword tokens to it's indices in the bpe model

Assume you have a list of commands written in a text file `commands_list.txt` of the following format:

```
# command_id command_sentence
1 TELL ME A JOKE
2 MAKE A COFFEE
```
**Note**: command ids starts from 1, 0 is reserved in FST.

Run the following command to create the required files, do not change the filenames `commands.txt` and `tokens.txt`. 

```sh
pip install -r requirements.txt

python fst/prepare_for_fst.py \
    --infile commands_list.txt \
    --bpe-model fst/bpe.model \
    --out-command-list commands.txt \
    --out-token-symbols tokens.txt
```

#### Step 2. Move created files

1. Remove `/model/multinet_model/fst/fst.txt` and `/model/multinet_model/fst/fst_reversed.txt` if those files exist. 
2. Move the following files to `/model/multinet_model/fst/`
- commands.txt
- tokens.txt