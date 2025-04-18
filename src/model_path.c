#include "model_path.h"
#include "esp_wn_models.h"
#include "stdio.h"
#include "string.h"
#include <dirent.h>
#include <sys/stat.h>
// #ifndef CONFIG_IDF_TARGET_ESP32P4
// #include "esp_mn_models.h"
// #endif

#ifdef ESP_PLATFORM
#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "sdkconfig.h"
#include <sys/dirent.h>
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "spi_flash_mmap.h"
#endif
#endif

static char *TAG = "MODEL_LOADER";
static char *SRMODE_BASE_PATH = "/srmodel";
static srmodel_list_t *static_srmodels = NULL;

void set_model_base_path(const char *base_path)
{
    SRMODE_BASE_PATH = (char *)base_path;
}

char *get_model_info(char *data, int size)
{
    char *model_info = NULL;
    // Prase
    // if the line starts with '#', the line is a comment
    // else the line is model information
    while (size > 0) {
        if (*data == '#') {
            while (*data != '\n' && size > 1) {
                data++;
                size--;
            }
            data++;
            size--;
            continue;
        } else if (data != NULL && size > 0) {
            model_info = (char *)malloc((size + 1) * sizeof(char));
            memcpy(model_info, data, size);
            if (model_info[size - 1] == '\n') {
                model_info[size - 1] = '\0';
            }
            model_info[size] = '\0';
            break;
        }
    }

    return model_info;
}

char *get_wake_words_from_info(char *model_info)
{
    if (model_info == NULL)
        return NULL;

    int info_len = strlen(model_info);
    char *temp = (char *)malloc(info_len + 1);
    memcpy(temp, model_info, info_len);
    temp[info_len] = '\0';
    char *token = strtok(temp, "_");
    char *wake_words = NULL;
    int word_len = 0;
    int i = 1;
    while (token != NULL) {
        if (i % 4 == 3) {
            // find all valid wake word token
            if (wake_words == NULL) {
                word_len = strlen(token) + 1;
                wake_words = (char *)malloc(word_len);
                memcpy(wake_words, token, word_len - 1);
                wake_words[word_len - 1] = '\0';
            } else {
                word_len += strlen(token) + 1;
                wake_words = (char *)realloc(wake_words, word_len);
                strcat(wake_words, ";");
                strcat(wake_words, token);
                wake_words[word_len - 1] = '\0';
            }
        }
        token = strtok(NULL, "_");
        i++;
    }

    free(temp);
    return wake_words;
}

static srmodel_list_t *srmodel_list_alloc(void)
{
    srmodel_list_t *models = (srmodel_list_t *)malloc(sizeof(srmodel_list_t));
    models->model_data = NULL;
    models->model_name = NULL;
    models->model_info = NULL;
    models->num = 0;
#ifdef ESP_PLATFORM
    models->partition = NULL;
#endif
    models->mmap_handle = NULL;

    return models;
}

#ifdef ESP_PLATFORM
srmodel_list_t *read_models_form_spiffs(esp_vfs_spiffs_conf_t *conf)
{
    if (static_srmodels == NULL) {
        static_srmodels = srmodel_list_alloc();
    } else {
        return static_srmodels;
    }

    srmodel_list_t *models = static_srmodels;

    struct dirent *ret;
    DIR *dir = NULL;
    dir = opendir(conf->base_path);
    int model_num = 0;
    int idx = 0;

    if (dir != NULL) {
        // get the number of models
        while ((ret = readdir(dir)) != NULL) {
            // NULL if reach the end of directory

            if (ret->d_type == DT_DIR) { // continue if d_type is not file
                continue;
            }

            int len = strlen(ret->d_name);
            char *suffix = ret->d_name + len - 12;

            if (strcmp(suffix, "_MODEL_INFO_") == 0) {
                model_num++;
            }
        }

        // allocate model names
        if (model_num == 0) {
            return models;
        } else {
            models->num = model_num;
            models->model_name = malloc(models->num * sizeof(char *));
            for (int i = 0; i < models->num; i++) {
                models->model_name[i] = (char *)calloc(MODEL_NAME_MAX_LENGTH, sizeof(char));
            }
        }

        // read & save model names
        closedir(dir);
        dir = opendir(conf->base_path);
        while ((ret = readdir(dir)) != NULL) {
            // NULL if reach the end of directory

            if (ret->d_type == DT_DIR) { // continue if d_type is not file
                continue;
            }

            int len = strlen(ret->d_name);
            char *suffix = ret->d_name + len - 12;

            if (strcmp(suffix, "_MODEL_INFO_") == 0) {
                memcpy(models->model_name[idx], ret->d_name, (len - 13) * sizeof(char));
                // models->model_name[idx][len-13] = '\0';
                idx++;
            }
        }
        closedir(dir);
        dir = NULL;
    }
    return models;
}

