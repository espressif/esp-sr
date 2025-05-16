#pragma once


/**
 * @brief Set the debug mode for esp-sr component.
 * 
 * @param enabled   Flag to control debug mode: 
 *                  - 1: Enable debug mode (output debug information)
 *                  - 0: Disable debug mode
 * 
 * @note This setting affects the verbosity of debug output from the esp-sr component.
 *       Debug information may include processing details, intermediate results, etc.
 */
void esp_sr_set_debug_mode(int enabled);

/**
 * @brief Get the current debug mode status of esp-sr component.
 * 
 * @return Current debug mode status:
 *         - 1: Debug mode is enabled
 *         - 0: Debug mode is disabled
 * 
 * @note This function can be used to check whether debug output is currently active
 *       before performing potentially expensive debug logging operations.
 */
int esp_sr_get_debug_mode(void);