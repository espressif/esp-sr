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

#include "dsps_fft2r.h"
#include "dsps_fft4r.h"
#include "dsp_common.h"
#include "dsp_types.h"
#include <math.h>
#include "esp_attr.h"

#include "dsps_fft4r_platform.h"

#if (dsps_fft4r_fc32_ae32_enabled == 1)
esp_err_t dsps_fft4r_fc32_ae32_(float *data, int N, float *table, int table_size)
{
    int log4N = dsp_power_of_two(N) / 2;
    int m = 2;
    int j;
    float *win = table;

    float *ptr0;
    float *ptr1;
    float *ptr2;
    float *ptr3;
    float *win0;
    float *win1;
    float *win2;
    int start_index;
    int wind_step = table_size >> (2 * log4N - 1);

    asm volatile ("mov %0, %1" : "=a" (ptr0) : "a" (data));
    asm volatile ("mov %0, %1" : "=a" (ptr1) : "a" (data));
    asm volatile ("mov %0, %1" : "=a" (ptr2) : "a" (data));
    asm volatile ("mov %0, %1" : "=a" (ptr3) : "a" (data));
    asm volatile ("mov %0, %1" : "=a" (win0) : "a" (win)); //win0 = win;
    asm volatile ("mov %0, %1" : "=a" (win1) : "a" (win)); //win1 = win;
    asm volatile ("mov %0, %1" : "=a" (win2) : "a" (win)); //win2 = win;

    //asm volatile ("slli %0, %0, 1" : "+a" (m)); ////m <<= 1;

    while (1) {  ///radix 4
        if (log4N == 0) {
            break;
        }
        asm volatile ("srli %0, %0, 2" : "+a" (N)); //N = N >> 2;
        for (j = 0; j < m; j += 2) { // j: which FFT of this step
            asm volatile ("mul16u %0, %1, %2" : "=a" (start_index) : "a" (j), "a" (N));  ///int start_index = j * (N << 1); // N: N-point FFT
            asm volatile ("slli %0, %0, 1" : "+a" (start_index));
            asm volatile ("addx8 %0, %1, %2" : "+a" (ptr0) : "a" (start_index), "a" (data)); //ptr0 = data + (start_index << 1);
            asm volatile ("addx8 %0, %1, %2" : "+a" (ptr1) : "a" (N), "a" (ptr0)); //ptr1 = ptr0 + (N << 1);
            asm volatile ("addx8 %0, %1, %2" : "+a" (ptr2) : "a" (N), "a" (ptr1)); //ptr2 = ptr1 + (N << 1);
            asm volatile ("addx8 %0, %1, %2" : "+a" (ptr3) : "a" (N), "a" (ptr2)); //ptr3 = ptr2 + (N << 1);

            asm volatile ("loopnez %0, _loop_end_radix4_butterfly4_fft" :: "a"(N)); /// for (k = 0; k < N; k++) {
            asm volatile ("lsi f1, %0, 4" :: "a" (ptr0)); //f1 = *(ptr0 + 1);
            asm volatile ("lsi f3, %0, 4" :: "a" (ptr2)); //f3 = *(ptr2 + 1);
            asm volatile ("lsi f0, %0, 0" :: "a" (ptr0)); //f0 = *ptr0;
            asm volatile ("lsi f2, %0, 0" :: "a" (ptr2)); //f2 = *ptr2;
            asm volatile ("add.s f5, f1, f3"); //f5 = f1 + f3;
            asm volatile ("sub.s f7, f1, f3"); //f7 = f1 - f3;
            asm volatile ("lsi f1, %0, 4" :: "a" (ptr1)); //f1 = *(ptr1 + 1);
            asm volatile ("lsi f3, %0, 4" :: "a" (ptr3)); //f3 = *(ptr3 + 1);

            asm volatile ("add.s f4, f0, f2"); //f4 = f0 + f2;
            asm volatile ("sub.s f6, f0, f2"); //f6 = f0 - f2;
            asm volatile ("add.s f9, f1, f3"); //f9 = f1 + f3;
            asm volatile ("sub.s f11, f1, f3"); //f11 = f1 - f3;

            asm volatile ("lsi f0, %0, 0" :: "a" (ptr1)); //f0 = *ptr1;
            asm volatile ("lsi f2, %0, 0" :: "a" (ptr3)); //f2 = *ptr3;

            asm volatile ("lsi f12, %0, 0" :: "a" (win0)); //f12 = *win0; //f12 = win[k * 1 * m * 2 + 0];
            asm volatile ("lsi f13, %0, 0" :: "a" (win1)); //f13 = *win1; //f13 = win[k * 2 * m * 2 + 0];
            asm volatile ("add.s f8, f0, f2"); //f8 = f0 + f2;
            asm volatile ("sub.s f10, f0, f2"); //f10 = f0 - f2;

            asm volatile ("sub.s f1, f5, f9"); //f1 = f5 - f9;
            asm volatile ("add.s f5, f5, f9"); //f5 = f5 + f9;
            asm volatile ("add.s f2, f6, f11"); //f2 = f6 + f11; ///this fft & ifft is different
            asm volatile ("sub.s f6, f6, f11"); //f6 = f6 - f11; ///this fft & ifft is different

            asm volatile ("sub.s f0, f4, f8"); //f0 = f4 - f8;
            asm volatile ("add.s f4, f4, f8"); //f4 = f4 + f8;
            asm volatile ("sub.s f3, f7, f10"); //f3 = f7 - f10; ///this fft & ifft is different
            asm volatile ("add.s f7, f7, f10"); //f7 = f7 + f10; ///this fft & ifft is different

            asm volatile ("lsi f14, %0, 0" :: "a" (win2)); //f14 = *win2; //f14 = win[k * 3 * m * 2 + 0];

            asm volatile ("ssi f5, %0, 4" :: "a" (ptr0)); //*(ptr0 + 1) = f5;
            asm volatile ("ssip f4, %0, 8" :: "a" (ptr0)); //*ptr0 = f4; ptr0 += 2;
            asm volatile ("mul.s f5, f3, f12"); //f5 = f3 * f12;
            asm volatile ("mul.s f4, f2, f12"); //f4 = f2 * f12;
            asm volatile ("mul.s f9, f1, f13"); //f9 = f1 * f13;
            asm volatile ("mul.s f8, f0, f13"); //f8 = f0 * f13;
            asm volatile ("mul.s f11, f7, f14"); //f11 = f7 * f14;
            asm volatile ("mul.s f10, f6, f14"); //f10 = f6 * f14;
            asm volatile ("lsi f12, %0, 4" :: "a" (win0)); //f12 = *(win0 + 1); //f12 = win[k * 1 * m * 2 + 1];
            asm volatile ("lsi f13, %0, 4" :: "a" (win1)); //f13 = *(win1 + 1); //si2 = win[k * 2 * m * 2 + 1];
            asm volatile ("lsi f14, %0, 4" :: "a" (win2)); //f14 = *(win2 + 1); //si3 = win[k * 3 * m * 2 + 1]

            asm volatile ("addx4 %0, %1, %0" : "+a" (win0) : "a" (wind_step)); //win0 += 1 * m;
            asm volatile ("addx8 %0, %1, %0" : "+a" (win1) : "a" (wind_step)); //win1 += 2 * m;
            asm volatile ("addx8 %0, %1, %0" : "+a" (win2) : "a" (wind_step)); //win2 += 2 * m;
            asm volatile ("addx4 %0, %1, %0" : "+a" (win2) : "a" (wind_step)); //win2 += 1 * m;
            asm volatile ("msub.s f5, f2, f12"); //f5 -= f2 * f12;
            asm volatile ("madd.s f4, f3, f12"); //f4 += f3 * f12;
            asm volatile ("msub.s f9, f0, f13"); //f9 -= f0 * f13;
            asm volatile ("madd.s f8, f1, f13"); //f8 += f1 * f13;
            asm volatile ("msub.s f11, f6, f14"); //f11 -= f6 * f14;
            asm volatile ("madd.s f10, f7, f14"); //f10 += f7 * f14;
            asm volatile ("ssi f5, %0, 4" :: "a" (ptr1)); //*(ptr1 + 1) = f5;
            asm volatile ("ssip f4, %0, 8" :: "a" (ptr1)); //*ptr1 = f4; ptr1 += 2;
            asm volatile ("ssi f9, %0, 4" :: "a" (ptr2)); //*(ptr2 + 1) = f9;
            asm volatile ("ssip f8, %0, 8" :: "a" (ptr2)); //*ptr2 = f8; ptr2 += 2;
            asm volatile ("ssi f11, %0, 4" :: "a" (ptr3)); //*(ptr3 + 1) = f11;
            asm volatile ("ssip f10, %0, 8" :: "a" (ptr3)); //*ptr3 = f10; ptr3 += 2;
            //}
            asm volatile ("_loop_end_radix4_butterfly4_fft: mov %0, %1" : "=a" (win0) : "a" (win)); //win0 = win;
            asm volatile ("mov %0, %1" : "=a" (win0) : "a" (win)); //win0 = win;
            asm volatile ("mov %0, %1" : "=a" (win1) : "a" (win)); //win1 = win;
            asm volatile ("mov %0, %1" : "=a" (win2) : "a" (win)); //win2 = win;
        }
        asm volatile ("slli %0, %0, 2" : "+a" (m)); /// m = m << 2;
        asm volatile ("slli %0, %0, 2" : "+a" (wind_step)); /// m = m << 2;
        asm volatile ("addi %0, %0, -1" : "+a" (log4N)); ///log4N--;
    }
    return ESP_OK;
}
#endif // dsps_fft4r_fc32_ae32_enabled

