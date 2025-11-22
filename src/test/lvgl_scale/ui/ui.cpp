#include "ui.h"

// #374151  DarkGray (Background)
// #F9FAFB  White (Textcolor
// #00bd19FF InfoMessage, LED-Green
// #e11a1aff FrameFocus, ErrorMessage, LED-RED
// #f4f010ff Warn-Message
// #9000ffff Arc_Needle_H
// #c374ffff Arc_Needle_M
// #00ffa6ff IST_TIME, IST_TEMP
// #e100ffff ScaleFrame

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
#define COLOR_ARC_NEEDLE_H 0xFF8800
#define COLOR_ARC_NEEDLE_M COLOR_WHITE
#define COLOR_NEEDLES 0x00FFA6
#define COLOR_TEMP_SET 0x9000FF
#define COLOR_TEMP_ACTUAL 0x00FFA6
#define COLOR_SCALE_FRAME 0xE100FF

#define SCALE_MIN_TICKS 5
/*
    Sicherstellen das wenigistens Basis-Presets vorhanden sind
*/

typedef struct {
  int32_t x;
  int32_t y;
  int32_t length;
} TempLinePosition;

TempLinePosition tempSetPos;
TempLinePosition tempCurPos;

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

void next_needle_positions(const int32_t counter_minutes, int32_t &hour_pos, int32_t &minute_pos) {
  int32_t h = counter_minutes / 60;
  int32_t m = counter_minutes % 60;
  hour_pos = (h % 12) * SCALE_MIN_TICKS + (m / 12);
  UI_DBG("[CB] %d, h:%d, m:%d => hour_pos=%d\n", (int)counter_minutes, (int)h, (int)m, (int)hour_pos);
}

static int calc_minute_angle(int minute) {
  return (90 - minute * 6);
}

static int calc_hour_angle(int hour, int minute) {
  float hm = (float)hour + (float)(minute / 60.0f);
  return (90 - (int)(hm * 30));
}

static void update_needle(lv_obj_t *needle, lv_point_precise_t *buf, int angle_deg, int rFrom, int rTo) {
  lv_obj_t *parent = lv_obj_get_parent(needle);
  // int w = lv_obj_get_width(parent);
  // int h = lv_obj_get_height(parent);

  // int cx = w / 2;
  // int cy = h / 2;
  int cy = TIME_POS_Y + (TIME_SCALE_W / 2);
  int cx = TIME_POS_X + (TIME_SCALE_W / 2);

  float rad = angle_deg * 0.01745329252f; // PI/180

  // Startpunkt (innerer Radius)
  buf[0].x = cx + (int)(cosf(rad) * rFrom);
  buf[0].y = cy - (int)(sinf(rad) * rFrom);

  // Endpunkt (äußerer Radius)
  buf[1].x = cx + (int)(cosf(rad) * rTo);
  buf[1].y = cy - (int)(sinf(rad) * rTo);

  lv_line_set_points(needle, buf, 2);
}

static void calcTempLineLength(float currentTemp, int minTemp, int maxTemp, int padX, int padY, TempLinePosition *out) {
  /**
   * Kalkuliert die Linienlänge und gibt die X/Y/Höhe zurück als [3] Array zurück
   * padding[0] = top, padding[1] = bottom */

  UiContext *ui = &g_ui;
  if (ui->scaleTemp == nullptr) {
    return;
  }

  int h = lv_obj_get_height(ui->scaleTemp);
  int zP = h + padY;

  if (currentTemp < (float)minTemp)
    currentTemp = (float)minTemp;
  if (currentTemp > (float)maxTemp)
    currentTemp = (float)maxTemp;

  float tempRange = (float)(maxTemp - minTemp);
  float tempLength = (float)(h / tempRange * (currentTemp - (float)minTemp));

  out->x = padX;
  out->y = zP;
  out->length = (int32_t)tempLength;

  UI_DBG("Temp: %3.1f, padX: %d, padY: %d, h: %d, zP: %d, tR:%3.1f, tL:%3.1f, out->L:%d\n", currentTemp, padX, padY, h,
         zP, tempRange, tempLength, tempRange, tempLength, out->length);
}

