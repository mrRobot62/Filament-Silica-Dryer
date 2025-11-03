#include "gfx_ui.h"
#include <Arduino_GFX_Library.h>
#include <math.h>

// ===== Pins/Panel (deine funktionierende Config) =====
#ifndef LCD_SCK
#define LCD_SCK 12
#endif
#ifndef LCD_MOSI
#define LCD_MOSI 13
#endif
#ifndef LCD_CS
#define LCD_CS 10
#endif
#ifndef LCD_DC
#define LCD_DC 11
#endif
#ifndef LCD_RST
#define LCD_RST 1
#endif
#ifndef LCD_BL
#define LCD_BL 14
#endif

#ifndef LCD_WIDTH
#define LCD_WIDTH 320
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

// ===== GFX globals =====
static Arduino_DataBus *bus = nullptr;
static Arduino_GFX *gfx = nullptr;
static bool initialized = false;

// ===== Farben / Helfer =====
static inline uint16_t MAKE_RGB565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
static inline uint16_t C_ON() { return MAKE_RGB565(0, 220, 0); }     // GRÜN
static inline uint16_t C_OFF() { return MAKE_RGB565(220, 0, 0); }    // ROT
static inline uint16_t C_WARN() { return MAKE_RGB565(255, 255, 0); } // GELB
static inline uint16_t C_LBL() { return 0xFFFF; }                    // WEISS
static inline uint16_t C_BG() { return 0x0000; }                     // SCHWARZ
static inline uint16_t C_OPEN() { return MAKE_RGB565(220, 0, 0); }   // ROT
static inline uint16_t C_CLOSED() { return MAKE_RGB565(0, 220, 0); } // GRÜN
static inline void hline(int y) { gfx->drawFastHLine(0, y, LCD_WIDTH, C_LBL()); }
static inline void backlight(bool on)
{
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, on ? LCD_BACKLIGHT_ON : !LCD_BACKLIGHT_ON);
}

// ===== Layout-Geometrie (einmal fest, danach nur Werte updaten) =====
namespace UI
{
  // Schriften
  constexpr int HDR_SIZE = 2; // Header
  constexpr int TXT_SIZE = 1; // Content
  constexpr int FTR_SIZE = 1; // Footer

  // Header
  constexpr int HDR_H = 30;
  constexpr int HDR_Y = 20; // baseline für "STATUS"
  constexpr int HDR_LINE_Y = 32;

  // Spalten
  constexpr int COL_W = LCD_WIDTH / 2;
  constexpr int COL1_X = 8;
  constexpr int COL2_X = COL1_X + COL_W;
  constexpr int ROW_Y0 = 42;
  constexpr int ROW_DY = 20; // etwas dichter

  // Footer
  constexpr int FTR_H = 25;
  constexpr int FTR_Y = LCD_HEIGHT - FTR_H;
}

// Jede Value-Zeile bekommt ein kleines Rechteck zum „clearen“ vor dem Text.
struct Cell
{
  int x, y, w, h; // Clear-Rect für Value-Text
};
static Cell cellTemp, cellHeater, cellMotor, cellFan12, cellFan230, cellFault, cellDoor;

// Footer-State
static int32_t footerClicks = 0;
static String footerBtn = "RELEASED";

// ==== interne Helfer ====
static void drawLabel(int x, int y, const char *label)
{
  gfx->setTextSize(UI::TXT_SIZE);
  gfx->setTextColor(C_LBL(), C_BG());
  gfx->setCursor(x, y);
  gfx->print(label);
}
static void drawHeader()
{
  gfx->fillRect(0, 0, LCD_WIDTH, UI::HDR_H, C_BG());
  gfx->setTextWrap(false);
  gfx->setTextSize(UI::HDR_SIZE);
  gfx->setTextColor(C_LBL(), C_BG());
  gfx->setCursor(8, UI::HDR_Y);
  gfx->print("STATUS");
  hline(UI::HDR_LINE_Y);
}
static void setupLayoutRects()
{
  // Value-Rechteck: rechts von Label ~72px, Höhe passend zur Schrift size=2 (~16px)
  const int vpad = 2;
  const int vdx = 72; // Abstand Label->Value
  const int vh = 18;

  // Linke Spalte: Temp, Heater, Motor
  int y = UI::ROW_Y0;
  cellTemp = {UI::COL1_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};
  y += UI::ROW_DY;
  cellHeater = {UI::COL1_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};
  y += UI::ROW_DY;
  cellMotor = {UI::COL1_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};
  y += UI::ROW_DY;
  cellDoor = {UI::COL1_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};

  // Rechte Spalte: Fan12V, Fan230, Fault
  y = UI::ROW_Y0;
  cellFan12 = {UI::COL2_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};
  y += UI::ROW_DY;
  cellFan230 = {UI::COL2_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};
  y += UI::ROW_DY;
  cellFault = {UI::COL2_X + vdx, y - UI::TXT_SIZE * 8 + 2, UI::COL_W - vdx - 10, vh};
}
static void drawStaticLabelsAndLines()
{
  drawHeader();

  // Linke Spalte
  int y1 = UI::ROW_Y0;
  drawLabel(UI::COL1_X, y1, "Temp");
  y1 += UI::ROW_DY;
  drawLabel(UI::COL1_X, y1, "Heater");
  y1 += UI::ROW_DY;
  drawLabel(UI::COL1_X, y1, "Motor");
  y1 += UI::ROW_DY;
  drawLabel(UI::COL1_X, y1, "Door");

  // Rechte Spalte
  int y2 = UI::ROW_Y0;
  drawLabel(UI::COL2_X, y2, "Fan12V");
  y2 += UI::ROW_DY;
  drawLabel(UI::COL2_X, y2, "Fan230");
  y2 += UI::ROW_DY;
  drawLabel(UI::COL2_X, y2, "Fault");

  // Linie oberhalb Footer
  hline(LCD_HEIGHT - UI::FTR_H - 2);
}

