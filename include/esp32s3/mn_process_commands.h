#pragma once
#include "esp_mn_iface.h"
#include "esp_wn_models.h"
#define SPEECH_COMMANDS_NUM 200

esp_wn_model_t get_first_wake_word(void);
esp_wn_model_t get_second_wake_word(void);
void check_chip_config(void);
char *get_id_name_cn(int i);
char *get_id_name_en(int i);
void reset_speech_commands(model_iface_data_t *model_data, char* command_str, char *err_phrase_id);