static void updateTemperature(lv_obj_t *tempLine, float tempValue, TempLinePosition *scaleOut) {
  UiContext *ui = &g_ui;

  int xPadding = scaleOut->x;

  calcTempLineLength(tempValue, TEMP_MIN, TEMP_MAX, xPadding, 0, scaleOut);

  lv_obj_set_size(tempLine, TEMP_LINE_W, scaleOut->length);
  lv_obj_set_pos(tempLine, scaleOut->x, scaleOut->y - scaleOut->length);

  UI_DBG("scaleOut: X:%d, Y: %d, H: %d\n", scaleOut->x, scaleOut->y, scaleOut->length);
}

static void countdown_timer_cb(lv_timer_t *t) {
  LV_UNUSED(t);
  UiContext *ui = &g_ui;

  if (!ui->scaleTime || !ui->needleMM || !ui->needleHH) {
    UI_INFO("[CB] not ready, skip");
    return;
  }

  // --- Countdown finished ---
  if (ui->countdown_minutes <= 0) {
    ui->hours = 0;
    ui->minutes = 0;

    update_needle(ui->needleMM, g_minute_hand_points, calc_minute_angle(0), 40, 68);
    update_needle(ui->needleHH, g_hour_hand_points, calc_hour_angle(0, 0), 40, 60);

    lv_timer_del(ui->countdown_timer);
    ui->countdown_timer = nullptr;

    UI_INFO("[CB] Countdown finished");
    return;
  }

  // --- One minute passed ---
  ui->countdown_minutes--;

  int angM = calc_minute_angle(ui->minutes);
  int angH = calc_hour_angle(ui->hours, ui->minutes);

  // Recompute displayed time
  ui->hours = ui->countdown_minutes / 60;
  ui->minutes = ui->countdown_minutes % 60;

  // Debug (optional)
  UI_INFO("[CB] remaining=%ld  h=%ld  m=%ld\n", (long)ui->countdown_minutes, (long)ui->hours, (long)ui->minutes);

  // --- Compute Needle Angles ---
  int angleM = calc_minute_angle(ui->minutes);
  int angleH = calc_hour_angle(ui->hours, ui->minutes);

  // --- Update both needles ---

  update_needle(ui->needleMM, g_minute_hand_points, angleM, 40, 68);
  update_needle(ui->needleHH, g_hour_hand_points, angleH, 40, 60);
  static float newTemp = 20.0f;
  newTemp += 0.5f;
  if (newTemp > 120.0f)
    newTemp = 20.0f;

  tempSetPos.x = TEMP_LINE_X_S_PAD;
  tempSetPos.y = TEMP_Y_START;
  tempSetPos.length = 0;

  tempCurPos.x = TEMP_LINE_X_C_PAD;
  tempCurPos.y = TEMP_Y_START;
  tempCurPos.length = 0;

  updateTemperature(ui->lineTempSet, 100.0, &tempSetPos);
  updateTemperature(ui->lineTempCur, newTemp, &tempCurPos);
}

