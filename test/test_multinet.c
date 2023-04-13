/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "string.h"
#include <limits.h>
#include "unity.h"

#include "model_path.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "dakaidiandeng.h"
#include "alexa.h"
#include "dl_lib_convq_queue.h"
#include <sys/time.h>

TEST_CASE("multinet create/destroy API & memory leak", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    // test model loading time
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    printf("load multinet!\n");
    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    gettimeofday(&tv_end, NULL);
    int tv_ms = (tv_end.tv_sec - tv_start.tv_sec) * 1000 + (tv_end.tv_usec - tv_start.tv_usec) / 1000;
    printf("create latency:%d ms\n", tv_ms);

    // test model memory concumption
    int create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    printf("Internal RAM: %d, PSRAM:%d\n", create_internal_size, create_size - create_internal_size);
    multinet->destroy(model_data);
    esp_srmodel_deinit(models);

    // test memory leak
    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int last_end_size = first_end_size;
    int mem_leak = start_size - last_end_size;
    printf("create&destroy times:%d, memory leak:%d\n", 1, mem_leak);

    for (int i = 0; i < 5; i++) {
        printf("init partition ...\n");
        models = esp_srmodel_init("model");
        model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
        multinet = esp_mn_handle_from_name(model_name);

        int time_out = 3000 + i * 2000;
        printf("create ..., time out = %d\n", time_out);
        model_data = multinet->create(model_name, time_out);

        printf("destroy ...\n");
        multinet->destroy(model_data);
        esp_srmodel_deinit(models);

        last_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        mem_leak = start_size - last_end_size;
        printf("create&destroy times:%d, memory leak:%d\n", i + 2, mem_leak);
    }

    TEST_ASSERT_EQUAL(true, (mem_leak) < 1000 && last_end_size == first_end_size);
}

TEST_CASE("multinet detect/get_results API & cpu loading", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);
    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    int frequency = multinet->get_samp_rate(model_data);
    int audio_chunksize = multinet->get_samp_chunksize(model_data) * sizeof(int16_t);
    int16_t *buffer = malloc(audio_chunksize);
    int chunks = 0;
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);

    while (1) {
        if ((chunks + 1)*audio_chunksize <= sizeof(alexa)) {
            memcpy(buffer, alexa + chunks * audio_chunksize, audio_chunksize);
        } else {
            memset(buffer, 0, audio_chunksize);
        }
        esp_mn_state_t mn_state = multinet->detect(model_data, buffer);
        if (mn_state == ESP_MN_STATE_DETECTED) {
            esp_mn_results_t *mn_result = multinet->get_results(model_data);
            printf("Command id:%d\n", mn_result->command_id[0]);
        }

        chunks++;
        if (chunks == 64) {
            break;
        }
    }
    gettimeofday(&tv_end, NULL);
    int tv_ms = (tv_end.tv_sec - tv_start.tv_sec) * 1000 + (tv_end.tv_usec - tv_start.tv_usec) / 1000;
    chunks -= 7;
    int run_ms = (chunks) * audio_chunksize / sizeof(int16_t) * 1000 / frequency;
    float cpu_loading = tv_ms * 100.0 / run_ms;
    printf("Done! Took %d ms to parse %d ms worth of samples in %d iterations. CPU loading(single core):%.1f%%\n",
           tv_ms, run_ms, chunks, cpu_loading);

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, (cpu_loading < 75 && tv_ms > 0));
}

TEST_CASE("multinet switch loader mode", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, "mn6", NULL);
    if (model_name != NULL) {
        esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

        int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        struct timeval tv_start, tv_end;
        gettimeofday(&tv_start, NULL);

        model_iface_data_t *model_data = multinet->create(model_name, 6000);
        int create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        int create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        int first_create_size = create_size;
        printf("Internal RAM: %d, PSRAM:%d\n\n", create_internal_size, create_size-create_internal_size);

        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_PSRAM_FLASH);
        create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        printf("Internal RAM: %d, PSRAM:%d\n\n", create_internal_size, create_size-create_internal_size);

        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_PSRAM);
        create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        printf("Internal RAM: %d, PSRAM:%d\n\n", create_internal_size, create_size-create_internal_size);

        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_PSRAM);
        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_PSRAM_FLASH);
        create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        printf("Internal RAM: %d, PSRAM:%d\n\n", create_internal_size, create_size-create_internal_size);

        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_FLASH);
        create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        printf("Internal RAM: %d, PSRAM:%d\n\n", create_internal_size, create_size-create_internal_size);

        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_FLASH);
        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_PSRAM);
        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_FLASH);
        model_data = multinet->switch_loader_mode(model_data, ESP_MN_LOAD_FROM_PSRAM_FLASH);
        create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        printf("Internal RAM: %d, PSRAM:%d\n\n", create_internal_size, create_size-create_internal_size);


        gettimeofday(&tv_end, NULL);
        int tv_ms=(tv_end.tv_sec-tv_start.tv_sec)*1000+(tv_end.tv_usec-tv_start.tv_usec)/1000;
        printf("create latency:%d ms\n", tv_ms);
        multinet->destroy(model_data);

        int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        int first_end_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        int last_end_size = first_end_size;
        int last_end_internal_size = first_end_internal_size;
        int mem_leak = start_size - last_end_size;
        printf("create&destroy times:%d, memory leak:%d\n", 1, mem_leak);
        esp_srmodel_deinit(models);
        TEST_ASSERT_EQUAL(true, (mem_leak) < 1000 && first_create_size == create_size);
    } else {
        printf("Just support multinet6 and the later versions\n");
        TEST_ASSERT_EQUAL(true, 1);
    }

}