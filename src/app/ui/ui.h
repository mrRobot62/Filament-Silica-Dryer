/*
 * FILE: ui.h
 * AUTHOR: LVGL Visual Architect (Generated)
 * DESCRIPTION: Header file for the generated UI.
 *              Declares UI elements and the main initialization function.
 */
#pragma once
#include "../pins.h"
#include <RotarySwitch.h>
#include <lvgl.h>

/**
 * Bundle von widgets die über ui_event genutzt werden können
 */
struct UiContext {
  // Display
  lv_obj_t *screen = nullptr;

  // Eingabe-Widgets
  lv_group_t *group = nullptr;
  lv_obj_t *rollerType = nullptr;
  lv_obj_t *spnTimeHH = nullptr;
  lv_obj_t *spnTimeMM = nullptr;
  lv_obj_t *spnTimeSS = nullptr;
  lv_obj_t *spnTemp = nullptr;
  lv_obj_t *spnTempSoll = nullptr; // read-only nur Anzeige
  lv_obj_t *arcTempSoll = nullptr;

  // Buttons
  lv_obj_t *btnStart = nullptr;
  lv_obj_t *btnCancel = nullptr;
  lv_obj_t *lblInfoMsg = nullptr;

  // LED-Marker
  lv_obj_t *ledFan1 = nullptr;   // Fan230V - Fast
  lv_obj_t *ledFan1_L = nullptr; // Fan230V - Slow
  lv_obj_t *ledFan12V = nullptr; // Fan12V
  lv_obj_t *ledMotor = nullptr;  // Motor
  lv_obj_t *ledDoor = nullptr;   // Door-Indikator

  // Einfache Labels
  lv_obj_t *lblType = nullptr;
  lv_obj_t *lblTime = nullptr;
  lv_obj_t *lblTimeDP1 = nullptr;
  lv_obj_t *lblTimeDP2 = nullptr;
  lv_obj_t *lblTemp = nullptr;
  lv_obj_t *lblBtnStart = nullptr;
  lv_obj_t *lblBtnCancel = nullptr;

  //
  int selectedFilamentIndex = 0;
};

enum class UiFocusTarget {
  None,
  TimeHH,
  TimeMM,
  TimeSS,
  Temp,
  TypeRoller,
  BtnStart,
  BtnPause,
  BtnCancel,
};

// Screen externs
extern lv_obj_t *ui_main_screen;
extern UiContext *ui_get();

// Widget externs
extern lv_obj_t *ui_rolType;
extern lv_obj_t *ui_spnTimeHH;
extern lv_obj_t *ui_spnTimeMM;
extern lv_obj_t *ui_spnTimeSS;
extern lv_obj_t *ui_spnTemp;
extern lv_obj_t *ui_spnTempSoll;

// Function declarations for event callbacks
void ui_init(void);
void on_click_btnStart(lv_event_t *e);
void on_click_btnCancel(lv_event_t *e);
void ui_update_roller_focus_style(UiContext *ui, bool edit);

// void on_click_rolType(lv_event_t *e);
// void on_change_rolType(lv_event_t *e);
// void on_click_timeHH(lv_event_t *e);
// void on_change_timeHH(lv_event_t *e);
// void on_click_timeMM(lv_event_t *e);
// void on_change_timeMM(lv_event_t *e);
// void on_click_timeSS(lv_event_t *e);
// void on_change_timeSS(lv_event_t *e);
