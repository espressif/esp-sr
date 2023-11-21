#pragma once

#include "esp_nsn_iface.h"

// The prefix of nsnet model name is used to filter all wakenet from availabel models.
#define ESP_NSNET_PREFIX "nsnet"

extern const esp_nsn_iface_t esp_nsnet1_quantized;
#define ESP_NSN_HANDLE esp_nsnet1_quantized