/*
 * FILE: ui.c
 * AUTHOR: LVGL Visual Architect (Generated)
 * DESCRIPTION: Implementation of the UI. This file should NOT be modified
 *              by the user, as it will be regenerated.
 */

#include "ui.h"
#include "../logging/log_ui.h"

#define INPUT_PADDING 5
#define LBL_LEFT_POS_X 5

#define INPUT_POS_X0 50
#define INPUT_POS_X1 100
#define INPUT_POS_X2 150

#define INPUT_ROW0_Y 5
#define INPUT_ROW1_Y 40 + INPUT_PADDING
#define INPUT_ROW2_Y INPUT_ROW1_Y + 35 + INPUT_PADDING
#define INPUT_ROW0_Y 5

#define INPUT_LABLE_W 50
#define INPUT_LABLE_H 30

#define ROLLER_TYPE_X INPUT_ROW0_X
#define ROLLER_TYPE_Y INPUT_ROW0_Y
#define ROLLER_TYPE_W 150
#define ROLLER_TYPE_H 35

#define SPIN_TIME_W 40
#define SPIN_TIME_H 35
#define SPIN_TEMP_W 150
#define SPIN_TEMP_H SPIN_TIME_H

#define BTN_W 80
#define BTN_H 25
#define BTN_X 230
#define BTN_Y 10

#define LBL_ROW_Y1 INPUT_ROW0_Y + 10
#define LBL_ROW_Y2 INPUT_ROW1_Y + 10
#define LBL_ROW_Y3 INPUT_ROW2_Y + 10

// ------------------------------------------------------------
// Fonts (müssen in lv_conf.h aktiviert sein)
// ------------------------------------------------------------
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_18);
LV_FONT_DECLARE(lv_font_montserrat_20);

////////////////////////////////////////
// EVENT FORWARD DECLARATIONS
////////////////////////////////////////
void on_rolType_Clicked(lv_event_t *e);
void on_rolType_Focused(lv_event_t *e);
void on_rolType_ValueChanged(lv_event_t *e);
void on_btnStart_Clicked(lv_event_t *e);
void on_btnStart_Focused(lv_event_t *e);
void on_btnCancel_Focused(lv_event_t *e);
void on_btnCancel_Clicked(lv_event_t *e);
void on_spnTimeHH_Clicked(lv_event_t *e);
void on_spnTimeHH_ValueChanged(lv_event_t *e);
void on_spnTimeMM_Clicked(lv_event_t *e);
void on_spnTimeMM_ValueChanged(lv_event_t *e);
void on_spnTimeSS_Clicked(lv_event_t *e);
void on_spnTimeSS_ValueChanged(lv_event_t *e);
void on_spnTemp_Clicked(lv_event_t *e);
void on_spnTemp_ValueChanged(lv_event_t *e);

/////////////////////
// ASSETS
/////////////////////
// LV_IMG_DECLARE(...)

/////////////////////
// SCREEN DEFINITIONS
/////////////////////
// lv_obj_t *ui_main_screen;

/////////////////////
// WIDGET DEFINITIONS
/////////////////////
lv_obj_t *ui_lblType;
lv_obj_t *ui_rolType;
lv_obj_t *ui_btnStart;
lv_obj_t *ui_btnCancel;
lv_obj_t *ui_lblTime;
lv_obj_t *ui_spnTimeHH;
lv_obj_t *ui_lblDP;
lv_obj_t *ui_spnTimeMM;
lv_obj_t *ui_lblDP2;
lv_obj_t *ui_spnTimeSS;
lv_obj_t *ui_lblTemp;
lv_obj_t *ui_spnTemp;
lv_obj_t *ui_lblInfoMsg;
lv_obj_t *ui_imgDorr;
lv_obj_t *ui_Image2;
lv_obj_t *ui_Image3;
lv_obj_t *ui_imgMotor;

static UiContext g_ui;

UiContext *ui_get() {
  return &g_ui;
}

