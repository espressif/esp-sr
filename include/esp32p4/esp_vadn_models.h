#pragma once
#include "esp_vadn_iface.h"

#ifdef __cplusplus
extern "C" {
#endif

// The prefix of vadnet model name is used to filter all wakenet from availabel models.
#define ESP_VADN_PREFIX "vadnet"

/**
 * @brief Get the wakenet handle from model name
 *
 * @param model_name   The name of model 
 * @returns The handle of wakenet
 */
const esp_vadn_iface_t *esp_vadn_handle_from_name(const char *model_name);


#ifdef __cplusplus
}
#endif
