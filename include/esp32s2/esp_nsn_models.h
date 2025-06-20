#pragma once

#include "esp_nsn_iface.h"

/*
The prefix of nset
Now there are nsnet1 and nsnet2
*/
#define ESP_NSNET_PREFIX "nsnet"

/**
 * @brief Get the nsnet handle from model name
 *
 * @param model_name   The name of model 
 * @returns The handle of multinet
 */
esp_nsn_iface_t *esp_nsnet_handle_from_name(char *model_name);
