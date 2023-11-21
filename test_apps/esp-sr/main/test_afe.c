/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <limits.h>
#include "unity.h"
#include "esp_log.h"

#include "model_path.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "dl_lib_convq_queue.h"
#include <sys/time.h>

#ifdef CONFIG_IDF_TARGET_ESP32S3
#include "esp_nsn_models.h"
#include "esp_nsn_iface.h"
#endif

#define ARRAY_SIZE_OFFSET                   8       // Increase this if audio_sys_get_real_time_stats returns ESP_ERR_INVALID_SIZE
#define AUDIO_SYS_TASKS_ELAPSED_TIME_MS     1000    // Period of stats measurement

static const char *TAG = "AFE_TEST";
static volatile int s_cpu_test_task_flag = 0;
static esp_afe_sr_data_t *afe_data = NULL;

static int total_ram_size_before = 0;
static int internal_ram_size_before = 0;
static int psram_size_before = 0;



#if (CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID && CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS)
const static char *task_state[] = {
    "Running",
    "Ready",
    "Blocked",
    "Suspended",
    "Deleted"
};

/** @brief
* "Extr": Allocated task stack from psram, "Intr": Allocated task stack from internel
*/
const static char *task_stack[] = {"Extr", "Intr"};
#endif


TEST_CASE(">>>>>>>> audio_front_end SR create/destroy API & memory leak <<<<<<<<", "[afe_sr]")
{
    int audio_chunksize = 0;
    int16_t *feed_buff = NULL;

    for (int aec_init = 0; aec_init < 2; aec_init++) {
        for (int se_init = 0; se_init < 2; se_init++) {
            for (int vad_init = 0; vad_init < 2; vad_init++) {
                for (int wakenet_init = 0; wakenet_init < 2; wakenet_init++) {
                    printf("aec_init: %d, se_init: %d, vad_init: %d, wakenet_init: %d\n", aec_init, se_init, vad_init, wakenet_init);

                    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
                    srmodel_list_t *models = esp_srmodel_init("model");
                    char *model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);

                    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
                    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
                    afe_config.aec_init = aec_init;
                    afe_config.se_init = se_init;
                    afe_config.vad_init = vad_init;
                    afe_config.wakenet_init = wakenet_init;
                    afe_config.memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
                    afe_config.wakenet_model_name = model_name;
                    afe_config.voice_communication_init = false;


                    // test model loading time
                    struct timeval tv_start, tv_end;
                    gettimeofday(&tv_start, NULL);
                    afe_data = afe_handle->create_from_config(&afe_config);
                    gettimeofday(&tv_end, NULL);
                    int tv_ms = (tv_end.tv_sec - tv_start.tv_sec) * 1000 + (tv_end.tv_usec - tv_start.tv_usec) / 1000;
                    printf("create latency:%d ms\n", tv_ms);

                    // test model memory concumption
                    int create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
                    int create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
                    printf("Internal RAM: %d, PSRAM:%d\n", create_internal_size, create_size - create_internal_size);
                    afe_handle->destroy(afe_data);
                    esp_srmodel_deinit(models);

                    // test memory leak
                    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                    int last_end_size = first_end_size;
                    int mem_leak = start_size - last_end_size;
                    printf("create&destroy times:%d, memory leak:%d\n", 1, mem_leak);

                    for (int i = 0; i < 6; i++) {
                        printf("init partition ...\n");
                        models = esp_srmodel_init("model");
                        model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
                        afe_config.wakenet_model_name = model_name;

                        printf("create ...\n");
                        afe_data = afe_handle->create_from_config(&afe_config);

                        audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
                        feed_buff = malloc(audio_chunksize * sizeof(int16_t) * afe_config.pcm_config.total_ch_num);
                        assert(feed_buff);

                        afe_handle->feed(afe_data, feed_buff);
                        printf("destroy ...\n");
                        afe_handle->destroy(afe_data);
                        afe_data = NULL;
                        if (feed_buff) {
                            free(feed_buff);
                            feed_buff = NULL;
                        }
                        esp_srmodel_deinit(models);

                        vTaskDelay(100 / portTICK_PERIOD_MS);
                        last_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                        mem_leak = start_size - last_end_size;
                        printf("create&destroy times:%d, memory leak:%d\n", i + 2, mem_leak);
                    }

                    TEST_ASSERT_EQUAL(true, (mem_leak) < 1000 && last_end_size == first_end_size);
                }
            }
        }
    }
}

