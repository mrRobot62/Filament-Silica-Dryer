#include "heat_screen.h"
#include "gfx_ui.h"
#include <Arduino_GFX_Library.h>

// ---- Colors (match gfx_ui) ----
static inline uint16_t MAKE_RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
static inline uint16_t C_BG() { return 0x0000; }                      // black
static inline uint16_t C_FG() { return 0xFFFF; }                      // white
static inline uint16_t C_TAB() { return MAKE_RGB565(200, 215, 255); } // light blue
static inline uint16_t C_ACC() { return MAKE_RGB565(36, 118, 240); }  // button fill
static inline uint16_t C_DIM() { return MAKE_RGB565(90, 90, 90); }    // outlines

// Tab-Colors
static inline uint16_t C_TAB_IDLE_BG() { return 0xFFFF; }                   // white
static inline uint16_t C_TAB_ACTIVE_BG() { return MAKE_RGB565(0, 200, 0); } // green
static inline uint16_t C_TAB_BORDER() { return C_DIM(); }
static inline uint16_t C_TAB_TEXT() { return 0x0000; } // black

// Button-Colors
static inline uint16_t C_BTN_IDLE_BG() { return MAKE_RGB565(180, 210, 255); }  // light blue
static inline uint16_t C_BTN_ACTIVE_BG() { return MAKE_RGB565(36, 118, 240); } // blue
static inline uint16_t C_BTN_BORDER() { return C_DIM(); }

// --- Physical screen ---
static constexpr int W = 320;
static constexpr int H = 170;

// --- Tabs ---
static constexpr int TAB_H = 24; // war 30
static constexpr int TAB_W = 72;
static constexpr int TAB_Y = 0;
static constexpr int TAB_X0 = 6;
static constexpr int TAB_GAP = 6;

// --- Content & Footer ---
static constexpr int CONTENT_Y = TAB_H + 2;   // = 26
static constexpr int FOOTER_H = 26;           // war 36 (macht Content niedriger)
static constexpr int FOOTER_Y = H - FOOTER_H; // = 144

// --- Columns (3 equal) ---
static constexpr int COL_W = W / 3;
static constexpr int COL1_X = 0;
static constexpr int COL2_X = COL_W;
static constexpr int COL3_X = COL_W * 2;

// --- Buttons row (kleiner & höher) ---
static constexpr int BTN_W = 80;                   // war 92
static constexpr int BTN_H = 20;                   // war 28
static constexpr int BTN_Y = FOOTER_Y - BTN_H - 2; // neue Footer-Höhe einkalkuliert
static constexpr int BTN_RAD = 8;

static constexpr int BTN1_X = 10;
static constexpr int BTN2_X = (W - BTN_W) / 2;
static constexpr int BTN3_X = W - BTN_W - 10;
static bool buttonsActive = true; // wenn false: alle Buttons als "idle" (hellblau) zeichnen

// Tab selection
static HeatTab selectedTab = HeatTab::Heat;
static HeatTab activeTab = HeatTab::Heat;
static HeatTab focusedTab = HeatTab::Heat;

HeatTab heat_get_active_tab() { return activeTab; }
HeatTab heat_get_focused_tab() { return focusedTab; }

// --- Tab-Zustand ---

// --- Rechte Tab-Spalte (Pixel) ---
// Passe diese Werte ggf. minimal an dein Layout an.
// static constexpr int TAB_W = 56;
// static constexpr int TAB_H = 26;
// static constexpr int TAB_GAP = 8;
static constexpr int TAB_MARGIN = 6; // rechter Außenrand
static constexpr int TAB_X = LCD_WIDTH - (TAB_W + TAB_MARGIN);
static constexpr int TAB_Y0 = 8;
static constexpr int TAB_AREA_Y = 0;               // Beginn Tab-Spalte
static constexpr int TAB_AREA_H = LCD_HEIGHT - 30; // bis oberhalb Footer

