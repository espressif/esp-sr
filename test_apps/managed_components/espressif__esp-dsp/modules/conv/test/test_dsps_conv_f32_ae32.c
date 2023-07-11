// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <math.h>
#include "unity.h"
#include "dsp_platform.h"
#include "esp_log.h"

#include "dsps_conv.h"
#include "esp_attr.h"

static const char *TAG = "dsps_conv";

#define lenA  30
#define lenB  30

static float inputA[lenA];
static float inputB[lenB];
static float output_ref[lenA + lenB - 1 + 2];
static float output_fwd[lenA + lenB - 1 + 2];
static float output_back[lenA + lenB - 1 + 2];

TEST_CASE("dsps_conv_f32_ae32 test output", "[dsps]")
{
    int la = 3;
    int lb = 2;

    for (int i = 0; i < lenA; i++) {
        inputA[i] = 10 + i;
    }
    for (int i = 0; i < lenB; i++) {
        inputB[i] = 20 + i;
    }
    for (int i = 0; i < (lenA + lenB - 1 + 2); i++) {
        output_ref[i] = -1;
        output_fwd[i] = -1;
        output_back[i] = -1;
    }
    dsps_conv_f32_ansi(inputA, la, inputB, lb, &output_ref[1]);
    dsps_conv_f32_ae32(inputA, la, inputB, lb, &output_fwd[1]);

    for (size_t i = 0; i < (la + lb + 1); i++) {
        ESP_LOGD(TAG, "la=%i, lb=%i, i=%i, ref=%2.3f, fwd=%2.3f", la, lb, i, output_ref[i], output_fwd[i]);
    }
    float max_eps = 0.000001;
    for (size_t i = 0; i < (la + lb + 1); i++) {
        if (fabs(output_ref[i] - output_fwd[i]) > max_eps) {
            ESP_LOGE(TAG, "la=%i, lb=%i, i=%i, ref=%2.3f, fwd=%2.3f", la, lb, i, output_ref[i], output_fwd[i]);
        }
        TEST_ASSERT_EQUAL(output_ref[i], output_fwd[i]);
    }
}

TEST_CASE("dsps_conv_f32_ae32 functionality", "[dsps]")
{
    for (size_t la = 2; la < lenA; la++) {
        for (size_t lb = 2; lb < lenB; lb++) {
            for (int i = 0 ; i < lenA ; i++) {
                inputA[i] = (float)rand() / INT32_MAX;
            }
            for (int i = 0 ; i < lenB ; i++) {
                inputB[i] = (float)rand() / INT32_MAX;
            }
            for (int i = 0 ; i < (lenA + lenB  - 1 + 2); i++) {
                output_ref[i] = -1;
                output_fwd[i] = -1;
                output_back[i] = -1;
            }
            dsps_conv_f32_ansi(inputA, la, inputB, lb, &output_ref[1]);
            dsps_conv_f32_ae32(inputA, la, inputB, lb, &output_fwd[1]);
            dsps_conv_f32_ae32(inputB, lb, inputA, la, &output_back[1]);
            float max_eps = 0.000001;
            for (size_t i = 0; i < (la + lb + 1); i++) {
                if ((fabs(output_ref[i] - output_fwd[i]) > max_eps) || (fabs(output_ref[i] - output_back[i]) > max_eps) || (fabs(output_back[i] - output_fwd[i]) > max_eps)) {
                    ESP_LOGE(TAG, "la=%i, lb=%i, i=%i, ref=%2.3f, fwd=%2.3f, back=%2.3f", la, lb, i, output_ref[i], output_fwd[i], output_back[i]);
                }
                TEST_ASSERT_EQUAL(output_ref[i], output_fwd[i]);
                TEST_ASSERT_EQUAL(output_ref[i], output_back[i]);
                TEST_ASSERT_EQUAL(output_back[i], output_fwd[i]);
            }
        }
    }
}


TEST_CASE("dsps_conv_f32_ae32 benchmark", "[dsps]")
{
    int max_N = 1024;
    int conv_size = 64;
    float *x = (float *)malloc(max_N * sizeof(float));
    TEST_ASSERT_NOT_NULL(x);
    float *y = (float *)malloc(max_N * sizeof(float));
    TEST_ASSERT_NOT_NULL(y);
    float *z = (float *)malloc((max_N * 2 + 1) * sizeof(float));
    TEST_ASSERT_NOT_NULL(z);

    for (int i = 0 ; i < max_N ; i++) {
        x[i] = 0;
        y[i] = 1000;
    }

    unsigned int start_b = xthal_get_ccount();
    dsps_conv_f32_ae32(x, max_N, y, conv_size, &z[0]);
    unsigned int end_b = xthal_get_ccount();

    float cycles = end_b - start_b;
    ESP_LOGI(TAG, "dsps_conv_f32_ae32 - %f cycles for signal %i and pattern %i", cycles, max_N, conv_size);
    free(x);
    free(y);
    free(z);
}
