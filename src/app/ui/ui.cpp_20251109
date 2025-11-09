#include "ui.h"
#include "../pins.h"
#include <Arduino.h>
#include <RotarySwitch.h>
#include <lvgl.h>

// ============================== Encoder ==============================
static RotarySwitch rs(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW);

// ============================== UI Widgets ===========================
// static lv_obj_t *scr = nullptr;
static lv_obj_t *lblPos = nullptr;
static lv_obj_t *lblStep = nullptr;
static lv_obj_t *lblClick = nullptr;
static lv_obj_t *btnm = nullptr;
static lv_obj_t *spnTime = nullptr;
static lv_obj_t *spnTemp = nullptr;
static lv_obj_t *roller = nullptr;
static lv_group_t *group = nullptr;

//----------------------------- State ------------------------------
// Widgets (sichtbar nur in ui.cpp)
static lv_obj_t *g_scr = nullptr;

static lv_obj_t *g_lblType = nullptr;
static lv_obj_t *g_type_filament = nullptr;

static lv_obj_t *g_lblTime = nullptr;
static lv_obj_t *g_sbHH = nullptr;
static lv_obj_t *g_sbMM = nullptr;
static lv_obj_t *g_sbSS = nullptr;

static lv_obj_t *g_lblTemp = nullptr;
static lv_obj_t *g_sbTemp = nullptr;

static lv_obj_t *g_btnStart = nullptr;
static lv_obj_t *g_btnPause = nullptr;
static lv_obj_t *g_btnCancel = nullptr;

static lv_obj_t *g_log_msg = nullptr;

static lv_group_t *g_group = nullptr;
// --

static long lastPosShown = LONG_MIN;

// ------- Fonts -------
LV_FONT_DECLARE(lv_font_montserrat_6);
LV_FONT_DECLARE(lv_font_montserrat_8);
LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_14);
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_18);
LV_FONT_DECLARE(lv_font_montserrat_20);
LV_FONT_DECLARE(lv_font_montserrat_24);

// ============================== Helpers ==============================

static inline void no_chrome(lv_obj_t *o) {
  lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_border_opa(o, LV_OPA_TRANSP, 0);
  lv_obj_set_style_outline_opa(o, LV_OPA_TRANSP, 0);
  lv_obj_set_style_shadow_opa(o, LV_OPA_TRANSP, 0);
}

