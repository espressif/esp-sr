// debug.c
#include "esp_sr_debug.h"

static int debug_enabled = 0;

void esp_sr_set_debug_mode(int enabled) {
    debug_enabled = enabled ? 1 : 0;
}

int esp_sr_get_debug_mode(void) {
    return debug_enabled;
}
