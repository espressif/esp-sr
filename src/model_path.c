#include "stdio.h"
#include "sdkconfig.h"
#include "esp_spiffs.h"
#include <sys/stat.h>
#include <sys/dirent.h>
#include "string.h"
#include "model_path.h"
#include "esp_wn_models.h"
#include "esp_mn_models.h"

char *get_model_base_path(void)
{
    #if defined CONFIG_MODEL_IN_SDCARD
        return "sdcard";
    #elif defined CONFIG_MODEL_IN_SPIFFS
        return "srmodel";
    #else
        return NULL;
    #endif
}

srmodel_list_t *read_models_form_spiffs(const char *path)
{
    struct dirent *ret;
    DIR *dir;
    dir = opendir(path);
    srmodel_list_t *models = malloc(sizeof(srmodel_list_t*));
    models->num = 0;
    int idx = 0;

    if (dir != NULL)
    {
        while ((ret = readdir(dir)) != NULL)
        { // NULL if reach the end of directory

            if (ret->d_type == DT_DIR) // continue if d_type is not file
                continue;

            int len = strlen(ret->d_name);
            char *suffix = ret->d_name + len - 12;

            if (strcmp(suffix, "_MODEL_INFO_") == 0)
                models->num ++;
        }
        // allocate model names
        models->model_name = malloc(models->num*sizeof(char*));
        for (int i=0; i<models->num; i++) {
            models->model_name[i] = (char*) calloc(MODEL_NAME_MAX_LENGTH, sizeof(char));
        }

        dir = opendir(path);
        while ((ret = readdir(dir)) != NULL)
        { // NULL if reach the end of directory

            if (ret->d_type == DT_DIR) // continue if d_type is not file
                continue;

            int len = strlen(ret->d_name);
            char *suffix = ret->d_name + len - 12;

            if (strcmp(suffix, "_MODEL_INFO_") == 0)
            {
                memcpy(models->model_name[idx], ret->d_name, (len-13)*sizeof(char));
                // models->model_name[idx][len-13] = '\0';
                idx ++;
            }            
        }
    }
    return models;
}


srmodel_list_t* srmodel_spiffs_init(void)
{
    printf("Initializing models from SPIFFS\n");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/srmodel",
        .partition_label = "model",
        .max_files = 5,
        .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount or format filesystem\n");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            printf("Failed to find SPIFFS partition\n");
        } else {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return NULL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info("model", &total, &used);
    if (ret != ESP_OK) {
        printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    } else {
        printf("Partition size: total: %d, used: %d\n", total, used);
    }

    // Read all model from path
    return read_models_form_spiffs(conf.base_path);
}


void srmodel_spiffs_deinit(srmodel_list_t *models)
{
    if (models != NULL) {
        if (models->num>0) {
            for (int i=0; i<models->num; i++) {
                free(models->model_name[i]);
            }
        }
        free(models);
    }

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/srmodel",
        .partition_label = "model",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_vfs_spiffs_unregister(conf.partition_label);
}

srmodel_list_t *srmodel_config_init()
{
    srmodel_list_t *models = malloc(sizeof(srmodel_list_t*));
    models->num = 2;
    models->model_name = malloc(models->num*sizeof(char*));
    for (int i=0; i<models->num; i++) {
        models->model_name[i] = (char*) calloc(MODEL_NAME_MAX_LENGTH, sizeof(char));
    }

    // If wakenet is selected in menuconfig, load the wakenet model
    if (strcmp(WAKENET_MODEL_NAME, "NULL") == 0) {
        models->num --;
        free(models->model_name[models->num]);
    } else {
        strcpy(models->model_name[0], WAKENET_MODEL_NAME);
    }

    // If multinet is selected in menuconfig, load the multinet model
    if (strcmp(MULTINET_MODEL_NAME, "NULL") == 0) {
        models->num --;
        free(models->model_name[models->num]);
    } else {
        strcpy(models->model_name[models->num-1], MULTINET_MODEL_NAME);
    }

    // could not find any avaliable models, return NULL 
    if (models->num == 0) {
        free(models);
        models = NULL;
    }
    
    return models;
}

void srmodel_config_deinit(srmodel_list_t *models)
{
    if (models != NULL) {
        if (models->num>0) {
            for (int i=0; i<models->num; i++) {
                free(models->model_name[i]);
            }
        }
        free(models);
    }
}


srmodel_list_t* esp_srmodel_init()
{
#ifdef CONFIG_IDF_TARGET_ESP32
    return srmodel_config_init();
#else
    return srmodel_spiffs_init();
#endif
}

void esp_srmodel_deinit(srmodel_list_t *models)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    return srmodel_config_deinit(models);
#else
    return srmodel_spiffs_deinit(models);
#endif
}

// repackage strstr function to support needle==NULL
char *_esp_strstr_(const char *haystack, const char *needle)
{
    if (needle == NULL) return haystack;
    else return strstr(haystack, needle);
}

char *esp_srmodel_filter(srmodel_list_t *models, const char *keyword1, const char *keyword2) 
{
    if (models == NULL) return NULL;
    
    // return the first model name including specific keyword
    for (int i=0; i<models->num; i++) {

        if (_esp_strstr_(models->model_name[i], keyword1) != NULL) {
            if (_esp_strstr_(models->model_name[i], keyword2) != NULL)
                return models->model_name[i];
        }
    }

    return NULL;
}

int esp_srmodel_exists(srmodel_list_t *models, char *model_name) {
    if (models == NULL) return -1;
    
    for (int i=0; i<models->num; i++) {
        if (strcmp(models->model_name[i], model_name) == 0) {
            return i;
        }
    }

    return -1;
}