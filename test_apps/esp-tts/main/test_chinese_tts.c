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
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_tts.h"
#include "esp_tts_voice_xiaole.h"
#include "esp_tts_voice_template.h"
#include "esp_tts_player.h"
#include "esp_partition.h"
#include "esp_idf_version.h"

TEST_CASE("chinese tts create/destroy API & memory leak", "[tts]")
{
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    /*** 1. create esp tts handle ***/
    const void* voicedata;
    const esp_partition_t* partition=esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "voice_data");
    assert(partition != NULL);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_partition_mmap_handle_t mmap_handle;
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, ESP_PARTITION_MMAP_DATA, &voicedata, &mmap_handle));
#else
    spi_flash_mmap_handle_t mmap_handle;
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &voicedata, &mmap_handle));
#endif 

    // test model loading time
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    esp_tts_voice_t *voice=esp_tts_voice_set_init(&esp_tts_voice_template, (int16_t*)voicedata); 
    esp_tts_handle_t *tts_handle=esp_tts_create(voice);
    gettimeofday(&tv_end, NULL);
    int tv_ms = (tv_end.tv_sec - tv_start.tv_sec) * 1000 + (tv_end.tv_usec - tv_start.tv_usec) / 1000;
    printf("create latency:%d ms\n", tv_ms);

    // test model memory concumption
    int create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    printf("Internal RAM: %d, PSRAM:%d\n", create_internal_size, create_size - create_internal_size);

    // test memory leak
    esp_tts_voice_set_free(voice);
    esp_tts_destroy(tts_handle);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        esp_partition_munmap(mmap_handle); // support esp-idf v5
#else
        spi_flash_munmap(mmap_handle);  // support esp-idf v4
#endif
    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int last_end_size = first_end_size;
    int mem_leak = start_size - last_end_size;
    printf("create&destroy times:%d, memory leak:%d\n", 1, mem_leak);

    for (int i = 0; i < 6; i++) {
        printf("init partition ...\n");
    partition=esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "voice_data");
    assert(partition != NULL);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, ESP_PARTITION_MMAP_DATA, &voicedata, &mmap_handle));
#else
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &voicedata, &mmap_handle));
#endif 

        printf("create ...\n");
        voice=esp_tts_voice_set_init(&esp_tts_voice_template, (int16_t*)voicedata); 
        tts_handle=esp_tts_create(voice);

        printf("destroy ...\n");
        esp_tts_voice_set_free(voice);
        esp_tts_destroy(tts_handle);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        esp_partition_munmap(mmap_handle); // support esp-idf v5
#else
        spi_flash_munmap(mmap_handle);  // support esp-idf v4
#endif
        last_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        mem_leak = start_size - last_end_size;
        printf("create&destroy times:%d, memory leak:%d\n", i + 2, mem_leak);
    }

    TEST_ASSERT_EQUAL(true, (mem_leak) < 1000 && last_end_size == first_end_size);
}