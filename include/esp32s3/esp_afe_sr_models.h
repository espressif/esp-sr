#pragma once

#if defined CONFIG_USE_AFE

#if CONFIG_AFE_INTERFACE_V1
#include "esp_afe_sr_iface.h"
extern const esp_afe_sr_iface_t esp_afe_v1;
#define ESP_AFE_HANDLE esp_afe_v1
#else
#error No valid afe selected.
#endif

#else
#include "esp_afe_sr_iface.h"
extern const esp_afe_sr_iface_t esp_afe_v1;
#define ESP_AFE_HANDLE esp_afe_v1

#endif