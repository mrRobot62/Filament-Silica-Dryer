#include "ui.h"
// #374151  DarkGray (Background)
// #F9FAFB  White (Textcolor
// #00bd19FF InfoMessage, LED-Green
// #e11a1aff FrameFocus, ErrorMessage, LED-RED
// #f4f010ff Warn-Message
// #9000ffff Arc_Needle_H
// #c374ffff Arc_Needle_M
// #00ffa6ff IST_TIME, IST_TEMP

// #F9FAFB
// #000000
// #000000
// #000000
// #000000

#define COLOR_BG 0x374151
#define COLOR_GREEN 0x00BD19
#define COLOR_RED 0x11A11A
#define COLOR_YELLOW 0xF4F010
#define COLOR_WHITE 0xF9FAFB

#define COLOR_TEXT COLOR_WHITE
#define COLOR_ARC_NEEDLE_H 0x9000FF
#define COLOR_ARC_NEEDLE_M 0xC374FF
#define COLOR_NEEDLES 0x00FFA6
#define COLOR_TEMP_SOLL 0x9000FF
#define COLOR_TEMP_IST 0x00FFA6

/*
    Sicherstellen das wenigistens Basis-Presets vorhanden sind
*/

void initFilamentPresets() {
  g_presetCount = 0;

  // --- ans Ende: CUSTOM + SILICA ---

  g_presets[g_presetCount++] = FilamentPreset("CUSTOM", 0.0f, 0, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("SILICA", 105.0f, 1, 30, true);
  g_presets[g_presetCount++] = FilamentPreset("ABS", 80.0f, 5, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("ASA", 82.5f, 5, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("PETG", 62.5f, 6, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("PLA", 47.5f, 5, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("TPU", 45.0f, 5, 0, false); // aus TPU 95A / 98A
  g_presets[g_presetCount++] = FilamentPreset("Spec-ASA-CF", 85.0f, 9, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-BVOH", 52.5f, 7, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-HIPS", 65.0f, 5, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PA(CF,PET,PH*)", 85.0f, 9, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PC(CF/FR)", 85.0f, 9, 0, false); // PC (Polycarbonat)
  g_presets[g_presetCount++] = FilamentPreset("Spec-PC-ABS", 82.5f, 9, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PET-CF", 75.0f, 8, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PETG-CF", 70.0f, 8, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PETG-HF", 65.0f, 6, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PLA-CF", 55.0f, 6, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PLA-HT", 55.0f, 6, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PLA-WoodMetal", 45.0f, 5, 0, false); // aus PLA-Wood / Metal
  g_presets[g_presetCount++] = FilamentPreset("Spec-POM", 70.0f, 5, 0, false);           // POM (Delrin)
  g_presets[g_presetCount++] = FilamentPreset("Spec-PP", 55.0f, 5, 0, false);            // PP (Polypropylen)
  g_presets[g_presetCount++] = FilamentPreset("Spec-PP-GF", 65.0f, 8, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PPS(+CF)", 85.0f, 9, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PVA", 50.0f, 8, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-PVDF-PPSU", 85.0f, 9, 0, false); // aus PVDF / PPSU
  g_presets[g_presetCount++] = FilamentPreset("Spec-TPU 82A", 42.5f, 5, 0, false);
  g_presets[g_presetCount++] = FilamentPreset("Spec-WOOD-Composite", 45.0f, 5, 0, false); // aus WOOD / Composite
}

void ui_update_filament_options(uint8_t defaultID) {
  initFilamentPresets();

  UiContext *ui = &g_ui;
  if (ui->rollerFilament == nullptr)
    return;

  if (g_presetCount == 0) {
    lv_roller_set_options(ui->rollerFilament, "<empty>", LV_ROLLER_MODE_NORMAL);
    return;
  }
  if (defaultID >= g_presetCount)
    defaultID = 0;

  String options;
  for (uint8_t i = 0; i < g_presetCount; ++i) {
    options += g_presets[i].filament;
    if (i < g_presetCount - 1) {
      options += "\n";
    }
  }

  lv_roller_set_options(ui->rollerFilament, options.c_str(), LV_ROLLER_MODE_NORMAL);
  lv_roller_set_selected(ui->rollerFilament, defaultID, LV_ANIM_OFF);
}

void updateUiPresetWidgets(uint8_t optionID) {
  UiContext *ui = &g_ui;
  if (optionID >= g_presetCount) {
    return; // out of range
  }
  FilamentPreset fp = g_presets[optionID];
  UI_INFO("UpdateUiPresetWidgets(%s)", fp.filament.c_str());
}

void ui_main_screen(UiContext *ui) {

  // ------------------------------------------
  // grundlegendes Screen-Setup
  // ------------------------------------------
  ui->screen = lv_obj_create(NULL);
  lv_obj_clear_flag(ui->screen, LV_OBJ_FLAG_SCROLLABLE);

  // Hintergrund dunkel (z.B. fast schwarz)
  lv_obj_set_style_bg_color(ui->screen,
                            lv_color_hex(0x111827), // oder 0x111827 für dunkelgrau
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui->screen, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Optional: Standard-Textfarbe gleich mit auf hell setzen
  lv_obj_set_style_text_color(ui->screen, lv_color_hex(0xF9FAFB), LV_PART_MAIN | LV_STATE_DEFAULT);

  // ------------------------------------------
  // --- rollerFilament ---
  // ------------------------------------------

  ui->rollerFilament = lv_roller_create(ui->screen);

  lv_roller_set_selected(ui->rollerFilament, 1, LV_ANIM_OFF);
  lv_obj_set_width(ui->rollerFilament, FILAMENT_W);
  lv_obj_set_height(ui->rollerFilament, FILAMENT_H);
  // lv_obj_align(ui->rollerFilament, LV_ALIGN_TOP_LEFT, FILAMENT_X, FILAMENT_Y);
  lv_obj_center(ui->rollerFilament);
  lv_obj_set_style_bg_color(ui->rollerFilament, lv_color_hex(COLOR_BG), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui->rollerFilament, lv_color_hex(COLOR_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui->rollerFilament, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // lv_roller_set_options( ui->rollerFilament, "PLA-Basic\nPLA+\nPLA-HS\nPETG\nPETG-HS\nASA\nTPU",
  // LV_ROLLER_MODE_NORMAL);
  ui_update_filament_options(g_default_preset_id);

  // ------------------------------------------
  // --- scaleTime ---
  // ------------------------------------------
  static lv_obj_t *minute_needle;
  static lv_obj_t *hour_needle;

  auto mk_round_scale = [&](uint8_t w) -> lv_obj_t * {
    lv_obj_t *scale = lv_scale_create(ui->screen);
    lv_obj_set_size(scale, w, w);
    lv_obj_set_style_radius(scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(scale, true, 0);
    lv_obj_center(scale);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    return scale;
  };

  auto mk_time_ring = [&](uint8_t w, uint8_t t) -> lv_obj_t * {
    lv_obj_t *scale = lv_scale_create(ui->screen);
    lv_obj_set_style_arc_width(scale, w, LV_PART_MAIN);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_range(scale, 0, 60);
    return scale;
  };

  // ---------------------
  // Stunden-Ziffernblatt
  // ---------------------
  ui->scaleTime = mk_round_scale(TIME_SCALE_W);
  lv_obj_set_style_bg_opa(ui->scaleTime, LV_OPA_60, 0);
  lv_obj_set_style_bg_color(ui->scaleTime, /*lv_color_black()*/ lv_color_hex(COLOR_WHITE), 0);
  lv_scale_set_label_show(ui->scaleTime, true);
  lv_scale_set_total_tick_count(ui->scaleTime, 61);
  lv_scale_set_major_tick_every(ui->scaleTime, 5);
  static const char *hour_ticks[] = {"12", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", NULL};
  lv_scale_set_text_src(ui->scaleTime, hour_ticks);
  static lv_style_t indicator_style;
  lv_style_init(&indicator_style);

  /* Label style properties */
  lv_style_set_text_font(&indicator_style, LV_FONT_DEFAULT);
  lv_style_set_text_color(&indicator_style, lv_palette_main(LV_PALETTE_YELLOW));

  /* Major tick properties */
  lv_style_set_line_color(&indicator_style, lv_palette_main(LV_PALETTE_YELLOW));
  lv_style_set_length(&indicator_style, 18);    /* tick length */
  lv_style_set_line_width(&indicator_style, 3); /* tick width */
  lv_obj_add_style(ui->scaleTime, &indicator_style, LV_PART_INDICATOR);

  /* Minor tick properties */
  static lv_style_t minor_ticks_style;
  lv_style_init(&minor_ticks_style);
  lv_style_set_line_color(&minor_ticks_style, lv_palette_main(LV_PALETTE_YELLOW));

  lv_style_set_length(&minor_ticks_style, 14);    /* tick length */
  lv_style_set_line_width(&minor_ticks_style, 2); /* tick width */
  lv_obj_add_style(ui->scaleTime, &minor_ticks_style, LV_PART_ITEMS);

  /* Main line properties */
  static lv_style_t main_line_style;
  lv_style_init(&main_line_style);
  lv_style_set_arc_color(&main_line_style, lv_color_black());
  lv_style_set_arc_width(&main_line_style, 5);
  lv_obj_add_style(ui->scaleTime, &main_line_style, LV_PART_MAIN);

  lv_scale_set_range(ui->scaleTime, 0, 60);
  lv_scale_set_angle_range(ui->scaleTime, 360);
  lv_scale_set_rotation(ui->scaleTime, 270); // 0 = 3Uhr, 90=6Uhr, 180=9Uhr, 270=12Uhr

  // ---------------------
  // Stunden-Anzeige
  // ---------------------
  ui->scaleTime_HH = mk_time_ring(TIME_SCALE_HH_W, TIME_SCALE_HHMM_T);
  static lv_style_t style_HH;
  lv_style_init(&style_HH);
  lv_style_set_arc_color(&style_HH, lv_palette_darken(LV_PALETTE_PURPLE, 3));
  lv_scale_section_t *section_HH = lv_scale_add_section(ui->scaleTime_HH);
  lv_scale_set_section_range(ui->scaleTime_HH, section_HH, 5, 17);
  lv_scale_set_section_style_main(ui->scaleTime_HH, section_HH, &style_HH);

  // ---------------------
  // Stunden-Anzeige
  // ---------------------
  ui->scaleTime_MM = mk_round_scale(TIME_SCALE_MM_W);

  //   minute_hand = lv_line_create(ui->scaleTime);
  //   lv_line_set_points_mutable(minute_hand, minute_hand_points, 2);

  //   lv_obj_set_style_line_width(minute_hand, 3, 0);
  //   lv_obj_set_style_line_rounded(minute_hand, true, 0);
  //   lv_obj_set_style_line_color(minute_hand, lv_color_white(), 0);

  //   hour_hand = lv_line_create(ui->scaleTime);

  //   lv_obj_set_style_line_width(hour_hand, 5, 0);
  //   lv_obj_set_style_line_rounded(hour_hand, true, 0);
  //   lv_obj_set_style_line_color(hour_hand, lv_palette_main(LV_PALETTE_RED), 0);

  //   hour = 11;
  //   minute = 5;
  // lv_timer_t * timer = lv_timer_create(timer_cb, 250, NULL);
  // lv_timer_ready(timer);

  // To use custom font size, enable a font in lv_conf.h and apply it here.

  // --------------------------------------------
  // --- Focus-Gruppe erzeugen ---
  // --- wird benötigt um den Focus auf bestimmte
  // --- Widgets zu setzen
  // --------------------------------------------
  ui->group = lv_group_get_default();
}

void ui_init() {
  UiContext *ui = &g_ui;
  ui_main_screen(ui);
  lv_disp_load_scr(ui->screen);
}

void ui_task() {
  UiContext *ui = &g_ui;
}