// Schöner LED-Style mit Glow
static inline void style_led(lv_obj_t *led, lv_color_t color, lv_coord_t size = 18) {
  // Größe + Form
  lv_obj_set_size(led, size, size);
  lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, LV_PART_MAIN);

  // Rahmen
  lv_obj_set_style_border_width(led, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(led, lv_palette_lighten(LV_PALETTE_GREY, 2), LV_PART_MAIN);
  lv_obj_set_style_border_opa(led, LV_OPA_80, LV_PART_MAIN);

  // Glow / Shadow
  lv_obj_set_style_shadow_width(led, size / 2 + 4, LV_PART_MAIN);
  lv_obj_set_style_shadow_spread(led, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_color(led, color, LV_PART_MAIN);
  lv_obj_set_style_shadow_opa(led, LV_OPA_60, LV_PART_MAIN);

  // Keine Scrollerei
  lv_obj_clear_flag(led, LV_OBJ_FLAG_SCROLLABLE);
}

static inline void set_led_state(lv_obj_t *led, lv_color_t baseColor, bool on) {
  if (on) {
    // ON: volle Farbe, hell, kräftiger Glow
    lv_led_set_color(led, baseColor);
    lv_led_on(led);
    lv_led_set_brightness(led, 255);
    lv_obj_set_style_shadow_opa(led, LV_OPA_80, LV_PART_MAIN);
  } else {
    // OFF: stark abgedunkelte Farbe + praktisch keine Helligkeit
    // Mischung: viel Schwarz, wenig Originalfarbe
    lv_color_t offColor = lv_color_mix(lv_color_black(), baseColor, 20); // 20/255 ≈ 8%

    lv_led_set_color(led, offColor);
    lv_led_off(led);
    lv_led_set_brightness(led, 0);                             // Kern komplett dunkel
    lv_obj_set_style_shadow_opa(led, LV_OPA_10, LV_PART_MAIN); // kaum Glow
  }
}

static inline lv_obj_t *mk_label(lv_obj_t *p, const char *txt, const bool recolor = true) {
  lv_obj_t *l = lv_label_create(p);
  if (recolor)
    lv_label_set_recolor(l, true);
  lv_label_set_text(l, txt);
  no_chrome(l);
  return l;
}

static inline lv_obj_t *mk_circular_info_text(lv_obj_t *p, const char *txt, const int w = 300) {
  lv_obj_t *l = lv_label_create(p);
  lv_label_set_long_mode(l, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR); /*Circular scroll*/
  lv_label_set_recolor(l, true);
  lv_label_set_text(l, txt);
  lv_obj_set_style_text_font(l, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_width(l, w);
  // no_chrome(l);
  return l;
}

static inline lv_obj_t *mk_label45(lv_obj_t *p, const char *txt, const bool recolor = true) {
  lv_obj_t *l = lv_label_create(p);
  if (recolor)
    lv_label_set_recolor(l, true);
  lv_obj_set_style_text_font(l, &lv_font_montserrat_8, LV_PART_MAIN);
  lv_obj_set_style_transform_rotation(l, 450, 0); // 45.0° = 450 (Einheit: 0.1°)
  lv_label_set_text(l, txt);
  lv_obj_align(l, LV_ALIGN_CENTER, 0, 0);
  no_chrome(l);
  return l;
}

static inline lv_obj_t *mk_led(lv_obj_t *p, lv_color_t color = lv_color_hex(0x00ff22), bool state = false,
                               int size = 18) {

  lv_obj_t *led = lv_led_create(p);
  lv_led_set_color(led, color);
  style_led(led, color, size);
  set_led_state(led, color, state); // ON/OFF + Helligkeit + Glow

  return led;
}

static inline lv_obj_t *mk_btn(lv_obj_t *p, const char *txt, lv_coord_t w, lv_coord_t h, const int f_size = 12) {
  lv_obj_t *b = lv_btn_create(p);
  no_chrome(b);
  lv_obj_set_size(b, w, h);
  lv_obj_t *t = lv_label_create(b);
  lv_label_set_text(t, txt);
  lv_obj_center(t);
  // lv_obj_set_style_text_font(t, &lv_font_montserrat_20, 0);
  return b;
}

static inline void dark_screen(lv_obj_t *s) {
  lv_obj_set_style_bg_color(s, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(s, LV_OPA_COVER, 0);
}

// static inline void set_pos_label(long p) {
//   if (!lblPos)
//     return;
//   char buf[48];
//   snprintf(buf, sizeof(buf), "pos: %ld", p);
//   lv_label_set_text(lblPos, buf);
// }
// static inline void set_step_label(int step) {
//   if (!lblStep)
//     return;
//   char buf[32];
//   snprintf(buf, sizeof(buf), "step: %d", step);
//   lv_label_set_text(lblStep, buf);
// }

// static inline void set_click_label(const char *t) {
//   if (lblClick)
//     lv_label_set_text(lblClick, t);
// }

static inline void focus_obj(lv_obj_t *o) {
  if (group && o)
    lv_group_focus_obj(o);
}

// Button-matrix event
static void btnm_cb(lv_event_t *e) {
  lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    uint16_t id = lv_btnmatrix_get_selected_btn(obj);
    const char *txt = lv_btnmatrix_get_btn_text(obj, id);
    Serial.printf("BTN: %s\n", txt ? txt : "(null)");
  }
}
static void spn_time_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    int32_t v = lv_spinbox_get_value((lv_obj_t *)lv_event_get_target(e));
    Serial.printf("TIME: %ld\n", (long)v);
  }
}
static void spn_temp_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    int32_t v = lv_spinbox_get_value((lv_obj_t *)lv_event_get_target(e));
    Serial.printf("TEMP: %ld\n", (long)v);
  }
}
static void roller_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    char buf[64];
    lv_roller_get_selected_str((lv_obj_t *)lv_event_get_target(e), buf, sizeof(buf));
    Serial.printf("FILAMENT: %s\n", buf);
  }
}

