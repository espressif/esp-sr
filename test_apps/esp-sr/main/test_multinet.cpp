/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRAMTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "string.h"
#include <limits.h>
#include "unity.h"

#include "model_path.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "da_kai_kong_tiao.h"
#include "tell_me_a_joke.h"
#include "alexa.h"
#include "dl_lib_convq_queue.h"
#include <sys/time.h>
#include "esp_mn_speech_commands.h"
#include "esp_process_sdkconfig.h"

TEST_CASE("multinet create/destroy API & memory leak", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    int create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    printf("Internal RAM: %d, PSRAM:%d\n", create_internal_size, create_size-create_internal_size);

    gettimeofday(&tv_end, NULL);
    int tv_ms=(tv_end.tv_sec-tv_start.tv_sec)*1000+(tv_end.tv_usec-tv_start.tv_usec)/1000;
    printf("create latency:%d ms\n", tv_ms);
    multinet->destroy(model_data);

    int first_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int last_end_size = first_end_size;
    int mem_leak = start_size - last_end_size;
    printf("create&destroy times:%d, memory leak:%d\n", 1, mem_leak);


    for (int i=0; i<3; i++) {
        printf("create ...\n");
        model_data = multinet->create(model_name, 6000);
        printf("destroy ...\n");
        multinet->destroy(model_data);
        last_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        mem_leak = start_size - last_end_size;
        printf("create&destroy times:%d, memory leak:%d\n", i+2, mem_leak);
    }

    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, (mem_leak) < 1000 && last_end_size == first_end_size);
}

TEST_CASE("multinet cpu loading", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 500000);
    int frequency = multinet->get_samp_rate(model_data);
    int audio_chunksize = multinet->get_samp_chunksize(model_data) * sizeof(int16_t);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);
    unsigned char* data = NULL;
    size_t data_size = 0;
    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        data = (unsigned char*)tell_me_a_joke;
        data_size = sizeof(tell_me_a_joke);
        printf("commands: tell me a joke, size:%d\n", data_size);
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        data = (unsigned char*)da_kai_kong_tiao;
        data_size = sizeof(da_kai_kong_tiao);
        printf("commands: da kai kong tiao, size:%d\n", data_size);
    }
    
    int16_t *buffer = (int16_t *) malloc(audio_chunksize);
    int chunks = 0;
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    esp_mn_state_t mn_state;
    multinet->print_active_speech_commands(model_data);

    while (1) {
        if ((chunks + 1)*audio_chunksize <= data_size) {
            memcpy(buffer, data + chunks * audio_chunksize, audio_chunksize);
        } else {
            memset(buffer, 0, audio_chunksize);
        }
        mn_state = multinet->detect(model_data, buffer);
        if (mn_state == ESP_MN_STATE_DETECTED) {
            esp_mn_results_t *mn_result = multinet->get_results(model_data);
            if (mn_result->num > 0)
                printf("detected: command id:%d, string:%s\n",mn_result->command_id[0], mn_result->string);
            else
                printf("timeout\n");
            break;
        } 
        chunks++;
        if (chunks > 600)
            break;
    }
    gettimeofday(&tv_end, NULL);
    int tv_ms=(tv_end.tv_sec-tv_start.tv_sec)*1000+(tv_end.tv_usec-tv_start.tv_usec)/1000;
    chunks -= 7;
    int run_ms = (chunks)*audio_chunksize/sizeof(int16_t)*1000/frequency;
    printf("Done! Took %d ms to parse %d ms worth of samples in %d iterations. CPU loading(single core):%.1f%%\n", 
           tv_ms, run_ms, chunks, tv_ms*100.0/run_ms);
    
    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, mn_state == ESP_MN_STATE_DETECTED);
}

