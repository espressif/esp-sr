/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "audio_test_file.h"
#include "dl_lib_convq_queue.h"
#include "esp_afe_aec.h"
#include "esp_doa.h"
#include "esp_afe_sr_models.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_nsn_iface.h"
#include "esp_nsn_models.h"
#include "esp_timer.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "model_path.h"
#include "string.h"
#include "unity.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define ARRAY_SIZE_OFsample_rateET 8 // Increase this if audio_sys_get_real_time_stats returns ESP_ERR_INVALID_SIZE
#define AUDIO_SYS_TASKS_ELAPSED_TIME_MS 1000 // Period of stats measurement

static const char *TAG = "AFE_TEST";
static int detect_cnt = 0;
static int fetch_task_flag = 0;

void test_afe_by_config(afe_config_t *afe_config, int frame_num, int *memory, float *cpu, int idx)
{
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int start_internal_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    int first_end_size = 0;
    int end_size = 0;
    int mem_leak = 0;
    uint32_t feed_cpu_time = 0;
    uint32_t fetch_cpu_time = 0;
    uint32_t start = 0, end = 0;
    int loop = 3;
    int feed_chunksize = 0;
    int create_size = 0;
    int create_internal_size = 0;

    for (int i = 0; i < loop; i++) {
        // init config and handle
        esp_afe_sr_iface_t *afe_handle = esp_afe_handle_from_config(afe_config);
        // afe_config_print(afe_config);
        esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);

        create_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
        create_internal_size = start_internal_size - heap_caps_get_free_size(MALLOC_CAP_INTERNAL);

        // run afe feed
        feed_chunksize = afe_handle->get_feed_chunksize(afe_data);
        int feed_nch = afe_handle->get_feed_channel_num(afe_data);
        afe_handle->set_wakenet_threshold(afe_data, 1, 0.8);
        afe_handle->reset_wakenet_threshold(afe_data, 1);

        int16_t *feed_buff = (int16_t *)malloc(feed_chunksize * sizeof(int16_t) * feed_nch);
        start = esp_timer_get_time();
        for (int j = 0; j < frame_num; j++) {
            afe_handle->feed(afe_data, feed_buff);
        }
        end = esp_timer_get_time();
        feed_cpu_time += end - start;

        // run afe fetch
        start = esp_timer_get_time();
        while (1) {
            afe_fetch_result_t *res = afe_handle->fetch_with_delay(afe_data, 1 / portTICK_PERIOD_MS);
            if (res->ret_value != ESP_OK) {
                break;
            }
        }
        end = esp_timer_get_time();
        fetch_cpu_time += end - start;
        free(feed_buff);
        afe_handle->destroy(afe_data);
        end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);

        if (i == 0) {
            first_end_size = end_size;
        }
        mem_leak = start_size - end_size;
        ESP_LOGI(TAG, "create&destroy times:%d, memory leak:%d\n", i, mem_leak);
    }
    uint32_t feed_data_time = loop * frame_num * feed_chunksize / 16 * 1000; // us
    memory[idx * 2] = create_internal_size;
    memory[idx * 2 + 1] = create_size - create_internal_size;
    cpu[idx * 2] = feed_cpu_time * 1.0 / feed_data_time;
    cpu[idx * 2 + 1] = fetch_cpu_time * 1.0 / feed_data_time;
    printf("Internal RAM: %d, PSRAM:%d, feed cpu loading:%f, fetch cpu loading:%f\n",
           memory[idx * 2],
           memory[idx * 2 + 1],
           cpu[idx * 2],
           cpu[idx * 2 + 1]);
    TEST_ASSERT_EQUAL(true, mem_leak < 1000 && end_size == first_end_size);
}