srmodel_list_t *srmodel_spiffs_init(const esp_partition_t *part)
{
    ESP_LOGI(TAG, "\nInitializing models from SPIFFS, partition label: %s\n", part->label);

    esp_vfs_spiffs_conf_t conf = {
        .base_path = SRMODE_BASE_PATH, .partition_label = part->label, .max_files = 5, .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem\n");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition\n");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return NULL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(part->label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d\n", total, used);
    }

    // Read all model from path
    srmodel_list_t *models = read_models_form_spiffs(&conf);
    models->partition = (esp_partition_t *)part;
    return models;
}

void srmodel_spiffs_deinit(srmodel_list_t *models)
{
    if (models->partition != NULL) {
        esp_err_t ret = esp_vfs_spiffs_unregister(models->partition->label);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Already unregistered\n");
        } else {
            ESP_LOGI(TAG, "%s has been unregistered\n", models->partition->label);
        }
    }

    if (models != NULL) {
        if (models->num > 0) {
            for (int i = 0; i < models->num; i++) {
                free(models->model_name[i]);
            }
            free(models->model_name);
        }
        free(models);
    }
    models = NULL;
}

static uint32_t read_int32(char *data)
{
    uint32_t value = 0;
    value |= data[0] << 0;
    value |= data[1] << 8;
    value |= data[2] << 16;
    value |= data[3] << 24;
    return value;
}

srmodel_list_t *srmodel_load(const void *root)
{
    if (static_srmodels == NULL) {
        static_srmodels = srmodel_list_alloc();
        printf("create static models");
    }

    srmodel_list_t *models = static_srmodels;
    char *start = (char *)root;
    char *data = (char *)root;
    int str_len = SRMODEL_STRING_LENGTH;
    int int_len = 4;
    // read model number
    models->num = read_int32(data);
    data += int_len;
    models->model_data = (srmodel_data_t **)malloc(sizeof(srmodel_data_t *) * models->num);
    models->model_name = (char **)malloc(sizeof(char *) * models->num);
    models->model_info = (char **)malloc(sizeof(char *) * models->num);

    for (int i = 0; i < models->num; i++) {
        srmodel_data_t *model_data = (srmodel_data_t *)malloc(sizeof(srmodel_data_t));
        models->model_info[i] = NULL;
        // read model name
        models->model_name[i] = (char *)malloc((strlen(data) + 1) * sizeof(char));
        strcpy(models->model_name[i], data);
        data += str_len;
        // read model number
        int file_num = read_int32(data);
        model_data->num = file_num;
        data += int_len;
        model_data->files = (char **)malloc(sizeof(char *) * file_num);
        model_data->data = (char **)malloc(sizeof(void *) * file_num);
        model_data->sizes = (int *)malloc(sizeof(int) * file_num);

        for (int j = 0; j < file_num; j++) {
            // read file name
            model_data->files[j] = data;
            data += str_len;
            // read file start index
            int index = read_int32(data);
            data += int_len;
            model_data->data[j] = start + index;
            // read file size
            int size = read_int32(data);
            data += int_len;
            model_data->sizes[j] = size;

            // read model info
            if (strcmp(model_data->files[j], "_MODEL_INFO_") == 0) {
                models->model_info[i] = get_model_info(model_data->data[j], model_data->sizes[j]);
            }
        }
        models->model_data[i] = model_data;
    }
    ESP_LOGI(TAG, "Successfully load srmodels");
    set_model_base_path(NULL);
    return models;
}

