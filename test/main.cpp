// ESP32-S3 HMI (ESP32-1732S019N) + ST7789 170x320 (SPI) + LVGL 9 Widgets Demo (Landscape, Tabs on TOP)
// Rotation=1, COL_OFFSET=35, ROW_OFFSET=0, LV_COLOR_16_SWAP=1

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <esp_timer.h>
#include <esp_heap_caps.h>

// --- Display pins ---
#define LCD_DC 11
#define LCD_CS 10
#define LCD_SCK 12
#define LCD_MOSI 13
#define LCD_RST 1
#define LCD_BL 14

// --- Panel geometry ---
#define LCD_W 170
#define LCD_H 320
#define COL_O 35
#define ROW_O 0

// --- SPI bus + ST7789 (Landscape) ---
Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 1 /*rotation*/, true /*IPS*/,
                                      LCD_W, LCD_H,
                                      /*rot0/2*/ 0, 0,
                                      /*rot1/3*/ COL_O, ROW_O);

// --- LVGL globals ---
static lv_display_t *disp = nullptr;
static esp_timer_handle_t tick_timer = nullptr;
static lv_obj_t *label_fps = nullptr;
static lv_obj_t *label_slider_val = nullptr;
static lv_obj_t *arc = nullptr;
static lv_obj_t *chart = nullptr;
static lv_chart_series_t *ser = nullptr;
static volatile uint32_t frame_counter = 0;

// --- LVGL flush: SWAP=1 -> BeRGB path ---
static void flush_cb(lv_display_t *d, const lv_area_t *a, uint8_t *px)
{
  uint32_t w = a->x2 - a->x1 + 1;
  uint32_t h = a->y2 - a->y1 + 1;
  gfx->draw16bitBeRGBBitmap(a->x1, a->y1, reinterpret_cast<uint16_t *>(px), w, h);
  lv_display_flush_ready(d);
  frame_counter++;
}

// 1 ms Tick
static void tick_cb(void *) { lv_tick_inc(1); }