// --- Dynamic cells ---
struct Rect
{
    int x, y, w, h;
};
static Rect cell_filament{COL2_X, CONTENT_Y + 30, COL_W, 22};
static Rect cell_timer{COL3_X, CONTENT_Y + 14, COL_W, 70}; // kompakter
static Rect cell_log1{6, FOOTER_Y - 2, W - 12, 10};
static Rect cell_log2{6, FOOTER_Y + 8, W - 12, 10};

static HeatButton selected = HeatButton::Start;
static bool drawn = false;

// Rechte Tab-Spalte (Pixel) – gern feinjustieren
// static constexpr int TAB_W = 56;
// static constexpr int TAB_H = 26;
// static constexpr int TAB_GAP = 8;
// static constexpr int TAB_MARGIN = 6; // rechter Außenrand
// static constexpr int TAB_X = LCD_WIDTH - (TAB_W + TAB_MARGIN);
// static constexpr int TAB_Y0 = 8; // Start oben
// static constexpr int TAB_AREA_Y = 0;
// static constexpr int TAB_AREA_H = LCD_HEIGHT - UI::FTR_H; // bis oberhalb Footer

extern Arduino_GFX *gfx;

// helpers
static void draw_round_btn(Arduino_GFX *g, int x, int y, int w, int h, uint16_t fill, uint16_t outline)
{
    g->fillRoundRect(x, y, w, h, BTN_RAD, fill);
    g->drawRoundRect(x, y, w, h, BTN_RAD, outline);
}
static void write_center(Arduino_GFX *g, int cx, int y, const char *txt, uint8_t size)
{
    g->setTextSize(size);
    g->setTextColor(C_FG(), C_BG());
    int16_t x1, y1;
    uint16_t w, h;
    // Arduino_GFX has no text bounds API; quick center by mono char estimate:
    // size*6 per char width (classic 5x7 font + 1px spacing), height ~ size*8
    uint16_t tw = strlen(txt) * (size * 6);
    int x = cx - (int)tw / 2;
    g->setCursor(x, y);
    g->print(txt);
}
static void clear_rect(Arduino_GFX *g, const Rect &r)
{
    g->fillRect(r.x, r.y, r.w, r.h, C_BG());
}

// Tabs
static void draw_tabs(Arduino_GFX *g)
{
    g->fillRect(0, 0, W, TAB_H, C_BG()); // clear tab row

    struct Tab
    {
        const char *label;
        int x;
        HeatTab id;
    };
    Tab ts[3] = {
        {"HEAT", TAB_X0, HeatTab::Heat},
        {"CFG", TAB_X0 + TAB_W + TAB_GAP, HeatTab::Cfg},
        {"STAT", TAB_X0 + 2 * (TAB_W + TAB_GAP), HeatTab::Stat}};

    for (auto &t : ts)
    {
        bool sel = (t.id == selectedTab);
        uint16_t bg = sel ? C_TAB_ACTIVE_BG() : C_TAB_IDLE_BG();

        // Rechteck (kein rounded)
        g->fillRect(t.x, TAB_Y + 1, TAB_W, TAB_H - 2, bg);
        g->drawRect(t.x, TAB_Y + 1, TAB_W, TAB_H - 2, C_TAB_BORDER());

        // Text
        g->setTextSize(2);
        g->setTextColor(C_TAB_TEXT(), bg);
        g->setCursor(t.x + 8, TAB_Y + 6);
        g->print(t.label);
    }

    // Trennlinie zum Content
    g->drawFastHLine(0, CONTENT_Y - 1, W, C_DIM());
}