srmodel_list_t *srmodel_mmap_init(const esp_partition_t *partition)
{
    if (static_srmodels == NULL) {
        static_srmodels = srmodel_list_alloc();
    } else {
        return static_srmodels;
    }

    srmodel_list_t *models = static_srmodels;
    const void *root;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    int free_pages = spi_flash_mmap_get_free_pages(ESP_PARTITION_MMAP_DATA);
    uint32_t storage_size = free_pages * 64 * 1024; // Byte
    ESP_LOGI(TAG, "The storage free size is %ld KB", storage_size / 1024);
    ESP_LOGI(TAG, "The partition size is %ld KB", partition->size / 1024);
    if (storage_size < partition->size) {
        ESP_LOGE(TAG, "The storage free size of this board is less than %s partition required size", partition->label);
    }
    models->mmap_handle = (esp_partition_mmap_handle_t *)malloc(sizeof(esp_partition_mmap_handle_t));
    ESP_ERROR_CHECK(
        esp_partition_mmap(partition, 0, partition->size, ESP_PARTITION_MMAP_DATA, &root, models->mmap_handle));
#else
    int free_pages = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
    uint32_t storage_size = free_pages * 64 * 1024; // Byte
    ESP_LOGI(TAG, "The storage free size is %d KB", storage_size / 1024);
    ESP_LOGI(TAG, "The partition size is %d KB", partition->size / 1024);
    if (storage_size < partition->size) {
        ESP_LOGE(TAG, "The storage free size of board is less than %s partition size", partition->label);
    }
    models->mmap_handle = (spi_flash_mmap_handle_t *)malloc(sizeof(spi_flash_mmap_handle_t));
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &root, models->mmap_handle));
#endif

    models->partition = (esp_partition_t *)partition;
    srmodel_load(root);
    return models;
}

void srmodel_mmap_deinit(srmodel_list_t *models)
{
    if (models != NULL) {
        if (models->mmap_handle) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
            esp_partition_munmap(*(esp_partition_mmap_handle_t *)models->mmap_handle); // support esp-idf v5
#else
            spi_flash_munmap(*(spi_flash_mmap_handle_t *)models->mmap_handle); // support esp-idf v4
#endif
        }

        if (models->num > 0) {
            for (int i = 0; i < models->num; i++) {
                free(models->model_data[i]->files);
                free(models->model_data[i]->data);
                free(models->model_data[i]->sizes);
                free(models->model_data[i]);
                free(models->model_name[i]);
                if (models->model_info[i] != NULL)
                    free(models->model_info[i]);
            }
        }
        free(models->model_data);
        free(models->model_name);
        free(models->mmap_handle);
        free(models->model_info);
        free(models);
    }
    models = NULL;
    static_srmodels = NULL;
}

#endif

char *get_model_base_path(void)
{
    return SRMODE_BASE_PATH;
}

srmodel_list_t *get_static_srmodels(void)
{
    return static_srmodels;
}

static char *join_path(const char *dirname, const char *filename)
{
    if (dirname == NULL || filename == NULL) {
        return NULL;
    }
    int dirname_len = strlen(dirname);
    int filename_len = strlen(filename);
    int len = filename_len + dirname_len + 2;
    char *path = calloc(len, sizeof(char));
    memcpy(path, dirname, dirname_len);
    if (dirname[dirname_len - 1] == '/') {
        memcpy(path + dirname_len, filename, filename_len);
    } else {
        path[dirname_len] = '/';
        memcpy(path + dirname_len + 1, filename, filename_len);
    }
    return path;
}

