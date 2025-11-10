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

#include "../pins.h"
#include "RotarySwitch.h"
#include "lvgl.h"

// Screen externs
extern lv_obj_t *ui_Screen1;

// Widget externs
extern lv_obj_t *ui_rolType;
extern lv_obj_t *ui_spnTimeHH;
extern lv_obj_t *ui_spnTimeMM;

// Function declarations for event callbacks
void ui_init(void);
void on_click_rolType(lv_event_t *e);
void on_change_rolType(lv_event_t *e);
void on_click_btnStart(lv_event_t *e);
void on_click_btnCancel(lv_event_t *e);
void on_click_timeHH(lv_event_t *e);
void on_change_timeHH(lv_event_t *e);
void on_click_timeMM(lv_event_t *e);
void on_change_timeMM(lv_event_t *e);
void on_click_timeSS(lv_event_t *e);
void on_change_timeSS(lv_event_t *e);
