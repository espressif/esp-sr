#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_mn_speech_commands.h"

static char *TAG = "MN_COMMAND";
static esp_mn_node_t *esp_mn_root = NULL;

#define ESP_RETURN_ON_FALSE(a, err_code, log_tag, format, ...) do {                             \
        if (!(a)) {                                                                             \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

esp_err_t esp_mn_commands_alloc(void)
{
    ESP_RETURN_ON_FALSE(NULL == esp_mn_root, ESP_ERR_INVALID_STATE, TAG, "The mn commands already initialized");
    esp_mn_root = esp_mn_node_alloc(NULL);
    return ESP_OK;
}

esp_err_t esp_mn_commands_free(void)
{
    esp_mn_commands_clear();
    esp_mn_node_free(esp_mn_root);
    esp_mn_root = NULL;

    return ESP_OK;
}

int esp_mn_commands_num(void)
{
    esp_mn_node_t * t = esp_mn_root;
    int i = 0;
    while (t->next) {
        t = t->next;
        i++;
    }
    return i;
}

esp_err_t esp_mn_commands_clear(void)
{
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, ESP_ERR_INVALID_STATE, TAG, "The mn commands is not initialized");

    esp_mn_node_t * t = esp_mn_root->next;
    while (t) {
        esp_mn_node_t *cur_node = t;
        t = t->next;
        esp_mn_node_free(cur_node);
    }
    esp_mn_root->next = NULL;

    return ESP_OK;
}

esp_err_t esp_mn_commands_add(int command_id, char *phoneme_string)
{
    esp_mn_node_t *temp = esp_mn_root;
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, ESP_ERR_INVALID_STATE, TAG, "The mn commands is not initialized");
    int last_node_elem_num = esp_mn_commands_num();
    ESP_RETURN_ON_FALSE(ESP_MN_MAX_PHRASE_NUM >= last_node_elem_num, ESP_ERR_INVALID_STATE, TAG, "The number of speech commands phrase must less than 200");

    esp_mn_phrase_t *phrase = esp_mn_phrase_alloc(command_id, phoneme_string);
    if (phrase == NULL) return ESP_ERR_INVALID_STATE;

    esp_mn_node_t *new_node = esp_mn_node_alloc(phrase);
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;

    return ESP_OK;
}

esp_err_t esp_mn_commands_modify(char *old_phoneme_string, char *new_phoneme_string)
{
    esp_mn_node_t *temp = esp_mn_root;
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, ESP_ERR_INVALID_STATE, TAG, "The mn commands is not initialized");

    // search old phoneme_string to get command id
    bool flag = false;
    int command_id;
    while (temp->next) {
        temp = temp->next;
        if (strcmp(old_phoneme_string, temp->phrase->phoneme_string) == 0) {
            command_id = temp->phrase->command_id;
            flag = true;
            break;
        }
    }

    // replace old phrase with new phrase
    if (flag) {
        esp_mn_phrase_t *phrase = esp_mn_phrase_alloc(command_id, new_phoneme_string);
        if (phrase == NULL) return ESP_ERR_INVALID_STATE;
        esp_mn_phrase_free(temp->phrase);
        temp->phrase = phrase;
    } else {
        ESP_LOGE(TAG, "No such speech command: \"%s\"", old_phoneme_string);
        return ESP_ERR_INVALID_STATE;
    }
    
    return ESP_OK;
}