void test_feed_Task(void *arg)
{
    int sample_per_ms = 16;
    // esp_afe_sr_iface_t *afe_handle = &ESP_AFE_SR_HANDLE;
    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)arg;
    int feed_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int total_nch = afe_handle->get_total_channel_num(afe_data);
    int16_t *i2s_buff = malloc(feed_chunksize * sizeof(int16_t) * total_nch);
    assert(i2s_buff);
    ESP_LOGI(TAG, "feed task start\n");
    // FILE *fp = fopen("/sdcard/out", "w");
    // if (fp == NULL) printf("can not open file\n");

    while (s_cpu_test_task_flag) {
        // FatfsComboWrite(i2s_buff, audio_chunksize * I2S_CHANNEL_NUM * sizeof(int16_t), 1, fp);

        afe_handle->feed(afe_data, i2s_buff);
        vTaskDelay((feed_chunksize / sample_per_ms) / portTICK_PERIOD_MS);
    }
    if (i2s_buff) {
        free(i2s_buff);
    }
    ESP_LOGI(TAG, "feed task quit\n");
    vTaskDelete(NULL);
}

void test_detect_Task(void *arg)
{
    // esp_afe_sr_iface_t *afe_handle = &ESP_AFE_SR_HANDLE;
    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)arg;
    int fetch_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    int16_t *buff = malloc(fetch_chunksize * sizeof(int16_t));
    assert(buff);
    ESP_LOGI(TAG, "------------detect start------------\n");

    // FILE *fp = fopen("/sdcard/out1", "w");
    // if (fp == NULL) printf("can not open file\n");

    while (s_cpu_test_task_flag) {
        afe_fetch_result_t* res = afe_handle->fetch(afe_data); 
        if (!res || res->ret_value == ESP_FAIL) {
            printf("fetch error!\n");
            break;
        }

        if (res->wakeup_state == WAKENET_DETECTED) {
            ESP_LOGI(TAG, "wakeword detected\n");
        }
        if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED) {
            ESP_LOGI(TAG, "AFE_FETCH_CHANNEL_VERIFIED\n");
        }
    }
    if (buff) {
        free(buff);
    }
    ESP_LOGI(TAG, "detect task quit\n");
    vTaskDelete(NULL);
}

esp_err_t audio_sys_get_real_time_stats(void)
{
#if (CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID && CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS)
    TaskStatus_t *start_array = NULL, *end_array = NULL;
    UBaseType_t start_array_size, end_array_size;
    uint32_t start_run_time, end_run_time;
    uint32_t task_elapsed_time, percentage_time;
    esp_err_t ret;

    // Allocate array to store current task states
    start_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    start_array = malloc(sizeof(TaskStatus_t) * start_array_size);
    assert(start_array);
    // Get current task states
    start_array_size = uxTaskGetSystemState(start_array, start_array_size, &start_run_time);
    if (start_array_size == 0) {
        ESP_LOGE(TAG, "Insufficient array size for uxTaskGetSystemState. Trying increasing ARRAY_SIZE_OFFSET");
        ret = ESP_FAIL;
        goto exit;
    }

    vTaskDelay(pdMS_TO_TICKS(AUDIO_SYS_TASKS_ELAPSED_TIME_MS));

    // Allocate array to store tasks states post delay
    end_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    end_array = malloc(sizeof(TaskStatus_t) * end_array_size);
    assert(end_array);

    // Get post delay task states
    end_array_size = uxTaskGetSystemState(end_array, end_array_size, &end_run_time);
    if (end_array_size == 0) {
        ESP_LOGE(TAG, "Insufficient array size for uxTaskGetSystemState. Trying increasing ARRAY_SIZE_OFFSET");
        ret = ESP_FAIL;
        goto exit;
    }

    // Calculate total_elapsed_time in units of run time stats clock period.
    uint32_t total_elapsed_time = (end_run_time - start_run_time);
    if (total_elapsed_time == 0) {
        ESP_LOGE(TAG, "Delay duration too short. Trying increasing AUDIO_SYS_TASKS_ELAPSED_TIME_MS");
        ret = ESP_FAIL;
        goto exit;
    }

    ESP_LOGI(TAG, "| Task              | Run Time    | Per | Prio | HWM       | State   | CoreId   | Stack ");

    // Match each task in start_array to those in the end_array
    for (int i = 0; i < start_array_size; i++) {
        for (int j = 0; j < end_array_size; j++) {
            if (start_array[i].xHandle == end_array[j].xHandle) {

                task_elapsed_time = end_array[j].ulRunTimeCounter - start_array[i].ulRunTimeCounter;
                percentage_time = (task_elapsed_time * 100UL) / (total_elapsed_time * portNUM_PROCESSORS);
                ESP_LOGI(TAG, "| %-17s | %-11d |%2d%%  | %-4u | %-9u | %-7s | %-8x | %s",
                                start_array[i].pcTaskName, task_elapsed_time, percentage_time, start_array[i].uxCurrentPriority,
                                start_array[i].usStackHighWaterMark, task_state[(start_array[i].eCurrentState)],
                                start_array[i].xCoreID, task_stack[esp_ptr_internal(pxTaskGetStackStart(start_array[i].xHandle))]);

                // Mark that task have been matched by overwriting their handles
                start_array[i].xHandle = NULL;
                end_array[j].xHandle = NULL;
                break;
            }
        }
    }

    // Print unmatched tasks
    for (int i = 0; i < start_array_size; i++) {
        if (start_array[i].xHandle != NULL) {
            ESP_LOGI(TAG, "| %s | Deleted", start_array[i].pcTaskName);
        }
    }
    for (int i = 0; i < end_array_size; i++) {
        if (end_array[i].xHandle != NULL) {
            ESP_LOGI(TAG, "| %s | Created", end_array[i].pcTaskName);
        }
    }
    printf("\n");
    ret = ESP_OK;

exit:    // Common return path
    if (start_array) {
        free(start_array);
        start_array = NULL;
    }
    if (end_array) {
        free(end_array);
        end_array = NULL;
    }
    return ret;
#else
    ESP_LOGW(TAG, "Please enbale `CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID` and `CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS` in menuconfig");
    return ESP_FAIL;
#endif
}