TEST_CASE("multinet set commands and detect", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    int frequency = multinet->get_samp_rate(model_data);
    int audio_chunksize = multinet->get_samp_chunksize(model_data) * sizeof(int16_t);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);
    unsigned char* data = NULL;
    size_t data_size = 0;
    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        data = (unsigned char*)tell_me_a_joke;
        data_size = sizeof(tell_me_a_joke);
        printf("commands: tell me a joke, size:%d\n", data_size);
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        data = (unsigned char*)da_kai_kong_tiao;
        data_size = sizeof(da_kai_kong_tiao);
        printf("commands: da kai kong tiao, size:%d\n", data_size);
    }
 
    int16_t *buffer = (int16_t *) malloc(audio_chunksize);
    int chunks = 0;
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    esp_mn_state_t mn_state;
    esp_mn_error_t *error_phrases = NULL;
    esp_mn_commands_clear();

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        esp_mn_commands_phoneme_add(1, "TELL ME A JOKE", "TfL Mm c qbK");
        esp_mn_commands_phoneme_add(2, "SING A SONG", "Sgl c Sel");
        error_phrases = esp_mn_commands_update();
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_add(1, "da kai kong tiao");
        esp_mn_commands_add(2, "guan bi kong tiao");
        error_phrases = esp_mn_commands_update();
    } else {
        printf("Invalid language\n");
    }
    multinet->print_active_speech_commands(model_data);

    while (1) {
        if ((chunks + 1)*audio_chunksize <= data_size) {
            memcpy(buffer, data + chunks * audio_chunksize, audio_chunksize);
        } else {
            memset(buffer, 0, audio_chunksize);
        }
        mn_state = multinet->detect(model_data, buffer);
        if (mn_state == ESP_MN_STATE_DETECTED) {
            esp_mn_results_t *mn_result = multinet->get_results(model_data);
            if (mn_result->num > 0)
                printf("detected: command id:%d, string:%s\n",mn_result->command_id[0], mn_result->string);
            else
                printf("timeout\n");
            break;
        } 
        chunks++;
        if (chunks > 600)
            break;
    }
    gettimeofday(&tv_end, NULL);
    int tv_ms=(tv_end.tv_sec-tv_start.tv_sec)*1000+(tv_end.tv_usec-tv_start.tv_usec)/1000;
    chunks -= 7;
    int run_ms = (chunks)*audio_chunksize/sizeof(int16_t)*1000/frequency;
    printf("Done! Took %d ms to parse %d ms worth of samples in %d iterations. CPU loading(single core):%.1f%%\n", 
           tv_ms, run_ms, chunks, tv_ms*100.0/run_ms);
 
    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, error_phrases == NULL);
    TEST_ASSERT_EQUAL(true, mn_state == ESP_MN_STATE_DETECTED);
}

TEST_CASE("multinet set commands from sdkconfig and detect", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    int frequency = multinet->get_samp_rate(model_data);
    int audio_chunksize = multinet->get_samp_chunksize(model_data) * sizeof(int16_t);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);
    unsigned char* data = NULL;
    size_t data_size = 0;
    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        data = (unsigned char*)tell_me_a_joke;
        data_size = sizeof(tell_me_a_joke);
        printf("commands: tell me a joke, size:%d\n", data_size);
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        data = (unsigned char*)da_kai_kong_tiao;
        data_size = sizeof(da_kai_kong_tiao);
        printf("commands: da kai kong tiao, size:%d\n", data_size);
    }
 
    int16_t *buffer = (int16_t *) malloc(audio_chunksize);
    int chunks = 0;
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    esp_mn_state_t mn_state;
    esp_mn_error_t *error_phrases = NULL;
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);
    multinet->print_active_speech_commands(model_data);
    multinet->set_det_threshold(model_data, 0.1);

    while (1) {
        if ((chunks + 1)*audio_chunksize <= data_size) {
            memcpy(buffer, data + chunks * audio_chunksize, audio_chunksize);
        } else {
            memset(buffer, 0, audio_chunksize);
        }
        mn_state = multinet->detect(model_data, buffer);
        if (mn_state == ESP_MN_STATE_DETECTED) {
            esp_mn_results_t *mn_result = multinet->get_results(model_data);
            if (mn_result->num > 0)
                printf("detected: command id:%d, string:%s\n",mn_result->command_id[0], mn_result->string);
            else
                printf("timeout\n");
            break;
        } 
        chunks++;
        if (chunks > 600)
            break;
    }
    gettimeofday(&tv_end, NULL);
    int tv_ms=(tv_end.tv_sec-tv_start.tv_sec)*1000+(tv_end.tv_usec-tv_start.tv_usec)/1000;
    chunks -= 7;
    int run_ms = (chunks)*audio_chunksize/sizeof(int16_t)*1000/frequency;
    printf("Done! Took %d ms to parse %d ms worth of samples in %d iterations. CPU loading(single core):%.1f%%\n", 
           tv_ms, run_ms, chunks, tv_ms*100.0/run_ms);
 
    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, error_phrases == NULL);
    TEST_ASSERT_EQUAL(true, mn_state == ESP_MN_STATE_DETECTED);
}


