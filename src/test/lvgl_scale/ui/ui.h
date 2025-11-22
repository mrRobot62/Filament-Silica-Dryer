#pragma once
#include "../logging/log_ui.h"
#include "../pins.h"
#include <lvgl.h>

// ------------------------------------------------------------
// Position-Konstanten
//
//
// Screen-Aufteilung
// Col1     Col2    Col3    Col4
// 0-79     80-159  160-239 240-319
//
// ------------------------------------------------------------
#define COL_W 80 // 4x 80 = 320px
#define PADDING 5
#define DEFAULT_H 25
#define FILAMENT_X 5
#define FILAMENT_Y PADDING
#define FILAMENT_W int(COL_W * 1.0)
#define FILAMENT_H DEFAULT_H

#define TIME_SCALE_W 140
#define TIME_CENTER 140
#define TIME_POS_Y PADDING
#define TIME_POS_X 160 - (TIME_SCALE_W / 2)

#define TIME_SCALE_H TIME_SCALE_W
#define TIME_SCALE_X
#define TIME_SCALE_Y FILAMENT_Y + FILAMENT_H + PADDING
#define TIME_NEEDLE_LEN_M (TIME_SCALE_W / 2) - (TIME_SCALE_W / 100 * 25) // Minuten-Nadel-Länge
#define TIME_NEEDLE_LEN_H (TIME_SCALE_W / 2) - (TIME_SCALE_W / 100 * 30) // Stunde-Nadel-Länge

#define TEMP_LINE_W 15
#define TEMP_LINE_X_S_PAD 37                                        // Abstand der SET-Linie zur Skala
#define TEMP_LINE_X_C_PAD TEMP_LINE_X_S_PAD + TEMP_LINE_W + PADDING // Abstand der ACTUAL-Linie zur Skala
#define TEMP_MAX 120
#define TEMP_MIN 20
#define TEMP_Y_START 120
#define H_LINE_X TIME_SCALE_Y + TIME_SCALE_H + PADDING

// ------------------------------------------------------------
// Fonts (müssen in lv_conf.h aktiviert sein)
// ------------------------------------------------------------
LV_FONT_DECLARE(lv_font_montserrat_8);
LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_18);
LV_FONT_DECLARE(lv_font_montserrat_20);

// ------------------------------------------------------------
// Filament-Informationen & Presets
// ------------------------------------------------------------
static const uint8_t MAX_PRESETS = 30;

struct FilamentPreset {
  String filament; // Filament name
  float dryTemp;   // Drying temperature in °C
  uint8_t timeH;   // Drying time hours
  uint8_t timeM;   // Drying time minutes
  bool rotaryOn;   // Rotary enabled

  FilamentPreset() : filament(""), dryTemp(0.0f), timeH(0), timeM(0), rotaryOn(false) {}

  FilamentPreset(const String &name, float temp, uint8_t h, uint8_t m, bool rotary = false)
      : filament(name), dryTemp(temp), timeH(h), timeM(m), rotaryOn(rotary) {}
};

// Preset storage
static FilamentPreset g_presets[MAX_PRESETS];
static uint8_t g_presetCount = 0;
static uint8_t g_default_preset_id = 5;
// static uint32_t hours, minutes;
// static lv_timer_t *countdown_timer;
// static int32_t countdown_minutes = 0; // Restzeit in Minuten

struct UiContext {
  lv_obj_t *screen = nullptr;

  // Input- / Buttons
  lv_obj_t *scaleTime = nullptr;
  lv_obj_t *scaleTemp = nullptr;
  lv_obj_t *lineTempSet = nullptr; // SET-Temperatur
  lv_obj_t *lineTempCur = nullptr; // ACTUAL-Temperatur
  lv_obj_t *rollerFilament = nullptr;
  lv_obj_t *btnStartStop = nullptr;
  lv_obj_t *needleHH = nullptr;
  lv_obj_t *needleMM = nullptr;

  // LEDs zur Anzeige von Status
  lv_obj_t *ledFan230V = nullptr;
  lv_obj_t *ledFan230V_L = nullptr;
  lv_obj_t *ledFan12V = nullptr;
  lv_obj_t *ledMotor = nullptr;
  lv_obj_t *ledDoor = nullptr;

  // Fokus-Gruppe
  lv_group_t *group = nullptr;

  //
  int32_t countdown_minutes = 0; // Restzeit in Minuten
  lv_timer_t *countdown_timer = nullptr;
  int32_t hours, minutes;
};

static UiContext g_ui;
// für beide Zeiger eigene Punkt-Arrays
static lv_point_precise_t g_minute_hand_points[2];
static lv_point_precise_t g_hour_hand_points[2];
enum class UIFocusTarget { None, Time, TempSoll, Filament, StartStop };

void ui_init(void);
void on_click_btnStartStop(lv_event_t *e);
void ui_update_roller_focus_style(UiContext *ui, bool edit);