void test_print_cpuloading(void *arg)
{
    while (s_cpu_test_task_flag) {
        audio_sys_get_real_time_stats();
        int total_ram_size_after = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        int internal_ram_size_after = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        int psram_size_after = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

        ESP_LOGI(TAG, "total ram consume: %d KB", (total_ram_size_before - total_ram_size_after)/1024);
        ESP_LOGI(TAG, "internal ram consume: %d KB", (internal_ram_size_before - internal_ram_size_after)/1024);
        ESP_LOGI(TAG, "psram consume: %d KB\n\n", (psram_size_before - psram_size_after)/1024);
    }
    vTaskDelete(NULL);
}

TEST_CASE("audio_front_end SR cpu loading and memory info", "[afe_sr]")
{
    srmodel_list_t *models = esp_srmodel_init("model");
    if (models!=NULL) {
        for (int i=0; i < models->num; i++) {
            printf("Load: %s\n", models->model_name[i]);
        }
    }
    char *wn_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    printf("wn_name: %s\n", wn_name);

    total_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    internal_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    psram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_model_name = wn_name;

    afe_data = afe_handle->create_from_config(&afe_config);
    if (!afe_data) {
        printf("afe_data is null!\n");
        return;
    }

    s_cpu_test_task_flag = 1;
    xTaskCreatePinnedToCore(&test_feed_Task, "feed", 8 * 1024, (void *)afe_handle, 5, NULL, 0);
    xTaskCreatePinnedToCore(&test_detect_Task, "detect", 8 * 1024, (void *)afe_handle, 5, NULL, 1);
    xTaskCreatePinnedToCore(&test_print_cpuloading, "cpuloading", 4 * 1024, NULL, 5, NULL, 1);

    vTaskDelay(10000 / portTICK_PERIOD_MS);
    s_cpu_test_task_flag = 0;

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "destroy\n");
    afe_handle->destroy(afe_data);
    afe_data = NULL;
    esp_srmodel_deinit(models);
    ESP_LOGI(TAG, "successful\n");
}



/******************************************** Divide VC Test ********************************************/