#if (dsps_cplx2real_fc32_ae32_enabled == 1)
esp_err_t dsps_cplx2real_fc32_ae32_(float *data, int fft_points, float *table, int table_size)
{
    float *ptr_inv;

    int wind_step = table_size / (fft_points);

    float *win0 = table + wind_step;
    float *win1 = table + wind_step * 2;

    ////0 & 1 => dc frequency
    ///fft_points * 2 + 0 & fft_points * 2 + 1 => Nyquist frequency
    asm volatile ("const.s f14, 0");  //f14 = 0f;
    asm volatile ("lsi f0, %0, 0" :: "a" (data)); //f0 = *data;
    asm volatile ("lsi f1, %0, 4" :: "a" (data)); //f1 = *(data + 1);
    asm volatile ("addx8 %0, %1, %2" : "=a" (ptr_inv) : "a" (fft_points), "a" (data)); //ptr_inv = data + fft_points * 2;
    asm volatile ("add.s f6, f0, f1"); //f6 = f0 + f1;
    asm volatile ("sub.s f7, f0, f1"); //f7 = f0 - f1;

    asm volatile ("srli %0, %0, 2" : "+a" (fft_points)); //fft_points >>= 2;

    asm volatile ("const.s f14, 3");  //f14 = 0.5f;, this is for multiply 0.5
    asm volatile ("neg.s f15, f14");  //f15 = -f14;

    asm volatile ("ssi f6, %0, 0" :: "a" (data)); //*data = f6;
    asm volatile ("ssi f7, %0, 4" :: "a" (data)); //*ptr_inv = f7;
    asm volatile ("addi %0, %0, -16" : "+a" (ptr_inv)); //ptr_inv -= 4; ///here increase address by -4 because float load/store not support negective immediate offset

    asm volatile ("loopnez %0, __loop_end_fftr_real_post_proc" :: "a" (fft_points)); //for (k = 0; k < fft_points; k++) {
    asm volatile ("lsi f1, %0, 12" :: "a" (data)); //f1 = *(data + 3);
    asm volatile ("lsi f3, %0, 12" :: "a" (ptr_inv)); //f3 = *(ptr_inv + 3);
    asm volatile ("lsi f0, %0, 8" :: "a" (data)); //f0 = *(data + 2);
    asm volatile ("lsi f2, %0, 8" :: "a" (ptr_inv)); //f2 = *(ptr_inv + 2);

    asm volatile ("lsi f7, %0, 20" :: "a" (data)); //f7 = *(data + 5);
    asm volatile ("lsi f9, %0, 4" :: "a" (ptr_inv)); //f9 = *(ptr_inv + 1);

    asm volatile ("lsi f6, %0, 16" :: "a" (data)); //f6 = *(data + 4);
    asm volatile ("lsi f8, %0, 0" :: "a" (ptr_inv)); //f8 = *ptr_inv;
    asm volatile ("sub.s f5, f1, f3"); //f5 = f1 - f3;
    asm volatile ("add.s f4, f0, f2"); //f4 = f0 + f2;
    asm volatile ("sub.s f11, f7, f9"); //f11 = f7 - f9;
    asm volatile ("add.s f10, f6, f8"); //f10 = f6 + f8;

    asm volatile ("add.s f1, f1, f3"); //f1 = f1 + f3;
    asm volatile ("sub.s f0, f0, f2"); //f0 = f0 - f2;

    asm volatile ("lsi f12, %0, 4" :: "a" (win0)); //f12 = *(win + 1);
    asm volatile ("add.s f7, f7, f9"); //f7 = f7 + f9;
    asm volatile ("sub.s f6, f6, f8"); //f6 = f6 - f8;
    asm volatile ("lsi f13, %0, 4" :: "a" (win1)); //f13 = *(win + 3);

    asm volatile ("mul.s f3, f1, f12"); //f3 = f1 * f12;
    asm volatile ("mul.s f2, f0, f12"); //f2 = f0 * f12;
    asm volatile ("lsi f12, %0, 0" :: "a" (win0)); //f12 = *(win + 0);

    asm volatile ("mul.s f9, f7, f13"); //f9 = f7 * f13;
    asm volatile ("mul.s f8, f6, f13"); //f8 = f6 * f13;

    asm volatile ("lsi f13, %0, 0" :: "a" (win1)); //f13 = *(win + 2);

    asm volatile ("madd.s f3, f0, f12"); //f3 += f0 * f12;
    asm volatile ("msub.s f2, f1, f12"); //f2 -= f1 * f12;
    asm volatile ("madd.s f9, f6, f13"); //f9 += f6 * f13;
    asm volatile ("msub.s f8, f7, f13"); //f8 -= f7 * f13;
    asm volatile ("addx8 %0, %1, %0" : "+a" (win0) : "a" (wind_step)); //win0 += 8 * wind_step;
    asm volatile ("addx8 %0, %1, %0" : "+a" (win1) : "a" (wind_step)); //win1 += 8 * wind_step;
    // Here we have tw1: f2,f3 and tw2: f8,f9

    asm volatile ("sub.s f1, f5, f3"); //f1 = f5 - f3;
    asm volatile ("sub.s f0, f4, f2"); //f0 = f4 - f2;

    asm volatile ("add.s f3, f3, f5"); //f3 = f3 + f5;
    asm volatile ("add.s f2, f4, f2"); //f2 = f4 + f2;

    asm volatile ("sub.s f7, f11, f9"); //f7 = f11 - f9;
    asm volatile ("sub.s f6, f10, f8"); //f6 = f10 - f8;
    asm volatile ("add.s f9, f9, f11"); //f9 = f9 + f11;
    asm volatile ("add.s f8, f10, f8"); //f8 = f10 + f8;

    asm volatile ("mul.s f1, f1, f14"); //f1 *= f14;
    asm volatile ("mul.s f0, f0, f14"); //f0 *= f14;

    asm volatile ("mul.s f3, f3, f15"); //f3 *= -f14;
    asm volatile ("mul.s f2, f2, f14"); //f2 *= f14;

    asm volatile ("mul.s f7, f7, f14"); //f7 *= f14;
    asm volatile ("mul.s f6, f6, f14"); //f6 *= f14;
    asm volatile ("mul.s f9, f9, f15"); //f9 *= -f14;
    asm volatile ("mul.s f8, f8, f14"); //f8 *= f14;

    asm volatile ("ssi f1, %0, 12" :: "a" (data)); //*(data + 3) = f1;
    asm volatile ("ssi f0, %0, 8" :: "a" (data)); //*(data + 2) = f0;

    asm volatile ("ssi f3, %0, 12" :: "a" (ptr_inv)); //*(ptr_inv + 3) = f3;
    asm volatile ("ssi f2, %0, 8" :: "a" (ptr_inv)); //*(ptr_inv + 2) = f2;

    asm volatile ("ssi f7, %0, 20" :: "a" (data)); //*(data + 5) = f7;
    asm volatile ("ssi f6, %0, 16" :: "a" (data)); //*(data + 4) = f6;
    asm volatile ("addi %0, %0, 16" : "+a" (data)); //data += 4;

    asm volatile ("ssi f9, %0, 4" :: "a" (ptr_inv)); //*(ptr_inv + 1) = f9;
    asm volatile ("ssi f8, %0, 0" :: "a" (ptr_inv)); //*ptr_inv = f8;
    asm volatile ("addi %0, %0, -16" : "+a" (ptr_inv)); //ptr_inv -= 4;
    //}
    asm volatile ("__loop_end_fftr_real_post_proc: nop");

    return ESP_OK;
}
#endif // dsps_cplx2real_fc32_ae32_enabled