// read srmodel from sdcard
srmodel_list_t *srmodel_sdcard_init(const char *base_path)
{
    printf("Initializing models from path: %s\n", base_path);
    if (static_srmodels == NULL) {
        static_srmodels = srmodel_list_alloc();
    } else {
        return static_srmodels;
    }

    srmodel_list_t *models = static_srmodels;
    set_model_base_path(base_path);

    struct dirent *ret;
    DIR *dir = NULL;
    dir = opendir(base_path);
    int model_num = 0;
    int idx = 0;
    FILE *fp;

    if (dir != NULL) {
        // get the number of models
        while ((ret = readdir(dir)) != NULL) {
            // NULL if reach the end of directory

            if (ret->d_type == DT_DIR) { // if d_type is directory
                char *sub_path = join_path(base_path, ret->d_name);
                char *info_file = join_path(sub_path, "_MODEL_INFO_");
                fp = fopen(info_file, "r");
                if (fp != NULL) {
                    model_num++; // If _MODLE_INFO_ file exists, model_num ++
                }
                printf("%s -> %s\n", sub_path, info_file);
                fclose(fp);
                free(sub_path);
                free(info_file);
            }
        }
        closedir(dir);

        // allocate srmodel_list_t struct
        if (model_num == 0) {
            return models;
        } else {
            models->num = model_num;
#ifdef ESP_PLATFORM
            models->partition = NULL;
#endif
            models->model_name = malloc(models->num * sizeof(char *));
            models->model_info = malloc(models->num * sizeof(char *));
            for (int i = 0; i < models->num; i++) {
                models->model_name[i] = (char *)calloc(MODEL_NAME_MAX_LENGTH, sizeof(char));
            }
        }

        // read & save model names
        dir = opendir(base_path);
        while ((ret = readdir(dir)) != NULL) {
            // NULL if reach the end of directory

            if (ret->d_type == DT_DIR) { // if d_type is directory
                char *sub_path = join_path(base_path, ret->d_name);
                char *info_file = join_path(sub_path, "_MODEL_INFO_");
                fp = fopen(info_file, "r");
                if (fp != NULL) {
                    memcpy(models->model_name[idx], ret->d_name, strlen(ret->d_name));
                    fseek(fp, 0L, SEEK_END);
                    int file_size = ftell(fp);
                    fseek(fp, 0, SEEK_SET);
                    char *data = (char *)malloc(file_size);
                    fread(data, file_size, 1, fp);
                    models->model_info[idx] = get_model_info(data, file_size);
                    free(data);
                    idx++;
                }
                fclose(fp);
                free(sub_path);
                free(info_file);
            }
        }
        closedir(dir);
        dir = NULL;
    }
    return models;
}

void srmodel_sdcard_deinit(srmodel_list_t *models)
{
    if (models != NULL) {
        if (models->num > 0) {
            for (int i = 0; i < models->num; i++) {
                free(models->model_name[i]);
                free(models->model_info[i]);
            }
        }
        free(models);
    }
    models = NULL;
}

srmodel_list_t *esp_srmodel_init(const char *partition_label)
{
#ifdef ESP_PLATFORM

#ifdef CONFIG_MODEL_IN_SDCARD
    // Read model data from SD card
    return srmodel_sdcard_init(partition_label);
#else
    // Read model data from flash partition
    const esp_partition_t *partition = NULL;
    // find spiffs partition
    partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, partition_label);

    if (partition) {
        return srmodel_mmap_init(partition);
    } else {
        ESP_LOGE(TAG, "Can not find %s in partition table", partition_label);
    }

    return NULL;
#endif

#else
    return srmodel_sdcard_init(partition_label);
#endif
}

void esp_srmodel_deinit(srmodel_list_t *models)
{
#ifdef ESP_PLATFORM
    return srmodel_mmap_deinit(models);
#else
    return srmodel_sdcard_deinit(models);
#endif
}

// repackage strstr function to support needle==NULL
static char *esp_strstr(const char *haystack, const char *needle)
{
    if (needle == NULL) {
        return (char *)haystack;
    } else {
        return (char *)strstr(haystack, needle);
    }
}

char *esp_srmodel_filter(srmodel_list_t *models, const char *keyword1, const char *keyword2)
{
    if (models == NULL) {
        return NULL;
    }

    // return the first model name including specific keyword
    for (int i = 0; i < models->num; i++) {
        if (esp_strstr(models->model_name[i], keyword1) != NULL) {
            if (esp_strstr(models->model_name[i], keyword2) != NULL) {
                return models->model_name[i];
            }
        }
    }

    return NULL;
}

int esp_srmodel_exists(srmodel_list_t *models, char *model_name)
{
    if (models == NULL) {
        return -1;
    }

    for (int i = 0; i < models->num; i++) {
        if (strcmp(models->model_name[i], model_name) == 0) {
            return i;
        }
    }

    return -1;
}

char *esp_srmodel_get_wake_words(srmodel_list_t *models, char *model_name)
{
    if (models == NULL) {
        return NULL;
    }

    for (int i = 0; i < models->num; i++) {
        if (strcmp(models->model_name[i], model_name) == 0) {
            return get_wake_words_from_info(models->model_info[i]);
        }
    }
    return NULL;
}
