#include "ui.h"
#include "../pins.h"
#include <Arduino.h>
#include <RotarySwitch.h>
#include <lvgl.h>

// ============================== Encoder ==============================
static RotarySwitch rs(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW);

// ============================== UI Widgets ===========================
static lv_obj_t *scr = nullptr;
static lv_obj_t *lblPos = nullptr;
static lv_obj_t *lblStep = nullptr;
static lv_obj_t *lblClick = nullptr;
static lv_obj_t *btnm = nullptr;
static lv_obj_t *spnTime = nullptr;
static lv_obj_t *spnTemp = nullptr;
static lv_obj_t *roller = nullptr;
static lv_group_t *group = nullptr;

static long lastPosShown = LONG_MIN;

// ============================== Helpers ==============================
static inline void set_pos_label(long p) {
  if (!lblPos)
    return;
  char buf[48];
  snprintf(buf, sizeof(buf), "pos: %ld", p);
  lv_label_set_text(lblPos, buf);
}
static inline void set_step_label(int step) {
  if (!lblStep)
    return;
  char buf[32];
  snprintf(buf, sizeof(buf), "step: %d", step);
  lv_label_set_text(lblStep, buf);
}
static inline void set_click_label(const char *t) {
  if (lblClick)
    lv_label_set_text(lblClick, t);
}
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
static void build_ui() {
  scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "ESP32-S3 HMI FSD – Skeleton");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

  lblPos = lv_label_create(scr);
  lv_label_set_text(lblPos, "pos: 0");
  lv_obj_align(lblPos, LV_ALIGN_TOP_LEFT, 8, 28);

  lblStep = lv_label_create(scr);
  lv_label_set_text(lblStep, "step: 1");
  lv_obj_align_to(lblStep, lblPos, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  lblClick = lv_label_create(scr);
  lv_label_set_text(lblClick, "click: -");
  lv_obj_align_to(lblClick, lblStep, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  static const char *map[] = {"Start", "Stop", "Dry", "\n", "Load", "Save", "Info", ""};
  btnm = lv_btnmatrix_create(scr);
  lv_btnmatrix_set_map(btnm, map);
  lv_obj_set_size(btnm, 200, 90);
  lv_obj_align(btnm, LV_ALIGN_LEFT_MID, 8, 0);
  lv_obj_add_event_cb(btnm, btnm_cb, LV_EVENT_ALL, NULL);

  spnTime = lv_spinbox_create(scr);
  lv_spinbox_set_range(spnTime, 0, 999);
  lv_spinbox_set_digit_format(spnTime, 3, 0);
  lv_spinbox_set_step(spnTime, 1);
  lv_obj_set_size(spnTime, 80, 36);
  lv_obj_align(spnTime, LV_ALIGN_RIGHT_MID, -8, -30);
  lv_obj_add_event_cb(spnTime, spn_time_cb, LV_EVENT_ALL, NULL);

  spnTemp = lv_spinbox_create(scr);
  lv_spinbox_set_range(spnTemp, 0, 150);
  lv_spinbox_set_digit_format(spnTemp, 3, 0);
  lv_spinbox_set_step(spnTemp, 1);
  lv_obj_set_size(spnTemp, 80, 36);
  lv_obj_align_to(spnTemp, spnTime, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
  lv_obj_add_event_cb(spnTemp, spn_temp_cb, LV_EVENT_ALL, NULL);

  roller = lv_roller_create(scr);
  lv_roller_set_options(roller, "PLA\nPETG\nABS\nASA\nTPU\nPA-CF\nPET-CF", LV_ROLLER_MODE_NORMAL);
  lv_obj_set_size(roller, 110, 110);
  lv_obj_align(roller, LV_ALIGN_RIGHT_MID, -8, 40);
  lv_obj_add_event_cb(roller, roller_cb, LV_EVENT_ALL, NULL);

  group = lv_group_get_default();
  if (!group) {
    group = lv_group_create();
    lv_group_set_default(group);
  }
  lv_group_add_obj(group, btnm);
  lv_group_add_obj(group, spnTime);
  lv_group_add_obj(group, spnTemp);
  lv_group_add_obj(group, roller);
  focus_obj(btnm);

  lv_scr_load(scr);
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

  build_ui();
  set_step_label(1);
  set_click_label("click: -");
  Serial.println("UI init done.");
}

// WICHTIG: genau 1x update+poll je Loop
void ui_task() {
  rs.update();
  uint8_t sc, dc, lc = 0;

  // 1) Intent-first – NUR RotarySwitch-Original-APIs
  if (rs.wasDoubleClicked()) {
    set_click_label("DoubleClick");
    Serial.println("DoubleClick");
  } else if (rs.wasLongClicked()) {
    set_click_label("LongClick");
    Serial.println("LongClick");
  } else if (rs.wasShortClicked()) {
    set_click_label("ShortClick");
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
      set_pos_label(newPos);
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