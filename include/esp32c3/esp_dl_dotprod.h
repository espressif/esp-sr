#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void esp_dl_dotprod_f32(float *input0, float *input1, float *output, int length);

#ifdef __cplusplus
}
#endif