// Apply +/-1 step to focused widget
static void apply_step_to_focused(int8_t step) {
  if (!group)
    return;
  lv_obj_t *f = lv_group_get_focused(group);
  if (!f)
    return;

  if (f == btnm) {
    int cur = lv_btnmatrix_get_selected_btn(btnm);
    int next = cur + step;
    if (next < 0)
      next = 0;
    int last = cur;
    while (lv_btnmatrix_get_btn_text(btnm, last + 1))
      last++;
    if (next > last)
      next = last;
    lv_btnmatrix_set_selected_btn(btnm, next);
  } else if (f == spnTime) {
    if (step > 0)
      lv_spinbox_increment(spnTime);
    else
      lv_spinbox_decrement(spnTime);
  } else if (f == spnTemp) {
    if (step > 0)
      lv_spinbox_increment(spnTemp);
    else
      lv_spinbox_decrement(spnTemp);
  } else if (f == roller) {
    int sel = lv_roller_get_selected(roller);
    sel += step;
    int cnt = lv_roller_get_option_cnt(roller);
    if (sel < 0)
      sel = 0;
    if (sel >= cnt)
      sel = cnt - 1;
    lv_roller_set_selected(roller, sel, LV_ANIM_OFF);
  }
}

// ============================== Build UI =============================
void build_widgets() {
  //   g_scr = lv_obj_create(NULL);
  //   lv_obj_set_style_bg_opa(g_scr, LV_OPA_COVER, 0);

  //   lv_obj_t *title = lv_label_create(g_scr);
  //   lv_label_set_text(title, "ESP32-S3 HMI FSD – Skeleton");
  //   lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

  //   lblPos = lv_label_create(g_scr);
  //   lv_label_set_text(lblPos, "pos: 0");
  //   lv_obj_align(lblPos, LV_ALIGN_TOP_LEFT, 8, 28);

  //   lblStep = lv_label_create(g_scr);
  //   lv_label_set_text(lblStep, "step: 1");
  //   lv_obj_align_to(lblStep, lblPos, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  //   lblClick = lv_label_create(g_scr);
  //   lv_label_set_text(lblClick, "click: -");
  //   lv_obj_align_to(lblClick, lblStep, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  //   static const char *map[] = {"Start", "Stop", "Dry", "\n", "Load", "Save", "Info", ""};
  //   btnm = lv_btnmatrix_create(g_scr);
  //   lv_btnmatrix_set_map(btnm, map);
  //   lv_obj_set_size(btnm, 200, 90);
  //   lv_obj_align(btnm, LV_ALIGN_LEFT_MID, 8, 0);
  //   lv_obj_add_event_cb(btnm, btnm_cb, LV_EVENT_ALL, NULL);

  //   spnTime = lv_spinbox_create(g_scr);
  //   lv_spinbox_set_range(spnTime, 0, 999);
  //   lv_spinbox_set_digit_format(spnTime, 3, 0);
  //   lv_spinbox_set_step(spnTime, 1);
  //   lv_obj_set_size(spnTime, 80, 36);
  //   lv_obj_align(spnTime, LV_ALIGN_RIGHT_MID, -8, -30);
  //   lv_obj_add_event_cb(spnTime, spn_time_cb, LV_EVENT_ALL, NULL);

  //   spnTemp = lv_spinbox_create(g_scr);
  //   lv_spinbox_set_range(spnTemp, 0, 150);
  //   lv_spinbox_set_digit_format(spnTemp, 3, 0);
  //   lv_spinbox_set_step(spnTemp, 1);
  //   lv_obj_set_size(spnTemp, 80, 36);
  //   lv_obj_align_to(spnTemp, spnTime, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
  //   lv_obj_add_event_cb(spnTemp, spn_temp_cb, LV_EVENT_ALL, NULL);

  //   roller = lv_roller_create(g_scr);
  //   lv_roller_set_options(roller, "PLA\nPETG\nABS\nASA\nTPU\nPA-CF\nPET-CF", LV_ROLLER_MODE_NORMAL);
  //   lv_obj_set_size(roller, 110, 110);
  //   lv_obj_align(roller, LV_ALIGN_RIGHT_MID, -8, 40);
  //   lv_obj_add_event_cb(roller, roller_cb, LV_EVENT_ALL, NULL);

  //   group = lv_group_get_default();
  //   if (!group) {
  //     group = lv_group_create();
  //     lv_group_set_default(group);
  //   }
  //   lv_group_add_obj(group, btnm);
  //   lv_group_add_obj(group, spnTime);
  //   lv_group_add_obj(group, spnTemp);
  //   lv_group_add_obj(group, roller);
  //   focus_obj(btnm);

  //   lv_scr_load(g_scr);
  // }

  // // ============================== Public API ===========================
  // void ui_init() {
  //   // RotarySwitch: klare, reproduzierbare Timings
  //   rs.begin(true);
  //   rs.setDebounceMs(15);
  //   rs.setDoubleClickWindowMs(300);
  //   rs.setLongClickMs(1500);
  //   rs.setStep(1);
  //   rs.setReverseDirection(false); // CW = positiv
  //   rs.setButtonActiveLow(false);  // <-- NEU: pressed = HIGH (so wie in deinem Test)

  //   build_widgets();
  //   set_step_label(1);
  //   set_click_label("click: -");
  //   Serial.println("UI init done.");
  // }

  // // WICHTIG: genau 1x update+poll je Loop
  // void ui_task() {
  //   rs.update();
  //   uint8_t sc, dc, lc = 0;

  //   // 1) Intent-first – NUR RotarySwitch-Original-APIs
  //   if (rs.wasDoubleClicked()) {
  //     set_click_label("DoubleClick");
  //     Serial.println("DoubleClick");
  //   } else if (rs.wasLongClicked()) {
  //     set_click_label("LongClick");
  //     Serial.println("LongClick");
  //   } else if (rs.wasShortClicked()) {
  //     set_click_label("ShortClick");
  //     Serial.println("ShortClick");
  //     if (group) {
  //       lv_obj_t *f = lv_group_get_focused(group);
  //       if (f)
  //         lv_obj_send_event(f, LV_EVENT_CLICKED, NULL);
  //     }
  //   }

  //   // 2) Bewegung – NUR pollPositionChange (deine saubere Quelle)
  //   long newPos;
  //   if (rs.pollPositionChange(newPos)) {
  //     if (newPos != lastPosShown) {
  //       set_pos_label(newPos);
  //       lastPosShown = newPos;
  //     }
  //     // Auf fokussiertes Widget pro "Click" reagieren
  //     // (pollPositionChange liefert bereits in Anzeige-Einheiten -> +/-1 pro Rastung bei step=1)
  //     long diff = newPos - lastPosShown; // lastPosShown wurde oben gesetzt, hier diff auf Basis step=1 benutzen
  //     // Da oben lastPosShown aktualisiert wurde, nutzen wir signum:
  //     int8_t step = (newPos > lastPosShown) ? +1 : -1;
  //     // Falls du mehrere Schritte pro poll bekommst, wiederhole entsprechend:
  //     apply_step_to_focused(step);
  //     Serial.printf("ENC: %d\n", (int)newPos);
  //   }
  //   // Serial.printf("ENC: %d, SC=%d, DC=%d, LC=%d\n", newPos, sc, dc, lc);

  //   // Optional: Low-level Edges
  //   // if (rs.wasPressed())  Serial.println("PRESS");
  //   // if (rs.wasReleased()) Serial.println("RELEASE");

  // ---------- new --------------
  // Root Screen
  g_scr = lv_obj_create(NULL);
  dark_screen(g_scr);
  lv_obj_set_style_pad_all(g_scr, 0, 0);
  no_chrome(g_scr);

  // Display-Infos (nur fürs Debug)
  lv_disp_t *d = lv_disp_get_default();
  const int W = lv_disp_get_hor_res(d);
  const int H = lv_disp_get_ver_res(d);

  const int PAD_X = 8; // Rand links
  const int PAD_Y = 3; // Rand oben
  const int INPUT_X = 55;
  //   const int RIGHT_W = 86; // rechte Button-Spalte
  //   const int RIGHT_X = W - PAD_X - RIGHT_W;
  const int INPUT_HEIGHT = 40;
  const int INPUT_HEIGHT_DIST = 5;

  const int LBL_ROW_1 = PAD_Y + 10;
  const int LBL_ROW_2 = LBL_ROW_1 + 45;
  const int LBL_ROW_3 = LBL_ROW_2 + 45;

  const int TYPE_ROLLER_W = 160;
  const int TYPE_ROLLER_H = INPUT_HEIGHT;
  const int TYPE_INPUT_Y = PAD_Y;

  const int TIME_SP_W = 35;
  const int TIME_SP_H = INPUT_HEIGHT;
  const int TIME_INPUT_Y = TYPE_INPUT_Y + INPUT_HEIGHT + INPUT_HEIGHT_DIST;
  const int TIME_INPUT_HH = INPUT_X;
  const int TIME_INPUT_MM = TIME_INPUT_HH + TIME_SP_W + 15;
  const int TIME_INPUT_SS = TIME_INPUT_MM + TIME_SP_W + 15;

  const int TEMP_SP_W = 50;
  const int TEMP_SP_H = INPUT_HEIGHT;
  const int TEMP_INPUT_Y = TIME_INPUT_Y + INPUT_HEIGHT + INPUT_HEIGHT_DIST;

  const int RIGHT_BTN_CONTAINER_X = 240;
  const int RIGHT_BTN_CONTAINER_Y = PAD_Y;
  const int BTN_W = 70;
  const int BTN_H = 25;
  const int BTN_PADDING = 5;

  const int BTN_ROW1_Y = RIGHT_BTN_CONTAINER_Y;
  const int BTN_ROW2_Y = BTN_ROW1_Y + BTN_H + BTN_PADDING;
  const int BTN_ROW3_Y = BTN_ROW2_Y + BTN_H + BTN_PADDING;

  const int LOGBAR_W = LCD_WIDTH - (PAD_X * 2);
  const int LOGBAR_H = 34;
  const int LOGBAR_Y = LCD_HEIGHT - LOGBAR_H;

  const int LED_LBL_X = 140;
  const int LED_LBL_X_OFFSET = 30;
  const int LED_LBL_Y = LBL_ROW_3 - 8;

  ; // TYPE
  // g_lblType = mk_label(g_scr, "#FFFFFF TYPE:");
  g_lblType = mk_label(g_scr, "TYPE:");
  lv_obj_set_pos(g_lblType, PAD_X, LBL_ROW_1);

  g_type_filament = lv_roller_create(g_scr);
  lv_roller_set_options(g_type_filament, "PLA\nPLA+\nPETG\nABS\nASA\nTPU\nPA-CF\nPET-CF", LV_ROLLER_MODE_NORMAL);
  no_chrome(g_type_filament);
  lv_obj_set_size(g_type_filament, TYPE_ROLLER_W, TYPE_ROLLER_H);
  lv_obj_set_pos(g_type_filament, INPUT_X, TYPE_INPUT_Y);

  // TIME
  // g_lblTime = mk_label(g_scr, "#FFFFFF TIME:");
  g_lblTime = mk_label(g_scr, "TIME:");
  lv_obj_set_pos(g_lblTime, PAD_X, LBL_ROW_2);

  auto mk_sb2 = [&](int x, int y, int w, int digits = 2) -> lv_obj_t * {
    lv_obj_t *sb = lv_spinbox_create(g_scr);
    lv_spinbox_set_digit_format(sb, digits, 0);
    lv_spinbox_set_step(sb, 1);
    lv_obj_set_style_text_font(sb, &lv_font_montserrat_18, LV_PART_MAIN);
    no_chrome(sb);
    lv_obj_set_size(sb, w, TIME_SP_H);
    lv_obj_set_pos(sb, x, y);
    return sb;
  };

  lv_obj_t *dp1 = mk_label(g_scr, "#ffffff :#");
  lv_obj_t *dp2 = mk_label(g_scr, "#ffffff :#");
  // HH
  g_sbHH = mk_sb2(TIME_INPUT_HH, TIME_INPUT_Y, TIME_SP_W);
  lv_spinbox_set_range(g_sbHH, 0, 99);
  /* : */ lv_obj_set_pos(dp1, TIME_INPUT_HH + TIME_SP_W + 5, LBL_ROW_2);

  // MM
  g_sbMM = mk_sb2(TIME_INPUT_MM, TIME_INPUT_Y, TIME_SP_W);
  lv_spinbox_set_range(g_sbMM, 0, 59);
  /* : */ lv_obj_set_pos(dp2, TIME_INPUT_MM + TIME_SP_W + 5, LBL_ROW_2);

  // SS
  g_sbSS = mk_sb2(TIME_INPUT_SS, TIME_INPUT_Y, TIME_SP_W);
  lv_spinbox_set_range(g_sbSS, 0, 59);

  // TEMP
  //  g_lblTemp = mk_label(g_scr, "#FFFFFF TEMP:#");
  g_lblTemp = mk_label(g_scr, "TEMP:");
  lv_obj_t *tUnit = mk_label(g_scr, "#FFFFFF °C#");

  g_sbTemp = mk_sb2(INPUT_X, TEMP_INPUT_Y, TEMP_SP_W, 3);
  lv_obj_set_pos(g_lblTemp, PAD_X, LBL_ROW_3);
  lv_spinbox_set_range(g_sbTemp, 0, 120);
  lv_obj_set_pos(tUnit, INPUT_X + TEMP_SP_W + 5, LBL_ROW_3 - 4);

  // ==================== Rechte Spalte: START / PAUSE / CANCEL ====================
  g_btnStart = mk_btn(g_scr, "START", BTN_W, BTN_H);
  g_btnPause = mk_btn(g_scr, "PAUSE", BTN_W, BTN_H);
  g_btnCancel = mk_btn(g_scr, "CANCEL", BTN_W, BTN_H);

  lv_obj_set_pos(g_btnStart, RIGHT_BTN_CONTAINER_X, BTN_ROW1_Y);
  lv_obj_set_pos(g_btnPause, RIGHT_BTN_CONTAINER_X, BTN_ROW2_Y);
  lv_obj_set_pos(g_btnCancel, RIGHT_BTN_CONTAINER_X, BTN_ROW3_Y);

  // ==================== LEDs
  //--- Labels
  lv_obj_t *led_lbl_1 = mk_label(g_scr, "F1");
  lv_obj_t *led_lbl_2 = mk_label(g_scr, "F2");
  lv_obj_t *led_lbl_3 = mk_label(g_scr, "MTR");
  lv_obj_t *led_lbl_4 = mk_label(g_scr, "HTR");
  lv_obj_t *led_lbl_5 = mk_label(g_scr, "DOOR");

  lv_obj_t *led_fan1 = mk_led(g_scr, lv_color_hex(0x00ff22), true, 20);
  lv_obj_t *led_fan2 = mk_led(g_scr, lv_color_hex(0x00ff22), false, 20);
  lv_obj_t *led_motor = mk_led(g_scr, lv_color_hex(0x00ff22), false, 20);
  lv_obj_t *led_heater = mk_led(g_scr, lv_color_hex(0x00ff22), false, 20);
  lv_obj_t *led_door = mk_led(g_scr, lv_color_hex(0xff0000), true, 20);

  lv_obj_set_pos(led_lbl_1, LED_LBL_X, LED_LBL_Y);
  lv_obj_set_pos(led_lbl_2, LED_LBL_X + LED_LBL_X_OFFSET, LED_LBL_Y);
  lv_obj_set_pos(led_lbl_3, LED_LBL_X + LED_LBL_X_OFFSET * 2, LED_LBL_Y);
  lv_obj_set_pos(led_lbl_4, LED_LBL_X + LED_LBL_X_OFFSET * 3, LED_LBL_Y);
  lv_obj_set_pos(led_lbl_5, LED_LBL_X + LED_LBL_X_OFFSET * 4, LED_LBL_Y);

  lv_obj_set_pos(led_fan1, LED_LBL_X, LED_LBL_Y + 15);
  lv_obj_set_pos(led_fan2, LED_LBL_X + LED_LBL_X_OFFSET, LED_LBL_Y + 15);
  lv_obj_set_pos(led_motor, LED_LBL_X + LED_LBL_X_OFFSET * 2, LED_LBL_Y + 15);
  lv_obj_set_pos(led_heater, LED_LBL_X + LED_LBL_X_OFFSET * 3, LED_LBL_Y + 15);
  lv_obj_set_pos(led_door, LED_LBL_X + LED_LBL_X_OFFSET * 4, LED_LBL_Y + 15);

  // ==================== LogBar unten ====================
  lv_obj_t *logbar = lv_obj_create(g_scr);
  no_chrome(logbar);
  lv_obj_set_size(logbar, LOGBAR_W, LOGBAR_H);
  lv_obj_set_pos(logbar, PAD_X, LOGBAR_Y);
  lv_obj_set_style_bg_opa(logbar, LV_OPA_20, 0);
  lv_obj_set_style_bg_color(logbar, lv_palette_lighten(LV_PALETTE_NONE, 3), 0);
  lv_obj_set_style_pad_hor(logbar, 0, 0);
  lv_obj_set_style_pad_ver(logbar, 1, 0);
  g_log_msg = mk_circular_info_text(logbar, "#00bd19ff *** Filament-Silicat Dryer V0.1 (c) 2025 LunaX *** #", 300);
  lv_obj_align(g_log_msg, LV_ALIGN_CENTER, 2, 0);

  // Fokusgruppe
  g_group = lv_group_get_default();
  if (!g_group) {
    g_group = lv_group_create();
    lv_group_set_default(g_group);
  }

  lv_group_add_obj(g_group, g_type_filament);
  lv_group_add_obj(g_group, g_sbHH);
  lv_group_add_obj(g_group, g_sbMM);
  lv_group_add_obj(g_group, g_sbSS);
  lv_group_add_obj(g_group, g_sbTemp);
  lv_group_add_obj(g_group, g_btnStart);
  lv_group_add_obj(g_group, g_btnPause);
  lv_group_add_obj(g_group, g_btnCancel);

  lv_group_focus_obj(g_type_filament);
  lv_scr_load(g_scr);

  Serial.println("build_widgets(): done");
}

