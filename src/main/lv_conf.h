#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO

// ST7789 über Arduino_GFX: RGB565 + swapped
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

// Tick kommt aus esp_timer
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "esp_timer.h"

#define LV_MEM_CUSTOM 1
#define LV_MEM_SIZE (64U * 1024U)

#endif // LV_CONF_H