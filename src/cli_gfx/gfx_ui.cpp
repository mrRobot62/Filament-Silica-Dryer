#include "gfx_ui.h"
#include <Arduino_GFX_Library.h>
#include <math.h>

// ============================== ST7789 Pins/Params ==========================
// ST7789 170x320 (ESP32-S3 HMI LCD)
static constexpr int PIN_LCD_SCK = 12;
static constexpr int PIN_LCD_MOSI = 13;
static constexpr int PIN_LCD_CS = 10;
static constexpr int PIN_LCD_DC = 11;
static constexpr int PIN_LCD_RST = 1;
static constexpr int PIN_LCD_BL = 14;

// Artefaktfreie Init (Landscape)
static constexpr uint16_t COL_OFFSET = 35;
static constexpr uint16_t ROW_OFFSET = 0;
static constexpr uint8_t LCD_ROT = 1;
static constexpr bool LCD_IPS = true;
static constexpr uint32_t LCD_SPI_HZ = 40000000;

// ============================== Colors/Layout ===============================
static constexpr uint16_t C_BG = 0x0000;
static constexpr uint16_t C_LBL = 0xFFFF;
static constexpr uint16_t C_VAL = 0x07E0;
static constexpr uint16_t C_ON = 0x07E0;
static constexpr uint16_t C_OFF = 0xF800;
static constexpr uint16_t C_DIM = 0x7BEF;

static constexpr int16_t COL_W = 160;
static constexpr int16_t INNER_PAD = 5;
static constexpr uint8_t TITLE_TS = 2;
static constexpr uint8_t BODY_TS = 1;
static constexpr int16_t LINE_H = 12;
static constexpr int16_t LABEL_W = 70;

static int16_t y_origin = 0;

// ============================== GFX Objects =================================
static Arduino_DataBus *bus = nullptr;
static Arduino_GFX *gfx = nullptr;
static bool g_gfx_ready = false;

// ============================== Helpers =====================================
static inline const char *dir_txt(int8_t d) { return (d > 0) ? "CW" : (d < 0) ? "CCW"
                                                                              : "N/A"; }
static inline const char *onoff_txt(bool on) { return on ? "ON" : "OFF"; }
static inline uint16_t onoff_col(bool on) { return on ? C_ON : C_OFF; }

static void header(const __FlashStringHelper *title)
{
    gfx->fillScreen(C_BG);
    gfx->setTextWrap(false);
    gfx->setTextSize(TITLE_TS);
    gfx->setTextColor(C_LBL);

    int16_t y = 6;
    gfx->setCursor(6, y);
    gfx->print(title);
    y += (TITLE_TS * 8) + 6;
    gfx->drawFastHLine(0, y, gfx->width(), C_DIM);
    y_origin = y + 6;
    gfx->setTextSize(BODY_TS);
}

static void draw_label_val_in_col(int col_index, int row_index,
                                  const __FlashStringHelper *label,
                                  const char *val, uint16_t col_val)
{
    const int16_t col_x = col_index * COL_W;
    const int16_t content_x = col_x + INNER_PAD;
    const int16_t content_w = COL_W - 2 * INNER_PAD;
    const int16_t y = y_origin + row_index * LINE_H;

    gfx->setTextColor(C_LBL);
    gfx->setCursor(content_x, y);
    gfx->print(label);

    gfx->setTextColor(col_val);
    int16_t val_x = content_x + LABEL_W + 6;
    if (val_x < content_x + content_w)
    {
        gfx->setCursor(val_x, y);
        gfx->print(val);
    }
}

static void draw_label_valf_in_col(int col_index, int row_index,
                                   const __FlashStringHelper *label,
                                   float v, uint16_t col_val, uint8_t frac = 2)
{
    const int16_t col_x = col_index * COL_W;
    const int16_t content_x = col_x + INNER_PAD;
    const int16_t content_w = COL_W - 2 * INNER_PAD;
    const int16_t y = y_origin + row_index * LINE_H;

    gfx->setTextColor(C_LBL);
    gfx->setCursor(content_x, y);
    gfx->print(label);

    gfx->setTextColor(col_val);
    int16_t val_x = content_x + LABEL_W + 6;
    if (val_x < content_x + content_w)
    {
        gfx->setCursor(val_x, y);
        gfx->print(v, frac);
    }
}

