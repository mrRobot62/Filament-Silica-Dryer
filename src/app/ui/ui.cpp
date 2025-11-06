#include "ui.h"
#include "../pins.h"
#include "encoder_input.h"
#include <Arduino.h>
#include <lvgl.h>

static bool s_focus_mode = false; // false=fine(1), true=coarse(15)
static bool s_ui_inited = false;

static lv_obj_t *s_screen = nullptr;
static lv_obj_t *s_lbl_title = nullptr;
static lv_obj_t *s_lbl_pos = nullptr;
static lv_obj_t *s_lbl_step = nullptr;
static lv_obj_t *s_lbl_click = nullptr;

static lv_obj_t *s_btnm = nullptr;
static lv_obj_t *s_spn_time = nullptr;
static lv_obj_t *s_spn_temp = nullptr;
static lv_obj_t *s_roller = nullptr;

static lv_group_t *s_group = nullptr;

static inline void ui_update_pos_label(long pos) {
  if (!s_lbl_pos)
    return;
  char buf[48];
  snprintf(buf, sizeof(buf), "pos: %ld", pos);
  lv_label_set_text(s_lbl_pos, buf);
}
static inline void ui_update_step_label(int step) {
  if (!s_lbl_step)
    return;
  char buf[48];
  snprintf(buf, sizeof(buf), "step: %d", step);
  lv_label_set_text(s_lbl_step, buf);
}
static inline void ui_update_click_label(const char *text) {
  if (!s_lbl_click)
    return;
  lv_label_set_text(s_lbl_click, text);
}
static inline void ui_focus_obj(lv_obj_t *obj) {
  if (!obj)
    return;
  if (!s_group)
    return;
  lv_group_focus_obj(obj);
}

static void btnm_event_cb(lv_event_t *e) {
  lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e); // LVGL9: cast from void*
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    uint16_t id = lv_btnmatrix_get_selected_btn(obj);
    const char *txt = lv_btnmatrix_get_btn_text(obj, id);
    Serial.printf("BTN: %s\n", txt ? txt : "(null)");
  }
}
static void spn_time_event_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    int32_t v = lv_spinbox_get_value((lv_obj_t *)lv_event_get_target(e));
    Serial.printf("TIME: %ld min\n", (long)v);
  }
}
static void spn_temp_event_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    int32_t v = lv_spinbox_get_value((lv_obj_t *)lv_event_get_target(e));
    Serial.printf("TEMP: %ld C\n", (long)v);
  }
}
static void roller_event_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
    char buf[64];
    lv_roller_get_selected_str((lv_obj_t *)lv_event_get_target(e), buf, sizeof(buf));
    Serial.printf("FILAMENT: %s\n", buf);
  }
}