// Buttons static
static void draw_buttons(Arduino_GFX *g)
{
    uint16_t fill1, fill2, fill3;

    if (!buttonsActive)
    {
        // Inaktiver Modus: alle hellblau
        fill1 = C_BTN_IDLE_BG();
        fill2 = C_BTN_IDLE_BG();
        fill3 = C_BTN_IDLE_BG();
    }
    else
    {
        // Aktiver Modus: selektierter blau, andere hellblau
        fill1 = (selected == HeatButton::Cancel) ? C_BTN_ACTIVE_BG() : C_BTN_IDLE_BG();
        fill2 = (selected == HeatButton::Start) ? C_BTN_ACTIVE_BG() : C_BTN_IDLE_BG();
        fill3 = (selected == HeatButton::Pause) ? C_BTN_ACTIVE_BG() : C_BTN_IDLE_BG();
    }

    draw_round_btn(g, BTN1_X, BTN_Y, BTN_W, BTN_H, fill1, C_BTN_BORDER());
    draw_round_btn(g, BTN2_X, BTN_Y, BTN_W, BTN_H, fill2, C_BTN_BORDER());
    draw_round_btn(g, BTN3_X, BTN_Y, BTN_W, BTN_H, fill3, C_BTN_BORDER());

    g->setTextSize(1);
    g->setTextColor(C_FG());
    g->setCursor(BTN1_X + 12, BTN_Y + 7);
    g->print("CANCEL");
    g->setCursor(BTN2_X + 24, BTN_Y + 7);
    g->print("START");
    g->setCursor(BTN3_X + 24, BTN_Y + 7);
    g->print("PAUSE");
}

// Focus ring around selected button (no fill redraw)
static void draw_focus(Arduino_GFX *g, HeatButton btn)
{
    // erase previous focus area by redrawing outlines of all
    // then draw highlighted outline for selected
    uint16_t normal = C_DIM();
    uint16_t focus = MAKE_RGB565(255, 255, 255);

    g->drawRoundRect(BTN1_X, BTN_Y, BTN_W, BTN_H, BTN_RAD, normal);
    g->drawRoundRect(BTN2_X, BTN_Y, BTN_W, BTN_H, BTN_RAD, normal);
    g->drawRoundRect(BTN3_X, BTN_Y, BTN_W, BTN_H, BTN_RAD, normal);

    int x = BTN2_X;
    if (btn == HeatButton::Cancel)
        x = BTN1_X;
    else if (btn == HeatButton::Pause)
        x = BTN3_X;

    // thicker border for focus
    for (int i = 0; i < 2; ++i)
        g->drawRoundRect(x - i, BTN_Y - i, BTN_W + 2 * i, BTN_H + 2 * i, BTN_RAD + i, focus);
}

// content scaffolding (center filament label + big timer)
static void draw_content_static(Arduino_GFX *g)
{
    // Vertical guides
    g->drawFastVLine(COL2_X, CONTENT_Y, FOOTER_Y - CONTENT_Y - 2, C_DIM());
    g->drawFastVLine(COL3_X, CONTENT_Y, FOOTER_Y - CONTENT_Y - 2, C_DIM());

    // Gauge: kleinerer Radius + höher platziert
    int gx = COL1_X + COL_W / 2;
    int gy = CONTENT_Y + 42; // war ~60
    int gr = 32;             // war 40
    g->drawCircle(gx, gy, gr, C_DIM());
    write_center(g, gx, gy - 6, "Temp", 2);

    // Filament/Timer-Zellen vorher leeren
    clear_rect(g, cell_filament);
    clear_rect(g, cell_timer);
}

static void draw_footer_static(Arduino_GFX *g)
{
    g->drawFastHLine(0, FOOTER_Y - 2, W, C_DIM());
    // clear both log lines once
    clear_rect(g, cell_log1);
    clear_rect(g, cell_log2);
}

// ---- Public API ----
void heat_init()
{
    auto *g = gfx_handle();
    if (!g)
        return;
    gfx_begin(); // ensure panel init + backlight as in your file

    // full-screen clear ONCE to avoid artifacts across mode switches
    g->fillScreen(C_BG());

    draw_tabs(g);
    draw_content_static(g);
    draw_buttons(g);
    draw_footer_static(g);

    heat_set_selected(HeatButton::Start);
    heat_set_tab(HeatTab::Heat); // sicherstellen, dass Tab-Reihe mit aktivem HEAT gezeichnet ist

    draw_focus(g, selected);
    // initial content
    heat_set_filament("PLA+");
    heat_set_timer_label("00:00:00");
    heat_set_log("Ready.", "LONG CLICK to change TAB");
    drawn = true;
}