TEST_CASE("audio_front_end VC create/destroy API & memory leak", "[afe_vc]")
{
    int start_total_mem_size = 0;
    int start_internal_mem_size = 0;
    int start_spiram_mem_size = 0;
    int end_total_mem_size = 0;
    int end_internal_mem_size = 0;
    int end_spiram_mem_size = 0;

    int audio_chunksize = 0;
    int16_t *feed_buff = NULL;

    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_VC_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_init = false;
    afe_config.voice_communication_init = true;

    for (int aec_init = 0; aec_init < 2; aec_init++) {
        for (int se_init = 0; se_init < 2; se_init++) {
            for (int vad_init = 0; vad_init < 2; vad_init++) {
                for (int voice_communication_agc_init = 0; voice_communication_agc_init < 2; voice_communication_agc_init++) {
                    #ifdef CONFIG_IDF_TARGET_ESP32S3
                        for (int afe_ns_mode = 0; afe_ns_mode < 2; afe_ns_mode++) {
                    #else
                            int afe_ns_mode = NS_MODE_SSP;
                    #endif
                            printf("aec_init: %d, se_init: %d, vad_init: %d, voice_communication_agc_init: %d, afe_ns_mode: %d\n", aec_init, se_init, vad_init, voice_communication_agc_init, afe_ns_mode);
                            afe_config.aec_init = aec_init;
                            afe_config.se_init = se_init;
                            afe_config.vad_init = vad_init;
                            afe_config.voice_communication_agc_init = voice_communication_agc_init;
                            afe_config.afe_ns_mode = afe_ns_mode;

                            //start_total_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                            //start_internal_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
                            //start_spiram_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

                            for (int i = 0; i < 2; i++) {
                                printf("index: %d\n", i);
                                vTaskDelay(500 / portTICK_PERIOD_MS);
                                start_total_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                                start_internal_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
                                start_spiram_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
                                srmodel_list_t *models = esp_srmodel_init("model");
                                char *nsnet_name = NULL;
                            #ifdef CONFIG_IDF_TARGET_ESP32S3
                                nsnet_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);
                            #endif
                                printf("nsnet_name: %s\n", nsnet_name ? nsnet_name : "");
                                afe_config.afe_ns_model_name = nsnet_name;
                                afe_data = afe_handle->create_from_config(&afe_config);
                                if (!afe_data) {
                                    printf("afe_data is null\n");
                                    continue;
                                }

                                audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
                                feed_buff = malloc(audio_chunksize * sizeof(int16_t) * afe_config.pcm_config.total_ch_num);
                                assert(feed_buff);

                                afe_handle->feed(afe_data, feed_buff);
                                afe_handle->destroy(afe_data);
                                afe_data = NULL;
                                if (feed_buff) {
                                    free(feed_buff);
                                    feed_buff = NULL;
                                }

                                esp_srmodel_deinit(models);
                                vTaskDelay(1000 / portTICK_PERIOD_MS);
                                end_total_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                                end_internal_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
                                end_spiram_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

                                printf("memory leak: %d\n", start_total_mem_size - end_total_mem_size);
                                if (i > 0) {     // skip index = 0
                                    TEST_ASSERT_EQUAL(start_internal_mem_size, end_internal_mem_size);
                                    TEST_ASSERT_EQUAL(start_spiram_mem_size, end_spiram_mem_size);
                                    TEST_ASSERT_EQUAL(start_total_mem_size, end_total_mem_size);
                                } else {
                                    TEST_ASSERT_EQUAL(true, (start_total_mem_size - end_total_mem_size) < 1000);
                                }
                            }
                    #ifdef CONFIG_IDF_TARGET_ESP32S3
                        }
                    #endif
                }
            }
        }
    }
}

TEST_CASE("audio_front_end VC cpu loading and memory info", "[afe_vc]")
{
    total_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    internal_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    psram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *nsnet_name = NULL;
#ifdef CONFIG_IDF_TARGET_ESP32S3
    nsnet_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);
#endif
    printf("nsnet_name: %s\n", nsnet_name ? nsnet_name : "");

    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_VC_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_init = false;
    afe_config.voice_communication_init = true;
    afe_config.voice_communication_agc_init = true;
#ifdef CONFIG_IDF_TARGET_ESP32S3
    afe_config.afe_ns_mode = NS_MODE_NET;
#else
    afe_config.afe_ns_mode = NS_MODE_SSP;
#endif
    afe_config.afe_ns_model_name = nsnet_name;

    afe_data = afe_handle->create_from_config(&afe_config);
    if (!afe_data) {
        printf("afe_data is null!\n");
        return;
    }

    s_cpu_test_task_flag = 1;
    xTaskCreatePinnedToCore(&test_feed_Task, "feed", 8 * 1024, (void *)afe_handle, 5, NULL, 0);
    xTaskCreatePinnedToCore(&test_detect_Task, "detect", 8 * 1024, (void *)afe_handle, 5, NULL, 1);
    xTaskCreatePinnedToCore(&test_print_cpuloading, "cpuloading", 4 * 1024, NULL, 5, NULL, 1);

    vTaskDelay(20000 / portTICK_PERIOD_MS);
    s_cpu_test_task_flag = 0;

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "destroy\n");
    afe_handle->destroy(afe_data);
    esp_srmodel_deinit(models);
    afe_data = NULL;
    ESP_LOGI(TAG, "successful\n");
}