static void build_ui() {
  s_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(s_screen, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(s_screen, LV_OPA_COVER, 0);

  s_lbl_title = lv_label_create(s_screen);
  lv_label_set_text(s_lbl_title, "ESP32-S3 HMI FSD – Skeleton");
  lv_obj_align(s_lbl_title, LV_ALIGN_TOP_MID, 0, 6);

  s_lbl_pos = lv_label_create(s_screen);
  lv_label_set_text(s_lbl_pos, "pos: 0");
  lv_obj_align(s_lbl_pos, LV_ALIGN_TOP_LEFT, 8, 28);

  s_lbl_step = lv_label_create(s_screen);
  lv_label_set_text(s_lbl_step, "step: 1");
  lv_obj_align_to(s_lbl_step, s_lbl_pos, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  s_lbl_click = lv_label_create(s_screen);
  lv_label_set_text(s_lbl_click, "click: -");
  lv_obj_align_to(s_lbl_click, s_lbl_step, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  static const char *map[] = {"Start", "Stop", "Dry", "\n", "Load", "Save", "Info", ""};
  s_btnm = lv_btnmatrix_create(s_screen);
  lv_btnmatrix_set_map(s_btnm, map);
  lv_obj_set_size(s_btnm, 200, 90);
  lv_obj_align(s_btnm, LV_ALIGN_LEFT_MID, 8, 0);
  lv_obj_add_event_cb(s_btnm, btnm_event_cb, LV_EVENT_ALL, NULL);

  s_spn_time = lv_spinbox_create(s_screen);
  lv_spinbox_set_range(s_spn_time, 0, 999);
  lv_obj_set_size(s_spn_time, 80, 36);
  lv_obj_align(s_spn_time, LV_ALIGN_RIGHT_MID, -8, -30);
  lv_obj_add_event_cb(s_spn_time, spn_time_event_cb, LV_EVENT_ALL, NULL);

  s_spn_temp = lv_spinbox_create(s_screen);
  lv_spinbox_set_range(s_spn_temp, 0, 150);
  lv_obj_set_size(s_spn_temp, 80, 36);
  lv_obj_align_to(s_spn_temp, s_spn_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
  lv_obj_add_event_cb(s_spn_temp, spn_temp_event_cb, LV_EVENT_ALL, NULL);

  s_roller = lv_roller_create(s_screen);
  lv_roller_set_options(s_roller, "PLA\nPETG\nABS\nASA\nTPU\nPA-CF\nPET-CF", LV_ROLLER_MODE_NORMAL);
  lv_obj_set_size(s_roller, 110, 110);
  lv_obj_align(s_roller, LV_ALIGN_RIGHT_MID, -8, 40);
  lv_obj_add_event_cb(s_roller, roller_event_cb, LV_EVENT_ALL, NULL);

  s_group = lv_group_get_default();
  if (!s_group) {
    s_group = lv_group_create();
    lv_group_set_default(s_group);
  }
  lv_group_add_obj(s_group, s_btnm);
  lv_group_add_obj(s_group, s_spn_time);
  lv_group_add_obj(s_group, s_spn_temp);
  lv_group_add_obj(s_group, s_roller);
  ui_focus_obj(s_btnm);

  lv_scr_load(s_screen);
}

static inline bool ui_poll_pos_and_update() {
  long p;
  if (encoder_poll_position_change(&p)) {
    ui_update_pos_label(p);
    // Serial.printf("POS %ld\n", p); // <-- debug: sofort sichtbar
    return true;
  }
  return false;
}

static void ui_apply_delta_to_focused(int32_t delta) {
  if (!s_group || delta == 0)
    return;
  lv_obj_t *f = lv_group_get_focused(s_group);
  if (!f)
    return;
  int8_t step = (delta > 0) ? +1 : -1;
  if (f == s_btnm) {
    int cur = lv_btnmatrix_get_selected_btn(s_btnm);
    int next = cur + step;
    if (next < 0)
      next = 0;
    int last = cur;
    while (lv_btnmatrix_get_btn_text(s_btnm, last + 1))
      last++;
    if (next > last)
      next = last;
    lv_btnmatrix_set_selected_btn(s_btnm, next);
  } else if (f == s_spn_time) {
    if (step > 0)
      lv_spinbox_increment(s_spn_time);
    else
      lv_spinbox_decrement(s_spn_time);
  } else if (f == s_spn_temp) {
    if (step > 0)
      lv_spinbox_increment(s_spn_temp);
    else
      lv_spinbox_decrement(s_spn_temp);
  } else if (f == s_roller) {
    int sel = lv_roller_get_selected(s_roller);
    sel += step;
    int cnt = lv_roller_get_option_cnt(s_roller);
    if (sel < 0)
      sel = 0;
    if (sel >= cnt)
      sel = cnt - 1;
    lv_roller_set_selected(s_roller, sel, LV_ANIM_OFF);
  }
}

static inline void ui_handle_encoder_events(const EncEvent &e) {
  // EncEvent e = encoder_poll();

  // Immer loggen, damit wir Sicht haben:ui_handle_encoder_events
  // Serial.printf("ENC d=%ld S=%d D=%d L=%d\n", (long)e.delta, (int)e.shortClick, (int)e.doubleClick,
  // (int)e.longClick);
  // Nur loggen, wenn Bewegung oder ein Click-Event vorliegt
  //   if (e.delta != 0 || e.shortClick || e.doubleClick || e.longClick) {
  //     Serial.printf("ENC d=%ld S=%d D=%d L=%d\n", (long)e.delta, (int)e.shortClick, (int)e.doubleClick,
  //     (int)e.longClick);
  //   }

  if (e.doubleClick) {
    ui_update_click_label("DoubleClick");
    encoder_set_step(5);
    ui_update_step_label(5);
  } else if (e.longClick) {
    ui_update_click_label("LongClick");
    encoder_set_step(15);
    ui_update_step_label(15);
  } else if (e.shortClick) {
    ui_update_click_label("ShortClick");
    if (s_group) {
      lv_obj_t *f = lv_group_get_focused(s_group);
      if (f)
        lv_obj_send_event(f, LV_EVENT_CLICKED, NULL);
    }
    encoder_set_step(1);
    ui_update_step_label(1);
  }

  if (e.delta != 0)
    ui_apply_delta_to_focused(e.delta);
}

void ui_init() {
  if (s_ui_inited)
    return;
  encoder_init();
  encoder_set_reverse(false);
  apply_focus_step();
  build_ui();
  Serial.println("UI init done.");
  Serial.println("Encoder init done.");
  s_ui_inited = true;
}

// void ui_task() {
//   // **WICHTIG**: genau EIN update pro Zyklus
//   encoder_update();

//   (void)ui_poll_pos_and_update();

//   ui_handle_encoder_events();
// }

void ui_task() {
  EncEvent e = encoder_poll();

  // Nur loggen, wenn tatsächlich was passiert ist:
  if (e.delta != 0 || e.shortClick || e.doubleClick || e.longClick) {
    Serial.printf("ENC d=%ld S=%d D=%d L=%d\n", (long)e.delta, (int)e.shortClick, (int)e.doubleClick, (int)e.longClick);
  }

  // Position-Label aktualisieren (optional)
  // ui_update_pos_label(e.pos);

  // Intent-first Aktionen:
  if (e.doubleClick) {
    ui_update_click_label("DoubleClick");
    encoder_set_step(5);
    ui_update_step_label(5);
  } else if (e.longClick) {
    ui_update_click_label("LongClick");
    encoder_set_step(15);
    ui_update_step_label(15);
  } else if (e.shortClick) {
    ui_update_click_label("ShortClick");
    // Fokus-Click
    if (s_group) {
      lv_obj_t *f = lv_group_get_focused(s_group);
      if (f)
        lv_obj_send_event(f, LV_EVENT_CLICKED, NULL);
    }
    encoder_set_step(1);
    ui_update_step_label(1);
    ui_update_pos_label(e.pos);
  }

  if (e.delta != 0)
    ui_apply_delta_to_focused(e.delta);
}

void ui_set_focus(bool focused) {
  s_focus_mode = focused;
  apply_focus_step();
}

void apply_focus_step() {
  if (s_focus_mode) {
    encoder_set_step(15);
    ui_update_step_label(15);
  } else {
    encoder_set_step(1);
    ui_update_step_label(1);
  }
}

// void ui_update_from_encoder() {
//   // encoder_update();
//   (void)ui_poll_pos_and_update();
//   ui_handle_encoder_events();
// }