TEST_CASE(">>>>>>>> AFE create/destroy API & memory leak <<<<<<<<", "[afe]")
{
    const char *input_format[6] = {"MR", "MMNR"};
    afe_type_t afe_type[2] = {AFE_TYPE_SR, AFE_TYPE_VC};
    afe_mode_t afe_mode[2] = {AFE_MODE_LOW_COST, AFE_MODE_HIGH_PERF};
    int count = 0;
    int memory[512];
    float cpu[512];

    // test all setting
    srmodel_list_t *models = esp_srmodel_init("model");
    for (int format_id = 0; format_id < 2; format_id++) {
        for (int type_id = 0; type_id < 2; type_id++) {
            for (int mode_id = 0; mode_id < 2; mode_id++) {
                for (int aec_init = 0; aec_init < 2; aec_init++) {
                    for (int se_init = 0; se_init < 2; se_init++) {
                        for (int ns_init = 0; ns_init < 2; ns_init++) {
                            for (int vad_init = 0; vad_init < 2; vad_init++) {
                                for (int wakenet_init = 0; wakenet_init < 2; wakenet_init++) {
                                    printf("format: %s, type: %d, mode: %d, memory size:%d %d\n",
                                           input_format[format_id],
                                           afe_type[type_id],
                                           afe_mode[mode_id],
                                           heap_caps_get_free_size(MALLOC_CAP_8BIT),
                                           count);
                                    afe_config_t *afe_config = afe_config_init(
                                        input_format[format_id], models, afe_type[type_id], afe_mode[mode_id]);
                                    afe_config->aec_init = aec_init;
                                    afe_config->se_init = se_init;
                                    afe_config->ns_init = ns_init;
                                    afe_config->vad_init = vad_init;
                                    afe_config->wakenet_init = wakenet_init;
                                    test_afe_by_config(afe_config, 4, memory, cpu, count);
                                    afe_config_free(afe_config);
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (int idx = 0; idx < 256; idx++) {
        printf("Internal RAM: %d, PSRAM:%d, feed cpu loading:%f, fetch cpu loading:%f\n",
               memory[idx * 2],
               memory[idx * 2 + 1],
               cpu[idx * 2],
               cpu[idx * 2 + 1]);
    }
    printf("AFE create/destroy API & memory leak test done\n");
}

TEST_CASE(">>>>>>>> AFE default setting <<<<<<<<", "[afe_benchmark]")
{
    const char *input_format[6] = {"MR", "MMNR"};
    afe_type_t afe_type[2] = {AFE_TYPE_SR, AFE_TYPE_VC};
    afe_mode_t afe_mode[2] = {AFE_MODE_LOW_COST, AFE_MODE_HIGH_PERF};
    int count = 0;
    int memory[16];
    float cpu[16];

    // test all setting
    srmodel_list_t *models = esp_srmodel_init("model");
    for (int format_id = 0; format_id < 2; format_id++) {
        for (int type_id = 0; type_id < 2; type_id++) {
            for (int mode_id = 0; mode_id < 2; mode_id++) {
                printf("format: %s, type: %d, mode: %d, memory size:%d %d\n",
                       input_format[format_id],
                       afe_type[type_id],
                       afe_mode[mode_id],
                       heap_caps_get_free_size(MALLOC_CAP_8BIT),
                       count);
                afe_config_t *afe_config =
                    afe_config_init(input_format[format_id], models, afe_type[type_id], afe_mode[mode_id]);
                test_afe_by_config(afe_config, 8, memory, cpu, count);
                afe_config_free(afe_config);
                count++;
            }
        }
    }
    count = 0;
    for (int format_id = 0; format_id < 2; format_id++) {
        for (int type_id = 0; type_id < 2; type_id++) {
            for (int mode_id = 0; mode_id < 2; mode_id++) {
                printf("--------format: %s, type: %s, mode: %s------------\n",
                       input_format[format_id],
                       type_id == 0 ? "SR" : "VC",
                       mode_id == 0 ? "LOW_COST" : "HIGH_PERF");
                printf("Internal RAM: %d, PSRAM:%d, feed cpu loading:%f, fetch cpu loading:%f\n",
                       memory[count * 2],
                       memory[count * 2 + 1],
                       cpu[count * 2],
                       cpu[count * 2 + 1]);
                count++;
            }
        }
    }
    printf("test done\n");
}

void test_feed_Task(void *arg)
{
    afe_task_into_t *afe_task_info = (afe_task_into_t *)arg;
    esp_afe_sr_iface_t *afe_handle = afe_task_info->afe_handle;
    esp_afe_sr_data_t *afe_data = afe_task_info->afe_data;

    int feed_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int feed_nch = afe_handle->get_feed_channel_num(afe_data);
    int sample_per_ms = afe_handle->get_samp_rate(afe_data) / 1000;
    int16_t *i2s_buff = (int16_t *)malloc(feed_chunksize * sizeof(int16_t) * feed_nch);
    assert(i2s_buff);
    ESP_LOGI(TAG, "feed task start\n");
    int count = 0;

    while (1) {
        count++;
        afe_handle->feed(afe_data, i2s_buff);
        vTaskDelay((feed_chunksize / sample_per_ms) / portTICK_PERIOD_MS);
        if (count > 100) {
            break;
        }
    }
    if (i2s_buff) {
        free(i2s_buff);
    }
    ESP_LOGI(TAG, "feed task quit\n");
    vTaskDelete(NULL);
}

void test_fetch_Task(void *arg)
{
    // esp_afe_sr_iface_t *afe_handle = &ESP_AFE_SR_HANDLE;
    afe_task_into_t *afe_task_info = (afe_task_into_t *)arg;
    esp_afe_sr_iface_t *afe_handle = afe_task_info->afe_handle;
    esp_afe_sr_data_t *afe_data = afe_task_info->afe_data;
    detect_cnt = 0;
    fetch_task_flag = 1;
    while (1) {
        afe_fetch_result_t *res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL) {
            break;
        }

        if (res->wakeup_state == WAKENET_DETECTED) {
            detect_cnt++;
        }
    }

    // TEST_ASSERT_EQUAL(true, detect_cnt > 0);
    ESP_LOGI(TAG, "detect task quit\n");
    fetch_task_flag = 0;
    vTaskDelete(NULL);
}

TEST_CASE("afe performance test (1ch)", "[afe_perf]")
{
    const char *input_format = "MR";
    afe_type_t afe_type = AFE_TYPE_VC;
    afe_mode_t afe_model[2] = {AFE_MODE_HIGH_PERF, AFE_MODE_LOW_COST};

    // test all setting
    srmodel_list_t *models = esp_srmodel_init("model");

    for (int mode_id = 0; mode_id < 2; mode_id++) {
        afe_config_t *afe_config = afe_config_init(input_format, models, afe_type, afe_model[mode_id]);
        if (afe_config->wakenet_init && afe_config->wakenet_model_name) {
            esp_afe_sr_iface_t *afe_handle = esp_afe_handle_from_config(afe_config);
            esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);
            afe_task_into_t task_info;
            task_info.afe_data = afe_data;
            task_info.afe_handle = afe_handle;
            task_info.feed_task = NULL;
            task_info.fetch_task = NULL;
            fetch_task_flag = 1;
            xTaskCreatePinnedToCore(
                test_feed_Task, "feed_task", 8 * 1024, (void *)(&task_info), 5, &task_info.feed_task, 0);
            xTaskCreatePinnedToCore(
                test_fetch_Task, "fetch_task", 8 * 1024, (void *)(&task_info), 5, &task_info.fetch_task, 0);
            while (fetch_task_flag) {
                vTaskDelay(32 / portTICK_PERIOD_MS);
            }
        }
        afe_config_free(afe_config);
    }
    esp_srmodel_deinit(models);
}

TEST_CASE("afe performance test (2ch)", "[afe_perf]")
{
    const char *input_format = "MMR";
    afe_type_t afe_type = AFE_TYPE_VC;
    afe_mode_t afe_model[2] = {AFE_MODE_HIGH_PERF, AFE_MODE_LOW_COST};

    // test all setting
    srmodel_list_t *models = esp_srmodel_init("model");

    for (int mode_id = 0; mode_id < 2; mode_id++) {
        afe_config_t *afe_config = afe_config_init(input_format, models, afe_type, afe_model[mode_id]);
        if (afe_config->wakenet_init && afe_config->wakenet_model_name) {
            esp_afe_sr_iface_t *afe_handle = esp_afe_handle_from_config(afe_config);
            esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);
            afe_task_into_t task_info;
            task_info.afe_data = afe_data;
            task_info.afe_handle = afe_handle;
            task_info.feed_task = NULL;
            task_info.fetch_task = NULL;
            fetch_task_flag = 1;
            xTaskCreatePinnedToCore(
                &test_feed_Task, "feed_task", 8 * 1024, (void *)(&task_info), 5, &task_info.feed_task, 0);
            xTaskCreatePinnedToCore(
                &test_fetch_Task, "fetch_task", 8 * 1024, (void *)(&task_info), 5, &task_info.fetch_task, 0);
            while (fetch_task_flag) {
                vTaskDelay(32 / portTICK_PERIOD_MS);
            }
        }
        afe_config_free(afe_config);
    }
    esp_srmodel_deinit(models);
}

TEST_CASE("test afe aec interface", "[afe]")
{
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);

    afe_aec_handle_t *afe_aec_handle = afe_aec_create("MNR", 4, AFE_TYPE_SR, AFE_MODE_LOW_COST);
    aec_handle_t *aec_handle = aec_create(16000, 4, 1, AEC_MODE_SR_LOW_COST);
    int frame_size = afe_aec_handle->frame_size;
    int nch = afe_aec_handle->pcm_config.total_ch_num;
    int mic_idx = afe_aec_handle->pcm_config.mic_ids[0];
    int ref_idx = afe_aec_handle->pcm_config.ref_ids[0];
    int frame_bytes = frame_size * sizeof(int16_t);
    int16_t *afe_indata = (int16_t *)heap_caps_calloc(1, frame_bytes * nch, MALLOC_CAP_SPIRAM);
    int16_t *indata = (int16_t *)heap_caps_aligned_calloc(16, 1, frame_bytes, MALLOC_CAP_SPIRAM);
    int16_t *refdata = (int16_t *)heap_caps_aligned_calloc(16, 1, frame_bytes, MALLOC_CAP_SPIRAM);
    int16_t *outdata1 = (int16_t *)heap_caps_aligned_calloc(16, 1, frame_bytes, MALLOC_CAP_SPIRAM);
    int16_t *outdata2 = (int16_t *)heap_caps_aligned_calloc(16, 1, frame_bytes, MALLOC_CAP_SPIRAM);
    int chunks = 0;
    uint32_t c0, c1, t_aec = 0, t_afe_aec = 0;

    while (1) {
        if ((chunks + 1) * frame_bytes <= sizeof(audio_mic_file)) {
            memcpy(indata, audio_mic_file + chunks * frame_size, frame_bytes);
            memcpy(refdata, audio_ref_file + chunks * frame_size, frame_bytes);

            for (int i = 0; i < frame_size; i++) {
                afe_indata[i * nch + mic_idx] = indata[i];
                afe_indata[i * nch + ref_idx] = refdata[i];
            }
        } else {
            break;
        }

        c0 = esp_timer_get_time();
        afe_aec_process(afe_aec_handle, afe_indata, outdata1);
        c1 = esp_timer_get_time();
        t_afe_aec += c1 - c0;

        c0 = esp_timer_get_time();
        aec_process(aec_handle, indata, refdata, outdata2);
        c1 = esp_timer_get_time();

        t_aec += c1 - c0;
        chunks++;
    }

    for (int i = 0; i < frame_size; i++) {
        assert(outdata1[i] == outdata2[i]);
    }
    printf("afe aec interface:%d\n, aec interface:%d\n", t_afe_aec, t_aec);
    afe_aec_destroy(afe_aec_handle);
    aec_destroy(aec_handle);
    free(afe_indata);
    free(indata);
    free(refdata);
    free(outdata1);
    free(outdata2);
    int end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    TEST_ASSERT_EQUAL(true, end_size == start_size);
}

void generate_test_frame(int16_t *left, int16_t *right, int frame_size, float angle_deg, int sample_rate)
{
    int TEST_FREQ = 1000;
    static float phase = 0.0f;
    const float d = 0.06f;
    const float c = 343.0f;

    float theta = angle_deg * M_PI / 180.0f;
    float tau = d * cosf(theta) / c;

    int delay_samples = (int)roundf(tau * sample_rate);
    printf("Angle: %f, Delay: %d samples\n", angle_deg, delay_samples);

    for (int i = 0; i < frame_size; i++) {
        float t = (float)(i + phase) / sample_rate;
        left[i] = (int16_t)(sinf(2 * M_PI * TEST_FREQ * t) * 32767);

        int delayed_index = i - delay_samples;
        right[i] = (int16_t)(sinf(2 * M_PI * TEST_FREQ * (delayed_index + phase) / sample_rate) * 32767);
    }
    phase += frame_size;
}

TEST_CASE("test doa interface", "[afe]")
{
   // 初始化DOA估计器
    int frame_samples = 1024;
    int sample_rate = 16000;
    int16_t *left = (int16_t *)malloc(frame_samples * sizeof(int16_t));
    int16_t *right = (int16_t *)malloc(frame_samples * sizeof(int16_t));
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    doa_handle_t *doa = esp_doa_create(sample_rate, 20.0f, 0.06f, frame_samples);

    uint32_t c0, c1, t_doa = 0;
    int angle = 180;
    for (int f = 0; f < angle; f++) { // 1秒多帧
        generate_test_frame(left, right, frame_samples, f*1.0, sample_rate);
        c0 = esp_timer_get_time();
        float est_angle = esp_doa_process(doa, left, right);
        c1 = esp_timer_get_time();
        t_doa += c1 - c0;

        printf("%.1f\t\t%.1f\n", f*1.0, est_angle); // memory leak
    }
    int doa_mem_size = start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("doa memory size:%d, cpu loading:%f\n", doa_mem_size, (t_doa * 1.0 / 1000000 * sample_rate) / (angle * frame_samples));
 
    esp_doa_destroy(doa);
    int end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);

    // create & destroy 5 times
    for (int i = 0; i < 5; i++) {
        doa = esp_doa_create(sample_rate, 20.0f, 0.06f, frame_samples);
        esp_doa_process(doa, left, right);
        esp_doa_destroy(doa);
    }

    int last_end_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("memory leak:%d\n", start_size - end_size);
    free(left);
    free(right);
    // return 0;
    printf("TEST DONE\n\n");
    TEST_ASSERT_EQUAL(true, (start_size - end_size) < 300);
    TEST_ASSERT_EQUAL(true, last_end_size == end_size);
}
