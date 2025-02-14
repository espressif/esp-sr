/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_aec.h"
#include "esp_afe_aec.h"
#include "audio_test_file.h"
#include "unity.h"
#include "esp_timer.h"


TEST_CASE("test esp32c5 afe aec interface", "[aec]")
{
    // vad_handle_t vad_handle = (vad_handle_t)arg;
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    int sample_rate = 16000;

    afe_aec_handle_t *aec_handle = afe_aec_create("MR", 2, AFE_TYPE_SR, AFE_MODE_LOW_COST);
    afe_aec_destroy(aec_handle);
    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("memory leak for first init: %d\n", start_size - first_end_size);

    aec_handle = afe_aec_create("MR", 2, AFE_TYPE_SR, AFE_MODE_LOW_COST);
    int audio_chunksize = afe_aec_get_chunksize(aec_handle);
    printf("audio chunksize:%d\n", audio_chunksize); //512
    int16_t *buffer = (int16_t *)malloc(audio_chunksize * sizeof(int16_t)*2);
    int16_t *out_buffer = (int16_t *)malloc(audio_chunksize * sizeof(int16_t));

    int chunks = 0;
    uint32_t c0, c1, c_res = 0;
    while (1) {
        if ((chunks + 1)*audio_chunksize * sizeof(int16_t) <= sizeof(audio_mic_file)) {
            memcpy(buffer, audio_mic_file + chunks * audio_chunksize , audio_chunksize * sizeof(int16_t));
            memcpy(buffer+audio_chunksize, audio_ref_file + chunks * audio_chunksize  , audio_chunksize * sizeof(int16_t));            
        } else {
            break;
        }
        
        c0 = esp_timer_get_time();
        afe_aec_process(aec_handle, buffer, out_buffer);
        c1 = esp_timer_get_time();

        c_res += c1 - c0;
        chunks++;
    }

    free(buffer);
    free(out_buffer);
    printf("RAM size after vad detection: total:%d, internal:%d\n", 
            start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT), 
            start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("Done! Took %ld ms to parse %d ms worth of samples in %d iterations.\n", 
            c_res/1000, chunks*audio_chunksize*1000/sample_rate, chunks);
    afe_aec_destroy(aec_handle);

    int end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("memory leak:%d\n", start_size-end_size);
    TEST_ASSERT_EQUAL(true, end_size == start_size);
}


TEST_CASE("test esp32c5 aec", "[aec]")
{
    // vad_handle_t vad_handle = (vad_handle_t)arg;
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    int sample_rate = 16000;

    aec_handle_t *aec_handle = aec_create(sample_rate, 2, 1, AEC_MODE_SR_LOW_COST);
    aec_destroy(aec_handle);
    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("memory leak for first init: %d\n", start_size - first_end_size);

    aec_handle = aec_create(sample_rate, 2, 1, AEC_MODE_SR_LOW_COST);
    int audio_chunksize = aec_get_chunksize(aec_handle);
    printf("audio chunksize:%d\n", audio_chunksize); //512
    int16_t *buffer = (int16_t *)malloc(audio_chunksize * sizeof(int16_t));
    int16_t *ref_buffer = (int16_t *)malloc(audio_chunksize * sizeof(int16_t));
    int16_t *out_buffer = (int16_t *)malloc(audio_chunksize * sizeof(int16_t));

    int chunks = 0;
    uint32_t c0, c1, c_res = 0;
    while (1) {
        if ((chunks + 1)*audio_chunksize * sizeof(int16_t) <= sizeof(audio_mic_file)) {
            memcpy(buffer, audio_mic_file + chunks * audio_chunksize , audio_chunksize * sizeof(int16_t));
            memcpy(ref_buffer, audio_ref_file + chunks * audio_chunksize  , audio_chunksize * sizeof(int16_t));            
        } else {
            break;
        }
        
        c0 = esp_timer_get_time();
        aec_process(aec_handle, buffer, ref_buffer, out_buffer);
        c1 = esp_timer_get_time();

        c_res += c1 - c0;
        chunks++;
    }

    free(buffer);
    free(ref_buffer);
    free(out_buffer);
    printf("RAM size after vad detection: total:%d, internal:%d\n", 
            start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT), 
            start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("Done! Took %ld ms to parse %d ms worth of samples in %d iterations.\n", 
            c_res/1000, chunks*audio_chunksize*1000/sample_rate, chunks);
    aec_destroy(aec_handle);

    int end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("memory leak:%d\n", start_size-end_size);
    TEST_ASSERT_EQUAL(true, end_size == start_size);
}