esp_err_t esp_mn_commands_remove(char *phoneme_string)
{
    esp_mn_node_t *temp = esp_mn_root;
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, ESP_ERR_INVALID_STATE, TAG, "The mn commands is not initialized");
    
    // search phoneme_string to get node point
    bool flag = false;
    while (temp->next) {
        if (strcmp(phoneme_string, temp->next->phrase->phoneme_string) == 0) {
            flag = true;
            break;
        }
        temp = temp->next;
    }

    if (flag) {
        esp_mn_node_t *rm_node = temp->next;
        temp->next = temp->next->next;
        esp_mn_node_free(rm_node);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "No such speech command: \"%s\"", phoneme_string);
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_mn_phrase_t *esp_mn_commands_get_from_index(int index)
{
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, NULL, TAG, "The mn commands is not initialized");
    
    // phrase index also is phrase id, which is the depth from this phrase node to root node 
    esp_mn_node_t *temp = esp_mn_root;
    for (int i=-1; i<index; i++) {
        if (temp->next == NULL) return NULL;
        temp = temp->next;
    }
    
    return temp->phrase;
}

esp_mn_phrase_t *esp_mn_commands_get_from_string(const char *phoneme_string)
{
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, NULL, TAG, "The mn commands is not initialized");
    
    // phrase index also is phrase id, which is the depth from this phrase node to root node 
    esp_mn_node_t *temp = esp_mn_root;
    while (temp->next) {
        if (strcmp(phoneme_string, temp->next->phrase->phoneme_string) == 0) {
            return temp->next->phrase;
        }
        temp = temp->next;
    }
    
    return NULL;
}

esp_mn_error_t *esp_mn_commands_update(const esp_mn_iface_t *multinet, model_iface_data_t *model_data)
{
    ESP_RETURN_ON_FALSE(NULL != esp_mn_root, NULL, TAG, "The mn commands is not initialize");
    esp_mn_error_t *error = multinet->set_speech_commands(model_data, esp_mn_root);

    if (error->num == 0) 
        return NULL;

    return error;
}

void esp_mn_commands_print(void)
{
    ESP_LOGI(TAG, "---------------------SPEECH COMMANDS---------------------");
    esp_mn_node_t* temp = esp_mn_root;
    int phrase_id = 0;
    while (temp->next) {
        temp = temp->next;
        ESP_LOGI(TAG, "Command ID%d, phrase ID%d: %s", temp->phrase->command_id, phrase_id, temp->phrase->phoneme_string);
        phrase_id++;
    }
    ESP_LOGI(TAG, "---------------------------------------------------------\n");
}

void *_esp_mn_calloc_(int n, int size)
{
#ifdef ESP_PLATFORM
    return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM);
#else
    return calloc(n, size);
#endif
}

esp_mn_phrase_t *esp_mn_phrase_alloc(int command_id, char *phoneme_string) 
{

    int phoneme_string_len = strlen(phoneme_string);
    if (phoneme_string_len > ESP_MN_MAX_PHRASE_LEN || phoneme_string_len<1) {
        ESP_LOGE(TAG, "The Length of \"%s\" > ESP_MN_MAX_PHRASE_LEN", phoneme_string);
        return NULL;
    }

    esp_mn_phrase_t *phrase = _esp_mn_calloc_(1, sizeof(esp_mn_phrase_t));
    ESP_RETURN_ON_FALSE(NULL != phrase, NULL, TAG, "Fail to alloc mn phrase");
    phrase->command_id = command_id;
    phrase->threshold = 0;
    phrase->wave = NULL;
    memcpy(phrase->phoneme_string, phoneme_string, phoneme_string_len);

    return phrase;
}

void esp_mn_phrase_free(esp_mn_phrase_t *phrase)
{
    if (phrase != NULL) {
        free(phrase);
    }
}

esp_mn_node_t *esp_mn_node_alloc(esp_mn_phrase_t *phrase)
{
    esp_mn_node_t *node = _esp_mn_calloc_(1, sizeof(esp_mn_node_t));
    ESP_RETURN_ON_FALSE(NULL != node, NULL, TAG, "Fail to alloc mn node");
    node->next = NULL;
    node->phrase = phrase;
    return node;
}

void esp_mn_node_free(esp_mn_node_t *node)
{
    if (node != NULL) {
        esp_mn_phrase_free(node->phrase);
        free(node);
    }
}