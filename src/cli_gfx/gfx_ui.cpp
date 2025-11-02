#include "gfx_ui.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

#ifndef LCD_SCK
#define LCD_SCK   12
#endif
#ifndef LCD_MOSI
#define LCD_MOSI  13
#endif
#ifndef LCD_CS
#define LCD_CS    10
#endif
#ifndef LCD_DC
#define LCD_DC    11
#endif
#ifndef LCD_RST
#define LCD_RST   1
#endif
#ifndef LCD_BL
#define LCD_BL    14
#endif

#ifndef LCD_WIDTH
#define LCD_WIDTH  320
#endif
#ifndef LCD_HEIGHT
#define LCD_HEIGHT 170
#endif
#ifndef LCD_COL_OFFSET
#define LCD_COL_OFFSET 35
#endif
#ifndef LCD_ROW_OFFSET
#define LCD_ROW_OFFSET 0
#endif

#ifndef LCD_ROTATION
#define LCD_ROTATION 1
#endif

#ifndef LCD_BACKLIGHT_ON
#define LCD_BACKLIGHT_ON HIGH
#endif

static Arduino_DataBus *bus = nullptr;
static Arduino_GFX     *gfx = nullptr;
static bool initialized = false;

static String lastLog;

struct ViewData {
  float tempC = NAN;
  bool  fan12v = false;
  bool  heater = false;
  bool  fan230 = false;
  bool  motor = false;
  int   fault  = 0;
};

static ViewData lastView{};
static bool hasLast = false;

static void backlight(bool on) {
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, on ? LCD_BACKLIGHT_ON : !LCD_BACKLIGHT_ON);
}

static bool changed(const ViewData& a, const ViewData& b) {
  if (a.heater != b.heater) return true;
  if (a.fan230 != b.fan230) return true;
  if (a.motor  != b.motor)  return true;
  if (a.fan12v != b.fan12v) return true;
  if (a.fault  != b.fault)  return true;
  if (isnan(a.tempC) != isnan(b.tempC)) return true;
  if (!isnan(a.tempC) && !isnan(b.tempC) && fabs(a.tempC - b.tempC) > 0.2f) return true;
  return false;
}

static void drawBanner() {
  gfx->fillRect(0, 0, LCD_WIDTH, 22, gfx->color565(30, 30, 30));
  gfx->setCursor(8, 16);
  gfx->setTextSize(1);
  gfx->setTextColor(0xFFFF, gfx->color565(30, 30, 30));
  gfx->print("CLI_GFX Status");
}

static void drawKeyValue(int y, const char* key, const String& value, uint16_t color) {
  const int leftKey = 8;
  const int leftVal = 120;
  gfx->setTextSize(2);
  gfx->setTextColor(0xFFFF, 0x0000);
  gfx->setCursor(leftKey, y);
  gfx->print(key);
  gfx->fillRect(leftVal-4, y-12, LCD_WIDTH-leftVal-8, 20, 0x0000);
  gfx->setTextColor(color, 0x0000);
  gfx->setCursor(leftVal, y);
  gfx->print(value);
}

static void drawFooter() {
  gfx->fillRect(0, LCD_HEIGHT-18, LCD_WIDTH, 18, gfx->color565(20, 20, 20));
  gfx->setTextSize(1);
  gfx->setTextColor(0xFFFF, gfx->color565(20, 20, 20));
  gfx->setCursor(6, LCD_HEIGHT-6);
  String msg = lastLog;
  if (msg.length() > 42) msg = msg.substring(0, 42);
  gfx->print(msg);
}

static void drawFull(const ViewData &v) {
  gfx->fillScreen(0x0000);
  drawBanner();

  int y = 42;
  char buf[32];

  if (isnan(v.tempC)) {
    drawKeyValue(y, "Temp", F("NaN"), gfx->color565(255, 120, 120));
  } else {
    snprintf(buf, sizeof(buf), "%.2f C", v.tempC);
    drawKeyValue(y, "Temp", String(buf), 0x07E0 /*green*/);
  }
  y += 26;

  drawKeyValue(y, "Fan12V", v.fan12v ? F("RUNNING") : F("STOPPED"),
               v.fan12v ? gfx->color565(120,255,120) : gfx->color565(255,180,120));
  y += 26;

  drawKeyValue(y, "Heater", v.heater ? F("ON") : F("OFF"),
               v.heater ? gfx->color565(255,180,120) : 0xFFFF);
  y += 26;

  drawKeyValue(y, "Fan230", v.fan230 ? F("ON") : F("OFF"),
               v.fan230 ? gfx->color565(255,255,120) : 0xFFFF);
  y += 26;

  drawKeyValue(y, "Motor", v.motor ? F("ON") : F("OFF"),
               v.motor ? gfx->color565(120,180,255) : 0xFFFF);
  y += 26;

  if (v.fault != 0) {
    snprintf(buf, sizeof(buf), "Fault %d", v.fault);
    drawKeyValue(y, "Fault", String(buf), gfx->color565(255, 120, 120));
  } else {
    drawKeyValue(y, "Fault", F("None"), 0xFFFF);
  }

  drawFooter();
}

void gfx_begin() {
  if (initialized) return;

  bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI, GFX_NOT_DEFINED);
  gfx = new Arduino_ST7789(bus, LCD_RST, LCD_ROTATION, true, LCD_WIDTH, LCD_HEIGHT, LCD_COL_OFFSET, LCD_ROW_OFFSET);

  if (!gfx->begin()) {
    initialized = false;
    return;
  }
  backlight(true);

  gfx->fillScreen(0x0000);
  gfx->setTextWrap(false);
  gfx->setCursor(0, 0);
  gfx->setTextSize(1);
  gfx->setTextColor(0xFFFF, 0x0000);
  gfx->println(F("GFX init OK"));
  initialized = true;
}

void gfx_log(const char* line) {
  if (!line) return;
  lastLog = String(line);
  if (!initialized) return;
  drawFooter();
}

void gfx_show(float tempC, bool fan12v, bool heaterOn, bool fan230On, bool motorOn, int fault) {
  if (!initialized) {
    gfx_begin();
    if (!initialized) return;
  }
  ViewData v;
  v.tempC = tempC;
  v.fan12v = fan12v;
  v.heater = heaterOn;
  v.fan230 = fan230On;
  v.motor = motorOn;
  v.fault = fault;

  bool need = !hasLast || changed(v, lastView);
  if (need) {
    drawFull(v);
    lastView = v;
    hasLast = true;
  } else {
    drawFooter();
  }
}