//--------------------------------------
// Färben von Widgets bezogen auf Focus
//--------------------------------------
// Spinbox: Text im Fokus blau, sonst weiß. Cursor im Fokus heller.
static inline void style_focus_spinbox(lv_obj_t *sb) {
  // Default: weiß
  lv_obj_set_style_text_color(sb, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);

  // Fokus: blau
  lv_obj_set_style_text_color(sb, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN | LV_STATE_FOCUSED);

  // Optional: Outline im Fokus
  lv_obj_set_style_outline_width(sb, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_color(sb, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_opa(sb, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_FOCUSED);

  // Wenn nicht fokussiert → Outline weg
  lv_obj_set_style_outline_width(sb, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// Roller: ausgewählte Option im Fokus blau, sonst weiß
static void style_focus_roller(lv_obj_t *roller) {
  // Standardfarbe für text
  lv_obj_set_style_text_color(roller, lv_color_hex(0xF9FAFB), LV_PART_SELECTED | LV_STATE_DEFAULT);

  // Fokus Text blau (optional)
  // lv_obj_set_style_text_color(roller, lv_palette_main(LV_PALETTE_BLUE), LV_PART_SELECTED | LV_STATE_FOCUSED);

  // --- Roter Fokusrahmen ---
  lv_obj_set_style_outline_width(roller, 3, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_color(roller, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_opa(roller, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_FOCUSED);

  // Kein Rahmen im Normalzustand
  lv_obj_set_style_outline_width(roller, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}
// Buttons: Hintergrund im Fokus blau, Text weiß
static void style_focus_button(lv_obj_t *btn, lv_color_t bg_default, lv_color_t bg_focus) {
  // Default
  lv_obj_set_style_bg_color(btn, bg_default, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(btn, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);

  // Fokus
  lv_obj_set_style_bg_color(btn, bg_focus, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_text_color(btn, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_FOCUSED);
}

/////////////////////
// SCREEN FUNCTIONS
/////////////////////
void ui_main_screen_init() {
  UiContext ui;

  ui.screen = lv_obj_create(NULL);
  // ui_main_screen = lv_obj_create(NULL);
  lv_obj_clear_flag(ui.screen, LV_OBJ_FLAG_SCROLLABLE);

  // Hintergrund dunkel (z.B. fast schwarz)
  lv_obj_set_style_bg_color(ui.screen,
                            lv_color_hex(0x111827), // oder 0x111827 für dunkelgrau
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui.screen, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Optional: Standard-Textfarbe gleich mit auf hell setzen
  lv_obj_set_style_text_color(ui.screen, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- lblType Setup ---
  // ------------------------------------------
  ui.lblType = lv_label_create(ui.screen);
  lv_label_set_text(ui.lblType, "Type:");
  lv_obj_set_width(ui.lblType, INPUT_LABLE_W);
  lv_obj_set_height(ui.lblType, INPUT_LABLE_H);
  lv_obj_align(ui.lblType, LV_ALIGN_TOP_LEFT, LBL_LEFT_POS_X, LBL_ROW_Y1);
  lv_obj_set_style_text_color(ui.lblType, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.lblType, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.lblType, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- rolType Setup ---
  // ------------------------------------------
  ui.rollerType = lv_roller_create(ui.screen);
  lv_roller_set_options(ui.rollerType, "PLA-Basic\nPLA+\nPLA-HS\nPETG\nPETG-HS\nASA\nTPU", LV_ROLLER_MODE_NORMAL);
  lv_roller_set_selected(ui.rollerType, 1, LV_ANIM_OFF);
  lv_obj_set_width(ui.rollerType, ROLLER_TYPE_W);
  lv_obj_set_height(ui.rollerType, ROLLER_TYPE_H);
  lv_obj_align(ui.rollerType, LV_ALIGN_TOP_LEFT, INPUT_POS_X0, INPUT_ROW0_Y);
  lv_obj_set_style_bg_color(ui.rollerType, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.rollerType, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.rollerType, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.rolType, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui.rollerType, on_rolType_Clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui.rollerType, on_rolType_Focused, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(ui.rollerType, on_rolType_ValueChanged, LV_EVENT_VALUE_CHANGED, NULL);

  // ------------------------------------------
  // --- btnStart Setup ---
  // ------------------------------------------
  ui.btnStart = lv_btn_create(ui.screen);
  ui.lblBtnStart = lv_label_create(ui.btnStart);
  lv_label_set_text(ui.lblBtnStart, "START");
  lv_obj_center(ui.lblBtnStart);
  lv_obj_set_width(ui.btnStart, BTN_W);
  lv_obj_set_height(ui.btnStart, BTN_H);
  lv_obj_align(ui.btnStart, LV_ALIGN_TOP_LEFT, BTN_X, BTN_Y);
  lv_obj_set_style_bg_color(ui.btnStart, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.btnStart, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.btnStart, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.btnStart, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui.btnStart, on_btnStart_Clicked, LV_EVENT_CLICKED, NULL);

  // ------------------------------------------
  // --- btnCancel Setup ---
  // ------------------------------------------
  ui.btnCancel = lv_btn_create(ui.screen);
  ui.lblBtnCancel = lv_label_create(ui.btnCancel);
  lv_label_set_text(ui.lblBtnCancel, "Cancel");
  lv_obj_center(ui.lblBtnCancel);
  lv_obj_set_width(ui.btnCancel, BTN_W);
  lv_obj_set_height(ui.btnCancel, BTN_H);
  lv_obj_align(ui.btnCancel, LV_ALIGN_TOP_LEFT, BTN_X, BTN_Y + (BTN_H * 1) + 5);
  lv_obj_set_style_bg_color(ui.btnCancel, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.btnCancel, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.btnCancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.btnCancel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui.btnCancel, on_btnCancel_Focused, LV_EVENT_FOCUSED, NULL);

  // ------------------------------------------
  // --- spnTimeHH/MM/SS Setup ---
  // ------------------------------------------
  // --- lblTime Setup ---
  ui.lblTime = lv_label_create(ui.screen);
  lv_label_set_text(ui.lblTime, "Time:");
  lv_obj_set_width(ui.lblTime, INPUT_LABLE_W);
  lv_obj_set_height(ui.lblTime, INPUT_LABLE_H);
  lv_obj_align(ui.lblTime, LV_ALIGN_TOP_LEFT, LBL_LEFT_POS_X, LBL_ROW_Y2);
  lv_obj_set_style_text_color(ui.lblTime, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.lblTime, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.lblTime, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  ui.spnTimeHH = lv_spinbox_create(ui.screen);
  lv_spinbox_set_range(ui.spnTimeHH, 0, 99);
  lv_spinbox_set_value(ui.spnTimeHH, 0);
  lv_obj_set_width(ui.spnTimeHH, SPIN_TIME_W);
  lv_obj_set_height(ui.spnTimeHH, SPIN_TIME_H);
  lv_obj_align(ui.spnTimeHH, LV_ALIGN_TOP_LEFT, INPUT_POS_X0, INPUT_ROW1_Y);
  lv_obj_set_style_bg_color(ui.spnTimeHH, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.spnTimeHH, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.spnTimeHH, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_spinbox_set_digit_format(ui.spnTimeHH, 2, 0);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.spnTimeHH, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui.spnTimeHH, on_spnTimeHH_Clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui.spnTimeHH, on_spnTimeHH_ValueChanged, LV_EVENT_VALUE_CHANGED, NULL);

  // --- lblDP Setup ---
  ui.lblTimeDP1 = lv_label_create(ui.screen);
  lv_label_set_text(ui.lblTimeDP1, ":");
  lv_obj_set_width(ui.lblTimeDP1, 10);
  lv_obj_set_height(ui.lblTimeDP1, 30);
  lv_obj_align(ui.lblTimeDP1, LV_ALIGN_TOP_LEFT, 90, 40);
  lv_obj_set_style_text_color(ui.lblTimeDP1, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.lblTimeDP1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.lblDP, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- spnTimeMM Setup ---
  ui.spnTimeMM = lv_spinbox_create(ui.screen);
  lv_spinbox_set_range(ui.spnTimeMM, 0, 59);
  lv_spinbox_set_value(ui.spnTimeMM, 0);
  lv_obj_set_width(ui.spnTimeMM, SPIN_TIME_W);
  lv_obj_set_height(ui.spnTimeMM, SPIN_TIME_H);
  lv_obj_align(ui.spnTimeMM, LV_ALIGN_TOP_LEFT, INPUT_POS_X1, INPUT_ROW1_Y);
  lv_obj_set_style_bg_color(ui.spnTimeMM, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.spnTimeMM, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.spnTimeMM, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_spinbox_set_digit_format(ui.spnTimeMM, 2, 0);
  lv_obj_add_event_cb(ui.spnTimeMM, on_spnTimeMM_Clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui.spnTimeMM, on_spnTimeMM_ValueChanged, LV_EVENT_VALUE_CHANGED, NULL);

  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.spnTimeMM, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- lblDP2 Setup ---
  ui.lblTimeDP1 = lv_label_create(ui.screen);
  lv_label_set_text(ui.lblTimeDP1, ":");
  lv_obj_set_width(ui.lblTimeDP1, 20);
  lv_obj_set_height(ui.lblTimeDP1, 30);
  lv_obj_align(ui.lblTimeDP1, LV_ALIGN_TOP_LEFT, INPUT_POS_X2, INPUT_ROW1_Y);
  lv_obj_set_style_text_color(ui.lblTimeDP1, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.lblTimeDP1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.lblDP2, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- spnTimeSS Setup ---
  ui.spnTimeSS = lv_spinbox_create(ui.screen);
  lv_spinbox_set_range(ui.spnTimeSS, 0, 59);
  lv_spinbox_set_value(ui.spnTimeSS, 0);
  lv_obj_set_width(ui.spnTimeSS, SPIN_TIME_W);
  lv_obj_set_height(ui.spnTimeSS, SPIN_TIME_H);
  lv_obj_align(ui.spnTimeSS, LV_ALIGN_TOP_LEFT, INPUT_POS_X2, INPUT_ROW1_Y);
  lv_obj_set_style_bg_color(ui.spnTimeSS, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.spnTimeSS, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.spnTimeSS, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_spinbox_set_digit_format(ui.spnTimeSS, 2, 0);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.spnTimeSS, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- Temperature
  // ------------------------------------------
  // --- lblTemp Setup ---
  ui.lblTemp = lv_label_create(ui.screen);
  lv_label_set_text(ui.lblTemp, "Temp:");
  lv_obj_set_width(ui.lblTemp, 60);
  lv_obj_set_height(ui.lblTemp, 30);
  lv_obj_align(ui.lblTemp, LV_ALIGN_TOP_LEFT, LBL_LEFT_POS_X, LBL_ROW_Y3);
  lv_obj_set_style_text_color(ui.lblTemp, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.lblTemp, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.lblTemp, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- spnTemp Setup ---
  ui.spnTemp = lv_spinbox_create(ui.screen);
  lv_spinbox_set_range(ui.spnTemp, 0, 120);
  lv_spinbox_set_value(ui.spnTemp, 0);
  lv_obj_set_width(ui.spnTemp, 150);
  lv_obj_set_height(ui.spnTemp, 35);
  lv_obj_align(ui.spnTemp, LV_ALIGN_TOP_LEFT, INPUT_POS_X0, INPUT_ROW2_Y);
  lv_obj_set_style_bg_color(ui.spnTemp, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui.spnTemp, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui.spnTemp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_spinbox_set_digit_format(ui.spnTemp, 3, 0);

  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui.spnTemp, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- Logger - Info-Message
  // ------------------------------------------
  // --- lblInfoMsg Setup ---
  ui.lblInfoMsg = lv_label_create(ui.screen);
  lv_label_set_long_mode(ui.lblInfoMsg, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
  lv_label_set_recolor(ui.lblInfoMsg, true);
  lv_obj_set_style_text_font(ui.lblInfoMsg, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_width(ui.lblInfoMsg, 300);
  lv_obj_set_height(ui.lblInfoMsg, 30);
  lv_obj_set_style_text_color(ui.lblInfoMsg, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(ui.lblInfoMsg, "#00bd19FF *** Filament-Silicat-Dreyer V0.1 (c) 2025 by LunaX ***#");
  // lv_obj_set_style_text_align(ui.lblInfoMsg, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(ui.lblInfoMsg, LV_ALIGN_BOTTOM_MID, 5, 0);

  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblInfoMsg, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // Actuator ON/OFF Images
  // ------------------------------------------
  // --- imgDorr Setup ---
  ui_imgDorr = lv_img_create(ui.screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_imgDorr, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_imgDorr, 30);
  lv_obj_set_height(ui_imgDorr, 30);
  lv_obj_align(ui_imgDorr, LV_ALIGN_TOP_LEFT, 5, 115);
  lv_obj_set_style_bg_color(ui_imgDorr, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- Image2 Setup ---
  ui_Image2 = lv_img_create(ui.screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_Image2, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_Image2, 30);
  lv_obj_set_height(ui_Image2, 30);
  lv_obj_align(ui_Image2, LV_ALIGN_TOP_LEFT, 45, 115);
  lv_obj_set_style_bg_color(ui_Image2, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- Image3 Setup ---
  ui_Image3 = lv_img_create(ui.screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_Image3, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_Image3, 30);
  lv_obj_set_height(ui_Image3, 30);
  lv_obj_align(ui_Image3, LV_ALIGN_TOP_LEFT, 90, 115);
  lv_obj_set_style_bg_color(ui_Image3, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- imgMotor Setup ---
  ui_imgMotor = lv_img_create(ui.screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_imgMotor, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_imgMotor, 30);
  lv_obj_set_height(ui_imgMotor, 30);
  lv_obj_align(ui_imgMotor, LV_ALIGN_TOP_LEFT, 135, 115);
  lv_obj_set_style_bg_color(ui_imgMotor, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // Focus-Groupierung für RotarySwitch
  // ------------------------------------------
  ui.group = lv_group_get_default();
  if (!ui.group) {
    ui.group = lv_group_create();
    lv_group_set_default(ui.group);
  }
  lv_group_add_obj(ui.group, ui.rollerType);
  lv_group_add_obj(ui.group, ui.spnTimeHH);
  lv_group_add_obj(ui.group, ui.spnTimeMM);
  lv_group_add_obj(ui.group, ui.spnTimeSS);
  lv_group_add_obj(ui.group, ui.spnTemp);
  lv_group_add_obj(ui.group, ui.btnStart);
  lv_group_add_obj(ui.group, ui.btnCancel);

  // lv_group_focus_obj(ui.rollerType);
  lv_group_focus_obj(ui.btnStart);

  // Roller, SpinBox, Buttons styles setzen
  style_focus_roller(ui.rollerType);
  style_focus_spinbox(ui.spnTimeHH);
  style_focus_spinbox(ui.spnTimeMM);
  style_focus_spinbox(ui.spnTimeSS);
  style_focus_spinbox(ui.spnTemp);

  style_focus_button(ui.btnStart, lv_color_hex(0x202020), lv_palette_main(LV_PALETTE_BLUE));
  style_focus_button(ui.btnCancel, lv_color_hex(0x202020), lv_palette_main(LV_PALETTE_BLUE));

  // global UI
  g_ui = ui;
}

/////////////////////
// MAIN UI FUNCTION
/////////////////////
void ui_init(void) {
  ui_main_screen_init();
  lv_disp_load_scr(ui_get()->screen);
}