void ui_main_screen(UiContext *ui) {

  auto mk_scale_needle = [&](uint8_t width, lv_color_t color) -> lv_obj_t * {
    lv_obj_t *line = lv_line_create(ui->scaleTime);
    lv_obj_set_style_line_width(line, width, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    lv_obj_set_style_line_color(line, color, 0);
    return line;
  };

  auto mk_scale_needle_mutable = [&](lv_point_precise_t *pts, uint8_t width, lv_color_t color) -> lv_obj_t * {
    UiContext *ui = &g_ui;
    lv_obj_t *line = lv_line_create(ui->screen);

    // WICHTIG: mutable Points setzen, damit lv_scale die Punkte in dieses Array schreibt
    lv_line_set_points_mutable(line, pts, 2);

    lv_obj_set_style_line_width(line, width, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    lv_obj_set_style_line_color(line, color, 0);
    return line;
  };

  // ------------------------------------------
  // grundlegendes Screen-Setup
  //
  // Bottom - UP - Widgets werden übereinander gelegt
  // Beginn mit dem untersten....
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

  // To use custom font size, enable a font in lv_conf.h and apply it here.

  // ------------------------------------------
  // Scale
  // ------------------------------------------
  ui->scaleTime = lv_scale_create(ui->screen);
  lv_obj_set_size(ui->scaleTime, TIME_SCALE_W, TIME_SCALE_W);
  lv_obj_set_style_radius(ui->scaleTime, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_clip_corner(ui->scaleTime, true, 0);
  lv_obj_set_pos(ui->scaleTime, TIME_POS_X, TIME_POS_Y);

  // lv_obj_center(ui->scaleTime);
  lv_scale_set_mode(ui->scaleTime, LV_SCALE_MODE_ROUND_INNER);
  lv_obj_set_style_bg_opa(ui->scaleTime, LV_OPA_60, 0);
  lv_obj_set_style_bg_color(ui->scaleTime, /*lv_color_black()*/ lv_color_hex(COLOR_WHITE), 0);
  lv_scale_set_label_show(ui->scaleTime, true);
  lv_scale_set_total_tick_count(ui->scaleTime, 61);
  lv_scale_set_major_tick_every(ui->scaleTime, SCALE_MIN_TICKS);
  static const char *hour_ticks[] = {"12", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", NULL};
  lv_scale_set_text_src(ui->scaleTime, hour_ticks);
  static lv_style_t indicator_style;
  lv_style_init(&indicator_style);

  /* Label style properties */
  lv_style_set_text_font(&indicator_style, LV_FONT_DEFAULT);
  lv_style_set_text_color(&indicator_style, lv_palette_main(LV_PALETTE_YELLOW));

  /* Major tick properties */
  lv_style_set_line_color(&indicator_style, lv_palette_main(LV_PALETTE_YELLOW));
  lv_style_set_length(&indicator_style, 12);    /* tick length */
  lv_style_set_line_width(&indicator_style, 3); /* tick width */
  lv_obj_add_style(ui->scaleTime, &indicator_style, LV_PART_INDICATOR);

  /* Minor tick properties */
  static lv_style_t minor_ticks_style;
  lv_style_init(&minor_ticks_style);
  lv_style_set_line_color(&minor_ticks_style, lv_palette_main(LV_PALETTE_YELLOW));

  lv_style_set_length(&minor_ticks_style, 8);     /* tick length */
  lv_style_set_line_width(&minor_ticks_style, 2); /* tick width */
  lv_obj_add_style(ui->scaleTime, &minor_ticks_style, LV_PART_ITEMS);

  /* Main line properties */
  static lv_style_t main_line_style;
  lv_style_init(&main_line_style);
  lv_style_set_arc_color(&main_line_style, /*lv_color_black()*/ lv_color_hex(COLOR_SCALE_FRAME));
  lv_style_set_arc_width(&main_line_style, 5);
  lv_obj_add_style(ui->scaleTime, &main_line_style, LV_PART_MAIN);

  lv_scale_set_range(ui->scaleTime, 0, 60);
  lv_scale_set_angle_range(ui->scaleTime, 360);
  lv_scale_set_rotation(ui->scaleTime, 270); // 0 = 3Uhr, 90=6Uhr, 180=9Uhr, 270=12Uhr

  // ui->needleMM = mk_scale_needle(3, lv_color_hex(COLOR_ARC_NEEDLE_M));
  // ui->needleHH = mk_scale_needle(5, lv_color_hex(COLOR_ARC_NEEDLE_H));
  ui->needleMM = mk_scale_needle_mutable(g_minute_hand_points, 3, lv_color_hex(COLOR_ARC_NEEDLE_M));
  ui->needleHH = mk_scale_needle_mutable(g_hour_hand_points, 5, lv_color_hex(COLOR_ARC_NEEDLE_H));

  ui->countdown_minutes = 4 * 60 + 0;
  ui->hours = ui->countdown_minutes / 60;
  ui->minutes = ui->countdown_minutes % 60;
  int32_t hour_pos = (ui->hours % 12) * 5 + (ui->minutes / 12);

  lv_scale_set_line_needle_value(ui->scaleTime, ui->needleMM, TIME_NEEDLE_LEN_M, ui->minutes);
  lv_scale_set_line_needle_value(ui->scaleTime, ui->needleHH, TIME_NEEDLE_LEN_H, hour_pos);

  // ------------------------------------------
  // --- rollerFilament ---
  // ------------------------------------------

  ui->rollerFilament = lv_roller_create(ui->screen);

  lv_roller_set_selected(ui->rollerFilament, 1, LV_ANIM_OFF);
  lv_obj_set_width(ui->rollerFilament, FILAMENT_W);
  lv_obj_set_height(ui->rollerFilament, FILAMENT_H);
  int x = 160 - (FILAMENT_W / 2);
  int y = TIME_POS_Y + (TIME_CENTER / 2) - (FILAMENT_H / 2);
  lv_obj_set_pos(ui->rollerFilament, x, y);
  // lv_obj_align(ui->rollerFilament, LV_ALIGN_CENTER, x, y);
  //  lv_obj_center(ui->rollerFilament);
  lv_obj_set_style_bg_color(ui->rollerFilament, lv_color_hex(COLOR_BG), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui->rollerFilament, lv_color_hex(COLOR_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui->rollerFilament, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_roller_set_options(ui->rollerFilament, "PLA-Basic\nPLA+\nPLA-HS\nPETG\nPETG-HS\nASA\nTPU", LV_ROLLER_MODE_NORMAL);
  // ui_update_filament_options(g_default_preset_id);

  // ------------------------------------------
  // --- scaleTemp (vertical temperature scale)
  // ------------------------------------------

  ui->scaleTemp = lv_scale_create(ui->screen);
  lv_obj_set_size(ui->scaleTemp, 30, 140); // 30px Breite, 140px Höhe
  lv_obj_set_pos(ui->scaleTemp, 5, 5);     // Links 25px, oben 5px
  // ==== Style für hellere Tick-Linien und Beschriftung ====
  lv_color_t white = lv_color_hex(COLOR_TEMP_SET);

  // Hauptlinie der Skala (die vertikale Linie)
  lv_obj_set_style_line_color(ui->scaleTemp, white, LV_PART_MAIN);

  // Major‑Tick-Linien und deren Beschriftung
  lv_obj_set_style_line_color(ui->scaleTemp, white, LV_PART_INDICATOR);
  lv_obj_set_style_text_color(ui->scaleTemp, white, LV_PART_INDICATOR);

  // Minor‑Tick-Linien und deren Beschriftung (falls vorhanden)
  lv_obj_set_style_line_color(ui->scaleTemp, white, LV_PART_ITEMS);
  lv_obj_set_style_text_color(ui->scaleTemp, white, LV_PART_ITEMS);

  lv_obj_set_style_pad_left(ui->scaleTemp, 3, LV_PART_INDICATOR);
  lv_obj_set_style_pad_left(ui->scaleTemp, 3, LV_PART_ITEMS);

  // 13 Schritte: 0,10,20,…,120  – wir zeigen aber nur 20..120 an
  lv_scale_set_range(ui->scaleTemp, 20, 120); // Default: 0°C bis 120°C
  lv_scale_set_total_tick_count(ui->scaleTemp, 6);
  lv_scale_set_major_tick_every(ui->scaleTemp, 1);

  // eigene Label-Liste; die ersten beiden leer
  static const char *temp_ticks[] = {"20", "40", "60", "80", "100", "120", NULL};
  lv_scale_set_text_src(ui->scaleTemp, temp_ticks);

  lv_scale_set_mode(ui->scaleTemp, LV_SCALE_MODE_VERTICAL_LEFT); // Vertikal am linken Rand
  lv_obj_set_style_text_align(ui->scaleTemp, LV_TEXT_ALIGN_LEFT, LV_PART_ITEMS);

  // Innenabstand oben/unten (damit „120“ nicht anstößt)
  lv_obj_set_style_pad_top(ui->scaleTemp, 5, 0);     // 5 px Platz am oberen Rand
  lv_obj_set_style_pad_bottom(ui->scaleTemp, 10, 0); // 10 px Platz am unteren Rand

  lv_obj_set_style_bg_opa(ui->scaleTemp, LV_OPA_0, 0); // keine Hintergrundfarbe (transparent)

  // ------------------------------------------
  // --- SET-Temperatur-Linie definieren ---
  // ------------------------------------------
  // Hilfs-Lambda: Erstellt eine Temperatur-Linie rechts der Skala
  // Lambda zum Erzeugen der Temperatur-Linie (Set/Curr)
  auto mk_temp_line = [&](lv_color_t color, lv_coord_t x_offset, lv_coord_t y_offset, lv_coord_t init_height,
                          lv_coord_t width) -> lv_obj_t * {
    lv_obj_t *line = lv_obj_create(ui->screen);
    lv_obj_set_size(line, width, init_height);

    // Farbe und Deckkraft
    lv_obj_set_style_bg_color(line, color, 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, 0);

    // Keine Ränder
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_radius(line, 0, 0);

    // Position X = Skala-X + Skala-Breite + x_offset
    lv_coord_t scale_x = lv_obj_get_x(ui->scaleTemp);
    lv_coord_t scale_w = lv_obj_get_width(ui->scaleTemp);
    lv_obj_set_x(line, scale_x + scale_w + x_offset);

    // Position Y = Skala-Y + y_offset
    lv_coord_t scale_y = lv_obj_get_y(ui->scaleTemp);
    lv_obj_set_y(line, scale_y + y_offset);

    return line;
  };

  // Temperatur-Linien erzeugen, SOLL(SET) und IST (Actual)
  int32_t startTemp = 20; // Anfangstemperatur
  tempSetPos.x = TEMP_LINE_X_S_PAD;
  tempSetPos.y = TEMP_Y_START;
  tempSetPos.length = 0;
  tempCurPos.x = TEMP_LINE_X_C_PAD;
  tempCurPos.y = TEMP_Y_START;
  tempCurPos.length = 0;

  ui->lineTempSet = mk_temp_line(lv_color_hex(COLOR_TEMP_SET), TEMP_LINE_X_S_PAD, TEMP_Y_START, TEMP_MIN, TEMP_LINE_W);
  ui->lineTempCur =
      mk_temp_line(lv_color_hex(COLOR_TEMP_ACTUAL), TEMP_LINE_X_C_PAD, TEMP_Y_START, TEMP_MIN, TEMP_LINE_W);

  // --------------------------------------------
  // --- Focus-Gruppe erzeugen ---
  // --- wird benötigt um den Focus auf bestimmte
  // --- Widgets zu setzen
  // --------------------------------------------
  ui->group = lv_group_get_default();
  if (!ui->group) {
    ui->group = lv_group_create();
    lv_group_set_default(ui->group);
  }
  lv_group_add_obj(ui->group, ui->rollerFilament);
  lv_group_add_obj(ui->group, ui->needleHH);
  lv_group_add_obj(ui->group, ui->needleMM);
  lv_group_add_obj(ui->group, ui->lineTempSet);

  lv_group_focus_obj(ui->rollerFilament);

  // Screen laden
  lv_disp_load_scr(ui->screen);
  update_needle(ui->needleMM, g_minute_hand_points, calc_minute_angle(ui->minutes), 40, 68);
  update_needle(ui->needleHH, g_hour_hand_points, calc_hour_angle(ui->hours, ui->minutes), 40, 60);
  // Timer **jetzt** erzeugen
  ui->countdown_timer = lv_timer_create(countdown_timer_cb, 1000, NULL); // 1 Minute
}

void ui_init() {
  UiContext *ui = &g_ui;
  ui_main_screen(ui);
}