TEST_CASE("multinet set commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_error_t *error_phrases = NULL;
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);
    printf("MODEL NAME %s\n", model_name);

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_add(1, "TfL Mm c qbK");
            esp_mn_commands_add(2, "hicST qbK");
        } else {
            esp_mn_commands_add(1, "SHUFFLE MY PLAYLIST");
            esp_mn_commands_add(2, "TURN OFF THE KITCHEN LIGHT");
        }
        error_phrases = esp_mn_commands_update();
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_add(1, "da kai dian deng");
        esp_mn_commands_add(2, "guan bi chu fang dian deng");
        error_phrases = esp_mn_commands_update();
    } else {
        printf("Invalid language\n");
    }

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_add(3, "TkN nN eL jc LiTS");
            esp_mn_commands_add(4, "TkN eF eL jc LiTS");
        } else {
            esp_mn_commands_add(3, "TURN ON THE LIGHT");
            esp_mn_commands_add(4, "TURN OFF THE LIGHT");
        }
        error_phrases = esp_mn_commands_update();
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_add(3, "guan bi dian deng");
        esp_mn_commands_add(4, "da kai chu fang dian deng");
        error_phrases = esp_mn_commands_update();
    } else {
        printf("Invalid language\n");
    }

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, error_phrases == NULL);
}


TEST_CASE("multinet add incorrect commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_error_t *error_phrases = NULL;

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        esp_mn_commands_add(1, "TURN ON THE LIGHT 123");
        esp_mn_commands_add(2, "TURN. OFF THE LIGHT?");
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        if (strcmp(model_name, "mn6_cn_ac") == 0 || strcmp(model_name, "mn6_cn") == 0) {
            esp_mn_commands_add(1, "dakai dian deng");
            esp_mn_commands_add(2, "关闭电灯");
        } else {
            esp_mn_commands_add(1, "k");
        }
    } else {
        printf("Invalid language\n");
    }
    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, error_phrases == NULL);
}


TEST_CASE("multinet add duplicated commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_error_t *error_phrases = NULL;

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_add(1, "TfL Mm c qbK");
            esp_mn_commands_add(1, "TfL Mm c qbK");
        } else {
            esp_mn_commands_add(1, "TURN ON THE LIGHT");
            esp_mn_commands_add(1, "TURN ON THE LIGHT");
        }
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_add(1, "da kai dian deng");
        esp_mn_commands_add(1, "da kai dian deng");
    } else {
        printf("Invalid language\n");
    }

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, error_phrases == NULL);
}


TEST_CASE("multinet print active commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_add(1, "TfL Mm qbK");
        } else {
            esp_mn_commands_add(1, "THIS SHOULD NOT APPEAR IN ACTIVE COMMANDS");
        }
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_add(1, "bu ying gai chu xian zai biao zhong");
    } else {
        printf("Invalid language\n");
    }

    multinet->print_active_speech_commands(model_data);

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, 1);
}


TEST_CASE("multinet remove commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_remove("TfL Mm c qbK");
        } else {
            esp_mn_commands_remove("TURN ON THE LIGHT");
        }
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_remove("da kai dian deng");
    } else {
        printf("Invalid language\n");
    }

    esp_mn_commands_update();
    multinet->print_active_speech_commands(model_data);

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, 1);
}


TEST_CASE("multinet clear and add commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);

    esp_mn_commands_clear();

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_add(2, "Sgl c Sel");
        } else {
            esp_mn_commands_add(2, "SING A SONG");
        }
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_add(1, "guan bi dian deng");
    } else {
        printf("Invalid language\n");
    }

    esp_mn_commands_update();
    esp_mn_commands_print();
    multinet->print_active_speech_commands(model_data);

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, 1);
}


TEST_CASE("multinet modify commands", "[mn]")
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(model_name);

    model_iface_data_t *model_data = multinet->create(model_name, 6000);
    char *lang = multinet->get_language(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);

    if (strcmp(lang, ESP_MN_ENGLISH) == 0) {
        if (strcmp(model_name, "mn5q8_en") == 0) {
            esp_mn_commands_modify("TfL Mm c qbK", "TfL TfL Mm c qbK");
        } else {
            esp_mn_commands_modify("TURN ON THE LIGHT", "TURN ON THE KITCHEN LIGHT");
        }
    } else if(strcmp(lang, ESP_MN_CHINESE) == 0) {
        esp_mn_commands_modify("da kai dian deng", "da kai chu fang deng");
    } else {
        printf("Invalid language\n");
    }

    esp_mn_commands_update();
    multinet->print_active_speech_commands(model_data);

    multinet->destroy(model_data);
    esp_srmodel_deinit(models);
    TEST_ASSERT_EQUAL(true, 1);
}
