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

#include "dsps_fft2r_platform.h"
#if (dsps_fft2r_fc32_ae32_enabled == 1)

// This is matrix multipliction function for ESP32 processor.
	.text
	.align  4
	.global dsps_fft2r_fc32_ae32_
	.type   dsps_fft2r_fc32_ae32_,@function

//    .extern dsps_fft_w_table_fc32;
    .global dsps_fft_w_table_fc32;

// The function implements the following C code:
//esp_err_t dsps_fft2r_fc32_ansi(float *data, int N)
//{
//    float *w = dsps_fft_w_table_fc32;
//
//    int ie, ia, m;
//    float re_temp, im_temp;
//    float c, s;
//    int N2 = N;
//    ie = 1;
//    for (int N2 = N/2; N2 > 0; N2 >>= 1) {
//        ia = 0;
//        for (int j = 0; j < ie; j++) {
//            c = w[2 * j];
//            s = w[2 * j + 1];
//            for (int i = 0; i < N2; i++) {
//                m = ia + N2;
//                re_temp = c * data[2 * m] + s * data[2 * m + 1];
//                im_temp = c * data[2 * m + 1] - s * data[2 * m];
//                data[2 * m] = data[2 * ia] - re_temp;
//                data[2 * m + 1] = data[2 * ia + 1] - im_temp;
//                data[2 * ia] = data[2 * ia] + re_temp;
//                data[2 * ia + 1] = data[2 * ia + 1] + im_temp;
//                ia++;
//            }
//            ia += N2;
//        }
//        ie <<= 1;
//    }
//    return result;
//}


dsps_fft2r_fc32_ae32_: 
//esp_err_t dsps_fft2r_fc32_ansi(float *data, int N, float* dsps_fft_w_table_fc32)

	entry	a1, 16
	// Array increment for floating point data should be 4
// data - a2
// N - a3
// dsps_fft_w_table_fc32 - a4 - for now

// a6 - k, main loop counter; N2 - for (int N2 = N/2; N2 > 0; N2 >>= 1)
// a7 - ie
// a8 - j
// a9 - test
// a10 - (j*2)<<2,  or a10 - j<<3
// f0 - c or w[2 * j]
// f1 - s or w[2 * j + 1]
// a11 - ia
// a12 - m
// a13 - ia pointer
// a14 - m pointer
// f6  - re_temp
// f7  - im_temp

// a15 - debug

    // This instruction are not working. Have to be fixed!!!
    // For now theres no solution...
//    l32r    a4, dsps_fft_w_table_fc32_ae32
    
    // Load shift register with 1
    movi.n  a5, 1   // a5 = 1;
    ssr a5          // load shift register with 1
    
    srli a6, a3, 1 // a6 = N2 = N/2
    movi a7, 1     // a7 - ie

.fft2r_l1: 
    movi a8, 0     // a8 - j
    movi a11,0     // a11 = ia = 0;

.fft2r_l2:           // loop for j, a8 - j
        slli    a10, a8, 3 // a10 = j<<3 // shift for cos ()   -- c = w[2 * j];
        add.n   a10, a10, a4 // a10 - pointer to cos
        lsi     f0, a10, 0   
        lsi     f1, a10, 4

        movi a9, 0 // just for debug
        loopnez a6, .fft2r_l3
            add.n    a12, a11, a6   // a12 = m = ia + N2

            slli     a14, a12, 3    // a14 - pointer for m*2
            slli     a13, a11, 3    // a13 - pointer for ia*2
            add.n    a14, a14, a2   // pointers to data arrays
            add.n    a13, a13, a2   //

            lsi      f4, a14, 0     // data[2 * m]
            mul.s    f6, f0, f4     // re_temp =  c * data[2 * m]
            lsi      f5, a14, 4     // data[2 * m + 1]
            mul.s    f7, f0, f5     // im_temp =  c * data[2 * m + 1]

            lsi      f2, a13, 0     // data[2 * ia] 
            madd.s   f6, f1, f5     // re_temp += s * data[2 * m + 1];
            lsi      f3, a13, 4     // data[2 * ia + 1]
            msub.s   f7, f1, f4     // im_temp -= s * data[2 * m];
            
            sub.s    f8, f2, f6     // = data[2 * ia] - re_temp;
            sub.s    f9, f3, f7     // = data[2 * ia + 1] - im_temp;

            add.s    f10, f2, f6    // = data[2 * ia] + re_temp;
            ssi      f8, a14, 0 
            add.s    f11, f3, f7    // = data[2 * ia + 1] + im_temp;            
            ssi      f9, a14, 4 

            ssi      f10, a13, 0 
            addi     a11, a11, 1// ia++
            ssi      f11, a13, 4 
.fft2r_l3:
        add     a11, a11, a6

        addi    a8, a8, 1     // j++
        BNE     a8, a7, .fft2r_l2 // 
    slli    a7, a7, 1  // ie = ie<<1
// main loop: for (int k = N/2; k > 0; k >>= 1)
    srli    a6, a6, 1  // a6 = a6>>1
    BNEZ    a6, .fft2r_l1// Jump if > 0

	movi.n	a2, 0 // return status ESP_OK
	retw.n

#endif // dsps_fft2r_fc32_ae32_enabled