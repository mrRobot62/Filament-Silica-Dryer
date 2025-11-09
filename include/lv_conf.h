#ifndef LV_CONF_H
#define LV_CONF_H
#pragma message(">>> USING LOCAL LV_CONF.H <<<")

#define LV_USE_LED 1

#define LV_USE_PERF_MONITOR 0
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

// ST7789 über Arduino_GFX: RGB565 + swapped
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

// Tick kommt aus esp_timer
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "esp_timer.h"

#define LV_MEM_CUSTOM 1
#define LV_MEM_SIZE (64U * 1024U)

/* Aktiviere gewünschte Standard-Fonts */
#define LV_FONT_MONTSERRAT_6 1
#define LV_FONT_MONTSERRAT_8 1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_24 0

/* Standard-Font wählen */
#define LV_FONT_DEFAULT &lv_font_montserrat_12

#if LV_USE_LED
#define LV_LED_BRIGHTNESS_ON 255
#define LV_LED_BRIGHTNESS_OFF 20
#endif

#endif // LV_CONF_H