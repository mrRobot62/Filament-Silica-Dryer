#include "pins.h"
#include "ui/encoder_input.h"
#include "ui/ui.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

// --- Arduino_GFX Bus + Panel (deine funktionierende Signatur) ---
Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, LCD_ROTATION,
                                      true,                        // IPS
                                      LCD_HEIGHT, LCD_WIDTH, 0, 0, // (0,0) vor Offsets
                                      35, 0                        // Offsets (COL_OFFSET=35, ROW_OFFSET=0)
);

// --- LVGL Display / Buffer (LVGL 9) ---
static lv_display_t *disp = nullptr;
static lv_color_t *buf1 = nullptr;
static lv_color_t *buf2 = nullptr;

// LVGL 9: Flush-Signatur ist lv_display_t*, const lv_area_t*, uint8_t*
static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  int32_t w = (area->x2 - area->x1 + 1);
  int32_t h = (area->y2 - area->y1 + 1);
  // LV_COLOR_16_SWAP=1 -> Big-Endian 565 => passt zu draw16bitBeRGBBitmap
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
  lv_display_flush_ready(disp);
}

void setup() {
  // Serial Debug
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("Booting ESP32-S3 HMI FSD 2...");

  // Backlight
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  // Display init
  if (!gfx->begin()) {
    Serial.println("ERROR: gfx->begin() failed!");
    while (true) {
      delay(1000);
    }
  }
  gfx->fillScreen(BLACK);
  Serial.println("Display initialized.");

  // LVGL init
  lv_init();

  // LVGL 9: Display anlegen
  disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
  if (!disp) {
    Serial.println("ERROR: lv_display_create failed!");
    while (true) {
      delay(1000);
    }
  }

  // Zwei DMA-fähige Buffers (je ~ 320*40*2 Bytes)
  size_t buf_pixels = LCD_WIDTH * 40;
  buf1 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_DMA);
  buf2 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_DMA);
  if (!buf1 || !buf2) {
    Serial.println("ERROR: LVGL buffers allocation failed!");
    while (true) {
      delay(1000);
    }
  }

  // LVGL 9: Buffer & Flush registrieren
  // size in BYTES, Render-Mode PARTIAL (typisch für GFX)
  lv_display_set_buffers(disp, buf1, buf2, buf_pixels * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  // UI
  ui_init();
  Serial.println("UI init done.");

  // Encoder
  encoder_init();
  Serial.println("Encoder init done.");

  // Kleiner Hinweis am Rand
  gfx->setTextColor(WHITE);
  gfx->setCursor(6, LCD_HEIGHT - 12);
  gfx->setTextSize(1);
  gfx->print("LVGL ready.");
}

void loop() {
  // LVGL Tick (ca. alle 5 ms)
  static uint32_t last_ms = millis();
  uint32_t now = millis();
  uint32_t diff = now - last_ms;
  if (diff >= 5) {
    lv_tick_inc(diff);
    last_ms = now;
  }

  // Genau EIN Poll + UI-Update (inkl. Logging) pro Zyklus
  ui_task();

  // LVGL verarbeiten
  lv_timer_handler();

  delay(1);
}