// ============================== Public API ==================================
void gfx_init()
{
    bus = new Arduino_ESP32SPI(PIN_LCD_DC, PIN_LCD_CS, PIN_LCD_SCK, PIN_LCD_MOSI, -1);
    gfx = new Arduino_ST7789(bus, PIN_LCD_RST, LCD_ROT, LCD_IPS, 170, 320, 0, 0, COL_OFFSET, ROW_OFFSET);
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, HIGH);
    g_gfx_ready = gfx->begin(LCD_SPI_HZ);
    if (g_gfx_ready)
    {
        gfx->invertDisplay(false);
        gfx->setTextWrap(false);
        delay(2);
    }
}

static SystemStatus s_last{}; // diff-cache

void gfx_draw_full(const SystemStatus &s)
{
    if (!g_gfx_ready)
        return;

    header(F("Filament Dryer Status"));

    int col = 0, row = 0;
    // Linke Spalte
    draw_label_val_in_col(col, row++, F("Door"), s.door_low ? "ON" : "OFF", s.door_low ? C_ON : C_OFF);
    draw_label_val_in_col(col, row++, F("SSR"), onoff_txt(s.ssr_on), onoff_col(s.ssr_on));
    draw_label_val_in_col(col, row++, F("FAN5V"), onoff_txt(s.fan5_on), onoff_col(s.fan5_on));
    draw_label_val_in_col(col, row++, F("FAN230"), onoff_txt(s.fan230_on), onoff_col(s.fan230_on));
    draw_label_val_in_col(col, row++, F("LAMP"), onoff_txt(s.lamp_on), onoff_col(s.lamp_on));
    draw_label_val_in_col(col, row++, F("MOTOR"), onoff_txt(s.motor_on), onoff_col(s.motor_on));

    // Rechte Spalte
    col = 1;
    row = 0;
    draw_label_val_in_col(col, row++, F("FAN-L"), onoff_txt(s.fanl_on), onoff_col(s.fanl_on));

    char buf[32];
    snprintf(buf, sizeof(buf), "%ld (%s)", s.enc_count, dir_txt(s.enc_last_dir));
    draw_label_val_in_col(col, row++, F("Encoder"), buf, C_VAL);

    draw_label_val_in_col(col, row++, F("Switch"), s.sw_on ? "ON" : "OFF", s.sw_on ? C_ON : C_OFF);

    if (s.thermo_ok && !isnan(s.tc))
    {
        draw_label_valf_in_col(col, row++, F("Temp"), s.tc, C_VAL, 2);
        draw_label_valf_in_col(col, row++, F("CJ"), s.cj, C_VAL, 2);
    }
    else
    {
        draw_label_val_in_col(col, row++, F("Temp"), "N/A", C_OFF);
    }

    s_last = s;
}

void gfx_tick(const SystemStatus &s)
{
    static uint32_t last_ms = 0;
    uint32_t now = millis();
    if (now - last_ms < 250)
        return; // ~4Hz
    last_ms = now;

    bool changed =
        s.door_low != s_last.door_low ||
        s.ssr_on != s_last.ssr_on ||
        s.fan5_on != s_last.fan5_on ||
        s.fan230_on != s_last.fan230_on ||
        s.fanl_on != s_last.fanl_on ||
        s.lamp_on != s_last.lamp_on ||
        s.motor_on != s_last.motor_on ||
        s.enc_count != s_last.enc_count ||
        s.enc_last_dir != s_last.enc_last_dir ||
        s.sw_on != s_last.sw_on ||
        (s.thermo_ok != s_last.thermo_ok) ||
        (s.thermo_ok && ((isnan(s.tc) != isnan(s_last.tc)) || (!isnan(s.tc) && fabsf(s.tc - s_last.tc) > 0.25f)));

    if (changed)
    {
        gfx_draw_full(s);
    }
}