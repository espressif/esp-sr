#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_afe_sr_iface.h"


#if CONFIG_AFE_INTERFACE_V1
extern const esp_afe_sr_iface_t esp_afe_sr_v1;
extern const esp_afe_sr_iface_t esp_afe_vc_v1;
#define ESP_AFE_SR_HANDLE esp_afe_sr_v1
#define ESP_AFE_VC_HANDLE esp_afe_vc_v1

#else
#error No valid afe selected.
#endif


#ifdef __cplusplus
}
#endif