static void ui_build()
{
  uint16_t scr_w = lv_display_get_horizontal_resolution(lv_display_get_default()); // 320
  uint16_t scr_h = lv_display_get_vertical_resolution(lv_display_get_default());   // 170

  // Hintergrund
  lv_obj_t *bg = lv_obj_create(lv_screen_active());
  lv_obj_set_size(bg, scr_w, scr_h);
  lv_obj_set_style_bg_color(bg, lv_color_hex(0x202040), 0);
  lv_obj_set_style_border_width(bg, 0, 0);

  // Header + FPS
  lv_obj_t *hdr = lv_label_create(lv_screen_active());
  lv_label_set_text(hdr, "ESP32-S3 HMI • LVGL 9");
  lv_obj_align(hdr, LV_ALIGN_TOP_MID, 0, 2);

  label_fps = lv_label_create(lv_screen_active());
  lv_label_set_text(label_fps, "FPS: --");
  lv_obj_align(label_fps, LV_ALIGN_TOP_RIGHT, -6, 2);

  // Tabs oben
  lv_obj_t *tabs = lv_tabview_create(lv_screen_active());
  lv_tabview_set_tab_bar_position(tabs, LV_DIR_TOP);
  lv_tabview_set_tab_bar_size(tabs, 30);
  lv_obj_set_size(tabs, scr_w, scr_h - 20);
  lv_obj_align(tabs, LV_ALIGN_BOTTOM_MID, 0, 0);

  lv_obj_t *tab1 = lv_tabview_add_tab(tabs, "Controls");
  lv_obj_t *tab2 = lv_tabview_add_tab(tabs, "Gauge");
  lv_obj_t *tab3 = lv_tabview_add_tab(tabs, "Chart");

  // --- Controls ---
  lv_obj_t *slider = lv_slider_create(tab1);
  lv_obj_set_width(slider, scr_w - 60);
  lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 20);

  label_slider_val = lv_label_create(tab1);
  lv_label_set_text(label_slider_val, "Value: 0");
  lv_obj_align(label_slider_val, LV_ALIGN_TOP_MID, 0, 45);

  lv_obj_add_event_cb(slider, [](lv_event_t *e)
                      {
    lv_obj_t *sl = (lv_obj_t*)lv_event_get_target(e);
    int v = lv_slider_get_value(sl);
    static char buf[24];
    snprintf(buf, sizeof(buf), "Value: %d", v);
    lv_label_set_text(label_slider_val, buf); }, LV_EVENT_VALUE_CHANGED, nullptr);

  lv_obj_t *sw = lv_switch_create(tab1);
  lv_obj_align(sw, LV_ALIGN_BOTTOM_LEFT, 10, -10);
  lv_obj_t *lbl_sw = lv_label_create(tab1);
  lv_label_set_text(lbl_sw, "Switch");
  lv_obj_align_to(lbl_sw, sw, LV_ALIGN_OUT_RIGHT_MID, 6, 0);

  lv_obj_t *btn = lv_btn_create(tab1);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
  lv_obj_t *lbl_btn = lv_label_create(btn);
  lv_label_set_text(lbl_btn, "Click");
  lv_obj_center(lbl_btn);

  static int clicks = 0;
  lv_obj_add_event_cb(btn, [](lv_event_t *e)
                      {
    clicks++;
    lv_obj_t *b = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t *lbl = lv_obj_get_child(b, 0);
    char t[32]; snprintf(t, sizeof(t), "Click (%d)", clicks);
    lv_label_set_text(lbl, t); }, LV_EVENT_CLICKED, nullptr);

  // Slider-Autoanimation
  lv_timer_create([](lv_timer_t *t)
                  {
    static int v=0, dir=1;
    v += dir * 2;
    if (v >= 100 || v <= 0) dir = -dir;
    lv_obj_t *sl = (lv_obj_t*)lv_timer_get_user_data(t);
    lv_slider_set_value(sl, v, LV_ANIM_OFF);
    static char buf[24]; snprintf(buf, sizeof(buf), "Value: %d", v);
    lv_label_set_text(label_slider_val, buf); }, 60, slider);

  // --- Gauge ---
  arc = lv_arc_create(tab2);
  lv_obj_set_size(arc, 120, 120);
  lv_obj_center(arc);
  lv_arc_set_range(arc, 0, 100);
  lv_arc_set_bg_angles(arc, 135, 45);
  lv_arc_set_value(arc, 0);

  lv_obj_t *arc_label = lv_label_create(tab2);
  lv_label_set_text(arc_label, "0");
  lv_obj_align_to(arc_label, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

  lv_timer_create([](lv_timer_t *)
                  {
    static int v=0, dir=1;
    v += dir * 2;
    if (v >= 100 || v <= 0) dir = -dir;
    lv_arc_set_value(arc, v);
    lv_obj_t *parent = lv_obj_get_parent(arc);
    lv_obj_t *lbl = lv_obj_get_child(parent, 1);
    char tt[8]; snprintf(tt, sizeof(tt), "%d", v);
    lv_label_set_text(lbl, tt); }, 50, nullptr);

  // --- Chart ---
  chart = lv_chart_create(tab3);
  lv_obj_set_size(chart, scr_w - 30, scr_h - 50);
  lv_obj_center(chart);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
  lv_chart_set_point_count(chart, 64);
  ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_ORANGE), LV_CHART_AXIS_PRIMARY_Y);

  lv_timer_create([](lv_timer_t *)
                  {
    static int t=0;
    int y = (int)(50 + 40 * sinf(t * 0.18f));
    lv_chart_set_next_value(chart, ser, y);
    t++; }, 40, nullptr);

  // --- FPS ---
  lv_timer_create([](lv_timer_t *t)
                  {
    static uint32_t last = 0;
    uint32_t now = frame_counter;
    uint32_t fps2 = (now - last) * 2;
    last = now;
    if (label_fps) {
      char buf[24];
      snprintf(buf, sizeof(buf), "FPS: %lu", (unsigned long)fps2);
      lv_label_set_text(label_fps, buf);
    } }, 500, nullptr);

  // Auto Tab Rotation (alle 3 s)
  lv_timer_create([](lv_timer_t *t)
                  {
    lv_obj_t *tabs_obj = (lv_obj_t*)lv_timer_get_user_data(t);
    static uint8_t idx = 0;
    idx = (idx + 1) % 3;
    lv_tabview_set_act(tabs_obj, idx, LV_ANIM_ON); }, 3000, tabs);
}

void setup()
{
  Serial.begin(115200);
  delay(50);

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  if (!gfx->begin(60000000))
  {
    Serial.println("GFX begin failed");
    while (1)
      delay(100);
  }
  gfx->invertDisplay(false);
  gfx->fillScreen(BLACK);
  Serial.println("GFX OK");

  lv_init();
  Serial.println("LVGL OK");

  uint16_t scr_w = gfx->width();
  uint16_t scr_h = gfx->height();

  const uint32_t buf_lines = 32;
  const size_t buf_bytes = scr_w * buf_lines * sizeof(lv_color_t);
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(buf_bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(buf_bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
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

  const esp_timer_create_args_t tcfg = {.callback = &tick_cb, .name = "lv_tick"};
  esp_timer_create(&tcfg, &tick_timer);
  esp_timer_start_periodic(tick_timer, 1000);

  ui_build();

  lv_obj_invalidate(lv_screen_active());
  lv_refr_now(disp);

  Serial.println("UI ready");
}

void loop()
{
  lv_timer_handler();
  delay(5);
}

// ------------------ Below: Other files with similar code snippets ------------------