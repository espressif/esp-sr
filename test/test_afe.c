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

#include "model_path.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "dl_lib_convq_queue.h"
#include <sys/time.h>

TEST_CASE("audio_front_end create/destroy API & memory leak", "[afe]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    
    esp_afe_sr_iface_t *afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
    afe_config.wakenet_init = true;
    afe_config.wakenet_model_name = model_name;
    afe_config.voice_communication_init = false;
    

    // test model loading time
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);
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

        printf("create ...\n");
        afe_data = afe_handle->create_from_config(&afe_config);


        printf("destroy ...\n");
        afe_handle->destroy(afe_data);
        esp_srmodel_deinit(models);

        last_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        mem_leak = start_size - last_end_size;
        printf("create&destroy times:%d, memory leak:%d\n", i + 2, mem_leak);
    }

    TEST_ASSERT_EQUAL(true, (mem_leak) < 1000 && last_end_size == first_end_size);
}