void heat_set_selected(HeatButton btn)
{
    selected = btn;
    auto *g = gfx_handle();
    if (!g)
        return;
    // redraw only buttons row to reflect new colors
    draw_buttons(g);
}

void heat_set_filament(const char *name)
{
    auto *g = gfx_handle();
    if (!g)
        return;
    clear_rect(g, cell_filament);
    write_center(g, cell_filament.x + cell_filament.w / 2, cell_filament.y + 8, name ? name : "-", 2);
}

void heat_set_timer_label(const char *hhmmss)
{
    auto *g = gfx_handle();
    if (!g)
        return;
    clear_rect(g, cell_timer);

    // Expect string "HH:MM:SS" or "00:00:00"
    uint8_t hh = 0, mm = 0, ss = 0;
    if (hhmmss && sscanf(hhmmss, "%hhu:%hhu:%hhu", &hh, &mm, &ss) == 3)
    {
        char buf[8];

        g->setTextColor(C_FG(), C_BG());
        g->setTextSize(2);

        // HH / MM / SS mit engerem Abstand (je ~22 px)
        snprintf(buf, sizeof(buf), "%02u", hh);
        write_center(g, cell_timer.x + cell_timer.w / 2, cell_timer.y + 0, buf, 3);

        snprintf(buf, sizeof(buf), "%02u", mm);
        write_center(g, cell_timer.x + cell_timer.w / 2, cell_timer.y + 22, buf, 3);

        snprintf(buf, sizeof(buf), "%02u", ss);
        write_center(g, cell_timer.x + cell_timer.w / 2, cell_timer.y + 44, buf, 3);
    }
    else
    {
        write_center(g, cell_timer.x + cell_timer.w / 2, cell_timer.y + 40, "--", 2);
    }
}

void heat_set_log(const char *line1, const char *line2)
{
    auto *g = gfx_handle();
    if (!g)
        return;
    clear_rect(g, cell_log1);
    clear_rect(g, cell_log2);
    g->setTextSize(1);
    g->setTextColor(C_FG(), C_BG());
    g->setCursor(cell_log1.x, cell_log1.y + 10);
    g->print(line1 ? line1 : "");
    g->setCursor(cell_log2.x, cell_log2.y + 10);
    g->print(line2 ? line2 : "");
}

void heat_set_tab(HeatTab tab)
{
    // dein bestehender Tab-Wechsel (Content) bleibt hier erhalten …
    s_activeTab = tab;
    s_focusTab = tab; // bei echter Aktivierung Fokus = aktiv
    redraw_tab_strip();
}

void heat_focus_tab(HeatTab tab)
{
    focusedTab = tab;

    // Bereich rechts komplett löschen -> verhindert Artefakte
    gfx->fillRect(TAB_X - 2, TAB_AREA_Y, TAB_W + 4, TAB_AREA_H, C_BG());

    const char *labels[3] = {"HEAT", "CFG", "STAT"};

    for (int i = 0; i < 3; i++)
    {
        HeatTab t = (HeatTab)i;
        uint16_t color;

        if (t == activeTab && t == focusedTab)
            color = MAKE_RGB565(0, 255, 0); // aktiv + fokussiert → kräftig GRÜN
        else if (t == activeTab)
            color = MAKE_RGB565(0, 200, 0); // aktiv → GRÜN
        else if (t == focusedTab)
            color = MAKE_RGB565(0, 0, 255); // Fokus → BLAU
        else
            color = MAKE_RGB565(255, 255, 255); // inaktiv → WEISS

        const int x = TAB_X;
        const int y = TAB_Y0 + i * (TAB_H + TAB_GAP);

        // Tab zeichnen
        gfx->fillRoundRect(x, y, TAB_W, TAB_H, 4, color);
        gfx->drawRoundRect(x, y, TAB_W, TAB_H, 4, MAKE_RGB565(200, 200, 200));

        // Text mittig
        gfx->setTextSize(1);
        gfx->setTextColor(MAKE_RGB565(0, 0, 0), color);
        const int tw = strlen(labels[i]) * 6; // 6x8 Font
        const int th = 8;
        const int tx = x + (TAB_W - tw) / 2;
        const int ty = y + (TAB_H - th) / 2 + 1;
        gfx->setCursor(tx, ty);
        gfx->print(labels[i]);
    }
}

