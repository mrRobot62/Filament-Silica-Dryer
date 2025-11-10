/*
 * FILE: ui.c
 * AUTHOR: LVGL Visual Architect (Generated)
 * DESCRIPTION: Implementation of the UI. This file should NOT be modified
 *              by the user, as it will be regenerated.
 */

#include "ui.h"

#define LBL_LEFT_POS_X 5
#define INPUT_POS_X0 50
#define INPUT_POS_X1 80
#define INPUT_ROW0_X 50
#define INPUT_ROW0_Y 5

#define ROLLER_TYPE_X INPUT_ROW0_X
#define ROLLER_TYPE_Y INPUT_ROW0_Y
#define ROLLER_TYPE_W 150
#define ROLLER_TYPE_H 35

#define ROLLER_TIME_W 40
#define ROLLER_TIME_H 35

// ------------------------------------------------------------
// Fonts (müssen in lv_conf.h aktiviert sein)
// ------------------------------------------------------------
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_18);
LV_FONT_DECLARE(lv_font_montserrat_20);

/////////////////////
// EVENT FORWARD DECLARATIONS
/////////////////////
void on_rolType_Clicked(lv_event_t *e);
void on_rolType_Focused(lv_event_t *e);
void on_rolType_ValueChanged(lv_event_t *e);
void on_bntStart_Clicked(lv_event_t *e);
void on_bntStart_Focused(lv_event_t *e);
void on_bntCancel_Focused(lv_event_t *e);
void on_bntCancel_Clicked(lv_event_t *e);
void on_spnTimeHH_Clicked(lv_event_t *e);
void on_spnTimeHH_ValueChanged(lv_event_t *e);

/////////////////////
// ASSETS
/////////////////////
// LV_IMG_DECLARE(...)

/////////////////////
// SCREEN DEFINITIONS
/////////////////////
lv_obj_t *ui_main_screen;

/////////////////////
// WIDGET DEFINITIONS
/////////////////////
lv_obj_t *ui_lblType;
lv_obj_t *ui_rolType;
lv_obj_t *ui_bntStart;
lv_obj_t *ui_bntCancel;
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

