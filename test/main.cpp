#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <esp_timer.h>
#include <esp_heap_caps.h>

// --- Pins / Display ---
#define LCD_DC 11
#define LCD_CS 10
#define LCD_SCK 12
#define LCD_MOSI 13
#define LCD_RST 1
#define LCD_BL 14
#define LCD_W 170
#define LCD_H 320
#define COL_O 35
#define ROW_O 0

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 1, true, LCD_W, LCD_H, 0, 0, COL_O, ROW_O);

static lv_display_t *disp = nullptr;
static esp_timer_handle_t tick_timer = nullptr;
static uint32_t frame_counter = 0;

// --- LVGL flush (echter Pixeltransfer) ---
static void flush_cb(lv_display_t *d, const lv_area_t *a, uint8_t *px) {
  uint32_t w = a->x2 - a->x1 + 1;
  uint32_t h = a->y2 - a->y1 + 1;
  // BeRGB erwartet geswappte 16-bit Farben -> passend zu LV_COLOR_16_SWAP=1
  gfx->draw16bitBeRGBBitmap(a->x1, a->y1, reinterpret_cast<uint16_t*>(px), w, h);
  lv_display_flush_ready(d);
}

// --- 1ms Tick ---
static void tick_cb(void *) { lv_tick_inc(1); }

void setup()
{
    Serial.begin(115200);
    delay(100);
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    if (!gfx->begin(60000000))
    {
        Serial.println("GFX init failed");
        while (1)
            delay(100);
    }
    gfx->invertDisplay(true);
    gfx->fillScreen(BLACK);
    Serial.println("GFX OK");

    lv_init();
    Serial.println("LVGL OK");

    uint16_t scr_w = gfx->width();
    uint16_t scr_h = gfx->height();

    // Zwei DMA-taugliche Buffer (32 Zeilen)
    const uint32_t buf_lines = 32;
    const size_t buf_bytes = scr_w * buf_lines * sizeof(lv_color_t);

    lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(buf_bytes,
                                                      MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(buf_bytes,
                                                      MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);

    if (!buf1 || !buf2)
    {
        Serial.println("Buffer alloc failed");
        while (1)
            delay(100);
    }

    disp = lv_display_create(scr_w, scr_h);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(disp, buf1, buf2, buf_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, flush_cb);

    // LVGL tick timer
    const esp_timer_create_args_t tcfg = {.callback = tick_cb, .name = "lv_tick"};
    esp_timer_create(&tcfg, &tick_timer);
    esp_timer_start_periodic(tick_timer, 1000);

    // --- UI ---
    lv_obj_t *bg = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(bg, lv_color_hex(0x202040), 0);
    lv_obj_set_size(bg, scr_w, scr_h);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello LVGL");
    lv_obj_center(label);

    lv_obj_t *fps = lv_label_create(lv_screen_active());
    lv_obj_align(fps, LV_ALIGN_BOTTOM_RIGHT, -4, -2);

    lv_timer_create([](lv_timer_t *t)
                    {
    static uint32_t last = 0;
    uint32_t now = frame_counter;
    uint32_t fps_now = (now - last) * 2;
    last = now;
    char buf[32];
    snprintf(buf, sizeof(buf), "FPS: %lu", (unsigned long)fps_now);
    lv_label_set_text((lv_obj_t*)lv_timer_get_user_data(t), buf); }, 500, fps);

    Serial.println("UI ready");
}

void loop()
{
    lv_timer_handler();
    delay(5);
}