// names for logging
const char *heat_button_name(HeatButton b)
{
    switch (b)
    {
    case HeatButton::Cancel:
        return "CANCEL";
    case HeatButton::Start:
        return "START";
    case HeatButton::Pause:
        return "PAUSE";
    }
    return "?";
}
const char *heat_tab_name(HeatTab t)
{
    switch (t)
    {
    case HeatTab::Heat:
        return "HEAT";
    case HeatTab::Cfg:
        return "CFG";
    case HeatTab::Stat:
        return "STAT";
    }
    return "?";
}

void heat_cycle_button(int8_t dir)
{
    // current 'selected' is static in this file
    int v = static_cast<int>(selected);
    v = (v + (dir > 0 ? 1 : -1) + 3) % 3;
    heat_set_selected(static_cast<HeatButton>(v));
}

void heat_cycle_tab(int8_t dir)
{
    int v = static_cast<int>(selectedTab);
    v = (v + (dir > 0 ? 1 : -1) + 3) % 3;
    heat_set_tab(static_cast<HeatTab>(v));
}

void heat_buttons_set_active(bool active)
{
    buttonsActive = active;
    auto *g = gfx_handle();
    if (!g)
        return;
    draw_buttons(g); // nur die Button-Zeile neu zeichnen
}

static void draw_tab_cell(int idx, const char *label, uint16_t bg)
{
    const int x = TAB_X;
    const int y = TAB_Y0 + idx * (TAB_H + TAB_GAP);

    // Buttonfläche
    gfx->fillRoundRect(x, y, TAB_W, TAB_H, 4, bg);
    // Rahmen dezent
    gfx->drawRoundRect(x, y, TAB_W, TAB_H, 4, RGB565(200, 200, 200));

    // Text mittig (horizontal)
    gfx->setTextSize(1);
    gfx->setTextColor(RGB565(0, 0, 0), bg);
    int tw = strlen(label) * 6; // Font 6x8
    int th = 8;
    int tx = x + (TAB_W - tw) / 2;
    int ty = y + (TAB_H - th) / 2 + 1;
    gfx->setCursor(tx, ty);
    gfx->print(label);
}

static void redraw_tab_strip()
{
    // Bereich rechts komplett löschen -> keine Artefakte
    gfx->fillRect(TAB_X - 2, TAB_AREA_Y, TAB_W + 4, TAB_AREA_H, RGB565(0, 0, 0));

    const char *labels[3] = {"HEAT", "CFG", "STAT"};

    for (int i = 0; i < 3; i++)
    {
        HeatTab t = static_cast<HeatTab>(i);
        uint16_t col;

        if (t == s_activeTab && t == s_focusTab)
        {
            col = RGB565(0, 255, 0); // aktiv + fokussiert: satt Grün
        }
        else if (t == s_activeTab)
        {
            col = RGB565(0, 200, 0); // aktiv: Grün
        }
        else if (t == s_focusTab)
        {
            col = RGB565(0, 0, 255); // Fokus: Blau
        }
        else
        {
            col = RGB565(255, 255, 255); // inaktiv: Weiß
        }

        draw_tab_cell(i, labels[i], col);
    }
}