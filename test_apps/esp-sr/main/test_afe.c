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

static esp_afe_sr_data_t *afe_data = NULL;


TEST_CASE("audio_front_end SR create/destroy API & memory leak", "[afe]")
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



/******************************************** Divide VC Test ********************************************/



TEST_CASE("audio_front_end VC create/destroy API & memory leak", "[afe]")
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
                    printf("aec_init: %d, se_init: %d, vad_init: %d, voice_communication_agc_init: %d\n", aec_init, se_init, vad_init, voice_communication_agc_init);
                    afe_config.aec_init = aec_init;
                    afe_config.se_init = se_init;
                    afe_config.vad_init = vad_init;
                    afe_config.voice_communication_agc_init = voice_communication_agc_init;

                    start_total_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
                    start_internal_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
                    start_spiram_mem_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

                    for (int i = 0; i < 6; i++) {
                        printf("index: %d\n", i);

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

                        vTaskDelay(100 / portTICK_PERIOD_MS);
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
                }
            }
        }
    }
}