// Value in Zelle zeichnen (Clear + Text farbig)
static void drawValueInCell(const Cell &c, const String &text, uint16_t color)
{
  gfx->fillRect(c.x, c.y, c.w, c.h, C_BG());
  gfx->setTextSize(UI::TXT_SIZE);
  gfx->setTextColor(color, C_BG());
  gfx->setCursor(c.x, c.y + UI::TXT_SIZE * 8 - 2); // baseline korrigiert
  gfx->print(text);
}

// ===== Public API =====
void gfx_begin()
{
  if (initialized)
    return;

  bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI, GFX_NOT_DEFINED);

  // >>> Exakt deine funktionierende Signatur (mit 0,0 vor Offsets):
  gfx = new Arduino_ST7789(
      bus,
      LCD_RST,
      LCD_ROTATION,
      true, // IPS
      LCD_HEIGHT, LCD_WIDTH,
      0, 0,
      35, 0);

  if (!gfx->begin())
  {
    initialized = false;
    return;
  }
  backlight(true);
  gfx->fillScreen(C_BG());
  gfx->setTextWrap(false);

  setupLayoutRects();
  drawStaticLabelsAndLines();

  // Initiale Werte leeren:
  drawValueInCell(cellTemp, "NaN", C_WARN());
  drawValueInCell(cellHeater, "OFF", C_OFF());
  drawValueInCell(cellMotor, "OFF", C_OFF());
  drawValueInCell(cellFan12, "STOPPED", C_OFF());
  drawValueInCell(cellFan230, "OFF", C_OFF());
  drawValueInCell(cellFault, "None", C_LBL());
  drawValueInCell(cellDoor, "CLOSED", C_OFF());

  // Footer initial
  gfx_footer(0, "RELEASED");

  initialized = true;
}

void gfx_show(float tempC, bool fan12v, bool heaterOn, bool fan230On, bool motorOn, int fault, bool doorOpen)
{
  if (!initialized)
  {
    gfx_begin();
    if (!initialized)
      return;
  }

  // Nur Values (kleine Rechtecke) updaten – KEIN fillscreen, kein Blinken
  char buf[24];

  if (isnan(tempC))
  {
    drawValueInCell(cellTemp, "NaN", C_WARN());
  }
  else
  {
    snprintf(buf, sizeof(buf), "%.1f C", tempC);
    drawValueInCell(cellTemp, String(buf), C_ON());
  }

  drawValueInCell(cellHeater, heaterOn ? "ON" : "OFF", heaterOn ? C_ON() : C_OFF());
  drawValueInCell(cellMotor, motorOn ? "ON" : "OFF", motorOn ? C_ON() : C_OFF());
  drawValueInCell(cellFan12, fan12v ? "RUNNING" : "STOPPED", fan12v ? C_ON() : C_OFF());
  drawValueInCell(cellFan230, fan230On ? "ON" : "OFF", fan230On ? C_ON() : C_OFF());
  drawValueInCell(cellDoor, doorOpen ? "OPEN" : "CLOSED", doorOpen ? C_OPEN() : C_CLOSED());

  if (fault != 0)
  {
    snprintf(buf, sizeof(buf), "F %d", fault);
    drawValueInCell(cellFault, String(buf), C_WARN());
  }
  else
  {
    drawValueInCell(cellFault, "None", C_LBL());
  }
}

void gfx_footer(int32_t encoderClicks, const char *buttonState)
{
  footerClicks = encoderClicks;
  footerBtn = buttonState ? String(buttonState) : String("RELEASED");

  // Footer-Zeile (zwei Spalten) OHNE Screen-Refresh
  gfx->fillRect(0, UI::FTR_Y, LCD_WIDTH, UI::FTR_H, C_BG());
  gfx->setTextSize(UI::FTR_SIZE);
  gfx->setTextColor(C_LBL(), C_BG());

  // links: Enc
  gfx->setCursor(6, LCD_HEIGHT - 6);
  gfx->print("Enc: ");
  gfx->print(footerClicks);

  // rechts: BTN
  const int col2x = UI::COL2_X; // zweite Spalte
  uint16_t btnCol = C_LBL();
  if (footerBtn == "PRESSED")
    btnCol = C_ON();
  else if (footerBtn == "LONG")
    btnCol = C_WARN();

  gfx->setTextColor(C_LBL(), C_BG());
  gfx->setCursor(col2x + 6, LCD_HEIGHT - 6);
  gfx->print("BTN: ");
  gfx->setTextColor(btnCol, C_BG());
  gfx->print(footerBtn);
}

Arduino_GFX *gfx_handle() { return gfx; }