/////////////////////
// SCREEN FUNCTIONS
/////////////////////
void ui_main_screen_init(void) {
  ui_main_screen = lv_obj_create(NULL);
  lv_obj_clear_flag(ui_main_screen, LV_OBJ_FLAG_SCROLLABLE);

  // Hintergrund dunkel (z.B. fast schwarz)
  lv_obj_set_style_bg_color(ui_main_screen,
                            lv_color_hex(0x111827), // oder 0x111827 für dunkelgrau
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui_main_screen, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Optional: Standard-Textfarbe gleich mit auf hell setzen
  lv_obj_set_style_text_color(ui_main_screen, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- lblType Setup ---
  // ------------------------------------------
  ui_lblType = lv_label_create(ui_main_screen);
  lv_label_set_text(ui_lblType, "Type:");
  lv_obj_set_width(ui_lblType, 50);
  lv_obj_set_height(ui_lblType, 30);
  lv_obj_align(ui_lblType, LV_ALIGN_TOP_LEFT, 5, 10);
  lv_obj_set_style_text_color(ui_lblType, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_lblType, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblType, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- rolType Setup ---
  // ------------------------------------------
  ui_rolType = lv_roller_create(ui_main_screen);
  lv_roller_set_options(ui_rolType, "PLA-Basic\nPLA+\nPLA-HS\nPETG\nPETG-HS\nASA\nTPU", LV_ROLLER_MODE_NORMAL);
  lv_roller_set_selected(ui_rolType, 1, LV_ANIM_OFF);
  lv_obj_set_width(ui_rolType, 150);
  lv_obj_set_height(ui_rolType, 35);
  lv_obj_align(ui_rolType, LV_ALIGN_TOP_LEFT, 50, 5);
  lv_obj_set_style_bg_color(ui_rolType, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_rolType, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_rolType, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_rolType, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui_rolType, on_rolType_Clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_rolType, on_rolType_Focused, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(ui_rolType, on_rolType_ValueChanged, LV_EVENT_VALUE_CHANGED, NULL);

  // ------------------------------------------
  // --- bntStart Setup ---
  // ------------------------------------------
  ui_bntStart = lv_btn_create(ui_main_screen);
  lv_obj_t *ui_bntStart_label = lv_label_create(ui_bntStart);
  lv_label_set_text(ui_bntStart_label, "START");
  lv_obj_center(ui_bntStart_label);
  lv_obj_set_width(ui_bntStart, 80);
  lv_obj_set_height(ui_bntStart, 25);
  lv_obj_align(ui_bntStart, LV_ALIGN_TOP_LEFT, 230, 10);
  lv_obj_set_style_bg_color(ui_bntStart, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_bntStart, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_bntStart, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_bntStart, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui_bntStart, on_bntStart_Clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_bntStart, on_bntStart_Focused, LV_EVENT_FOCUSED, NULL);

  // ------------------------------------------
  // --- bntCancel Setup ---
  // ------------------------------------------
  ui_bntCancel = lv_btn_create(ui_main_screen);
  lv_obj_t *ui_bntCancel_label = lv_label_create(ui_bntCancel);
  lv_label_set_text(ui_bntCancel_label, "Cancel");
  lv_obj_center(ui_bntCancel_label);
  lv_obj_set_width(ui_bntCancel, 80);
  lv_obj_set_height(ui_bntCancel, 25);
  lv_obj_align(ui_bntCancel, LV_ALIGN_TOP_LEFT, 230, 40);
  lv_obj_set_style_bg_color(ui_bntCancel, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_bntCancel, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_bntCancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_bntCancel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui_bntCancel, on_bntCancel_Focused, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(ui_bntCancel, on_bntCancel_Clicked, LV_EVENT_CLICKED, NULL);

  // ------------------------------------------
  // --- spnTimeHH/MM/SS Setup ---
  // ------------------------------------------
  // --- lblTime Setup ---
  ui_lblTime = lv_label_create(ui_main_screen);
  lv_label_set_text(ui_lblTime, "Time:");
  lv_obj_set_width(ui_lblTime, 50);
  lv_obj_set_height(ui_lblTime, 30);
  lv_obj_align(ui_lblTime, LV_ALIGN_TOP_LEFT, 5, 40);
  lv_obj_set_style_text_color(ui_lblTime, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_lblTime, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblTime, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  ui_spnTimeHH = lv_spinbox_create(ui_main_screen);
  lv_spinbox_set_range(ui_spnTimeHH, 0, 99);
  lv_spinbox_set_value(ui_spnTimeHH, 99);
  lv_obj_set_width(ui_spnTimeHH, 40);
  lv_obj_set_height(ui_spnTimeHH, 35);
  lv_obj_align(ui_spnTimeHH, LV_ALIGN_TOP_LEFT, 50, 40);
  lv_obj_set_style_bg_color(ui_spnTimeHH, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_spnTimeHH, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_spnTimeHH, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_spnTimeHH, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui_spnTimeHH, on_spnTimeHH_Clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_spnTimeHH, on_spnTimeHH_ValueChanged, LV_EVENT_VALUE_CHANGED, NULL);
  // --- lblDP Setup ---
  ui_lblDP = lv_label_create(ui_main_screen);
  lv_label_set_text(ui_lblDP, ":");
  lv_obj_set_width(ui_lblDP, 10);
  lv_obj_set_height(ui_lblDP, 30);
  lv_obj_align(ui_lblDP, LV_ALIGN_TOP_LEFT, 90, 40);
  lv_obj_set_style_text_color(ui_lblDP, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_lblDP, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblDP, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- spnTimeMM Setup ---
  ui_spnTimeMM = lv_spinbox_create(ui_main_screen);
  lv_spinbox_set_range(ui_spnTimeMM, 0, 59);
  lv_spinbox_set_value(ui_spnTimeMM, 59);
  lv_obj_set_width(ui_spnTimeMM, 40);
  lv_obj_set_height(ui_spnTimeMM, 35);
  lv_obj_align(ui_spnTimeMM, LV_ALIGN_TOP_LEFT, 100, 40);
  lv_obj_set_style_bg_color(ui_spnTimeMM, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_spnTimeMM, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_spnTimeMM, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_spnTimeMM, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- lblDP2 Setup ---
  ui_lblDP2 = lv_label_create(ui_main_screen);
  lv_label_set_text(ui_lblDP2, ":");
  lv_obj_set_width(ui_lblDP2, 20);
  lv_obj_set_height(ui_lblDP2, 30);
  lv_obj_align(ui_lblDP2, LV_ALIGN_TOP_LEFT, 140, 40);
  lv_obj_set_style_text_color(ui_lblDP2, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_lblDP2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblDP2, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- spnTimeSS Setup ---
  ui_spnTimeSS = lv_spinbox_create(ui_main_screen);
  lv_spinbox_set_range(ui_spnTimeSS, 0, 100);
  lv_spinbox_set_value(ui_spnTimeSS, 50);
  lv_obj_set_width(ui_spnTimeSS, 50);
  lv_obj_set_height(ui_spnTimeSS, 35);
  lv_obj_align(ui_spnTimeSS, LV_ALIGN_TOP_LEFT, 150, 40);
  lv_obj_set_style_bg_color(ui_spnTimeSS, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_spnTimeSS, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_spnTimeSS, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_spnTimeSS, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- Temperature
  // ------------------------------------------
  // --- lblTemp Setup ---
  ui_lblTemp = lv_label_create(ui_main_screen);
  lv_label_set_text(ui_lblTemp, "Temp:");
  lv_obj_set_width(ui_lblTemp, 60);
  lv_obj_set_height(ui_lblTemp, 30);
  lv_obj_align(ui_lblTemp, LV_ALIGN_TOP_LEFT, 5, 79.5);
  lv_obj_set_style_text_color(ui_lblTemp, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_lblTemp, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblTemp, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- spnTemp Setup ---
  ui_spnTemp = lv_spinbox_create(ui_main_screen);
  lv_spinbox_set_range(ui_spnTemp, 0, 120);
  lv_spinbox_set_value(ui_spnTemp, 0);
  lv_obj_set_width(ui_spnTemp, 150);
  lv_obj_set_height(ui_spnTemp, 35);
  lv_obj_align(ui_spnTemp, LV_ALIGN_TOP_LEFT, 50, 77);
  lv_obj_set_style_bg_color(ui_spnTemp, lv_color_hex(0x374151), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_spnTemp, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui_spnTemp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_spnTemp, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- Logger - Info-Message
  // ------------------------------------------
  // --- lblInfoMsg Setup ---
  ui_lblInfoMsg = lv_label_create(ui_main_screen);
  lv_label_set_text(ui_lblInfoMsg, "#00bd19FF *** Filament-Silicat-Dreyer V0.1 (c) 2025 by LunaX ***#");
  lv_label_set_long_mode(ui_lblInfoMsg, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
  lv_label_set_recolor(ui_lblInfoMsg, true);
  lv_obj_set_style_text_font(ui_lblInfoMsg, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_width(ui_lblInfoMsg, 300);
  lv_obj_set_height(ui_lblInfoMsg, 30);
  lv_obj_set_style_text_color(ui_lblInfoMsg, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);
  //lv_obj_set_style_text_align(ui_lblInfoMsg, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(ui_lblInfoMsg, LV_ALIGN_CENTER, 5, 145);

  // To use custom font size, enable a font in lv_conf.h and apply it here.
  // lv_obj_set_style_text_font(ui_lblInfoMsg, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // Actuator ON/OFF Images
  // ------------------------------------------
  // --- imgDorr Setup ---
  ui_imgDorr = lv_img_create(ui_main_screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_imgDorr, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_imgDorr, 30);
  lv_obj_set_height(ui_imgDorr, 30);
  lv_obj_align(ui_imgDorr, LV_ALIGN_TOP_LEFT, 5, 115);
  lv_obj_set_style_bg_color(ui_imgDorr, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- Image2 Setup ---
  ui_Image2 = lv_img_create(ui_main_screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_Image2, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_Image2, 30);
  lv_obj_set_height(ui_Image2, 30);
  lv_obj_align(ui_Image2, LV_ALIGN_TOP_LEFT, 45, 115);
  lv_obj_set_style_bg_color(ui_Image2, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- Image3 Setup ---
  ui_Image3 = lv_img_create(ui_main_screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_Image3, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_Image3, 30);
  lv_obj_set_height(ui_Image3, 30);
  lv_obj_align(ui_Image3, LV_ALIGN_TOP_LEFT, 90, 115);
  lv_obj_set_style_bg_color(ui_Image3, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
  // --- imgMotor Setup ---
  ui_imgMotor = lv_img_create(ui_main_screen);
  // LV_IMG_DECLARE(ui_img_img);
  // lv_img_set_src(ui_imgMotor, &ui_img_img);
  // NOTE: You must declare the image asset in ui.h or a separate asset file.
  lv_obj_set_width(ui_imgMotor, 30);
  lv_obj_set_height(ui_imgMotor, 30);
  lv_obj_align(ui_imgMotor, LV_ALIGN_TOP_LEFT, 135, 115);
  lv_obj_set_style_bg_color(ui_imgMotor, lv_color_hex(0x272F3B), LV_PART_MAIN | LV_STATE_DEFAULT);
}

/////////////////////
// MAIN UI FUNCTION
/////////////////////
void ui_init(void) {
  ui_main_screen_init();
  lv_disp_load_scr(ui_main_screen);
}
