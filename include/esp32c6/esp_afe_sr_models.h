#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_afe_sr_iface.h"

esp_afe_sr_iface_t *esp_afe_handle_from_config(const afe_config_t *config);

#ifdef __cplusplus
}
#endif