// ============================== Public API ===========================
void ui_init() {
  // RotarySwitch: klare, reproduzierbare Timings
  rs.begin(true);
  rs.setDebounceMs(15);
  rs.setDoubleClickWindowMs(300);
  rs.setLongClickMs(1500);
  rs.setStep(1);
  rs.setReverseDirection(false); // CW = positiv
  rs.setButtonActiveLow(false);  // <-- NEU: pressed = HIGH (so wie in deinem Test)

  build_widgets();
  Serial.println("UI init done.");
}

// WICHTIG: genau 1x update+poll je Loop
void ui_task() {
  lv_timer_handler();

  rs.update();
  uint8_t sc, dc, lc = 0;

  // 1) Intent-first – NUR RotarySwitch-Original-APIs
  if (rs.wasDoubleClicked()) {
    Serial.println("DoubleClick");
  } else if (rs.wasLongClicked()) {
    Serial.println("LongClick");
  } else if (rs.wasShortClicked()) {
    Serial.println("ShortClick");
    if (group) {
      lv_obj_t *f = lv_group_get_focused(group);
      if (f)
        lv_obj_send_event(f, LV_EVENT_CLICKED, NULL);
    }
  }

  // 2) Bewegung – NUR pollPositionChange (deine saubere Quelle)
  long newPos;
  if (rs.pollPositionChange(newPos)) {
    if (newPos != lastPosShown) {
      lastPosShown = newPos;
    }
    // Auf fokussiertes Widget pro "Click" reagieren
    // (pollPositionChange liefert bereits in Anzeige-Einheiten -> +/-1 pro Rastung bei step=1)
    long diff = newPos - lastPosShown; // lastPosShown wurde oben gesetzt, hier diff auf Basis step=1 benutzen
    // Da oben lastPosShown aktualisiert wurde, nutzen wir signum:
    int8_t step = (newPos > lastPosShown) ? +1 : -1;
    // Falls du mehrere Schritte pro poll bekommst, wiederhole entsprechend:
    apply_step_to_focused(step);
    Serial.printf("ENC: %d\n", (int)newPos);
  }
  // Serial.printf("ENC: %d, SC=%d, DC=%d, LC=%d\n", newPos, sc, dc, lc);

  // Optional: Low-level Edges
  // if (rs.wasPressed())  Serial.println("PRESS");
  // if (rs.wasReleased()) Serial.println("RELEASE");
}