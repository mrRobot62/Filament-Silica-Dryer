#include <Arduino.h>
#include "pins.h"
#include "sensors/rotary_input.h" // nutzt deine RotaryInput-Klasse
#include "ui/heat_screen.h"
#include "ui/gfx_ui.h"

// ---- DEBUG LOGGING ----
#ifndef ROTARY_DEBUG
#define ROTARY_DEBUG 1
#endif

#if ROTARY_DEBUG
#define DBG(...)                    \
    do                              \
    {                               \
        Serial.printf(__VA_ARGS__); \
    } while (0)
#else
#define DBG(...) \
    do           \
    {            \
    } while (0)
#endif

static const char *mode_name(bool tabsMode) { return tabsMode ? "TABS" : "BUTTONS"; }

// Rotary objects
static RotarySwitch gRot(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_BTN_PIN, /*buttonActiveLow=*/false);
static RotaryInput gRotInput(gRot);

// Aktueller Bedienmodus
enum class InputMode : uint8_t
{
    Buttons,
    Tabs
};
static InputMode mode = InputMode::Buttons;

// ------------------------------------------------------------------
// Setup
// ------------------------------------------------------------------
void rotary_setup()
{
    // interne Pullups AUS, wie von dir gefordert; Zeiten: debounce=15ms, double=350ms, long=2500ms
    gRotInput.begin(false, 15, 350, 2500);

    mode = InputMode::Buttons;
    heat_set_tab(HeatTab::Heat);          // aktiver Tab = HEAT (grün)
    heat_set_selected(HeatButton::Start); // Start-Button blau
    heat_buttons_set_active(true);        // Buttons aktiv zeichnen
    heat_set_log("BUTTON MODE", "Rotate: CANCEL / START / PAUSE");

    DBG("\n[rotary_setup] mode=%s  debounce=15  dClick=350  long=2500  dir%s\n",
        mode_name(false),
#ifdef ROTARY_DIR_REVERSED
        "=-1 (REVERSED)"
#else
        "=+1 (NORMAL)"
#endif
    );
}

// ------------------------------------------------------------------
// Poll events and drive the UI
// ------------------------------------------------------------------
void rotary_update()
{
    RotaryEvent ev{};
    if (!gRotInput.poll(ev))
        return;

    // --- ROTATION ---
    if (ev.moved)
    {
        int step =
#ifdef ROTARY_DIR_REVERSED
            (ev.delta > 0) ? -1 : +1;
#else
            (ev.delta > 0) ? +1 : -1;
#endif

        DBG("[rotary] MOVE  delta=%ld  pos=%ld  step=%+d  mode=%s\n",
            ev.delta, ev.newPosition, step, mode_name(mode == InputMode::Tabs));

        if (mode == InputMode::Buttons)
        {
            // Buttons: Fokus zwischen CANCEL/START/PAUSE wechseln (blau)
            heat_cycle_button(step);
        }
        else
        {
            // TAB-MODE: blauen Fokus (focus) zwischen HEAT/CFG/STAT verschieben
            HeatTab cur = heat_get_focused_tab();
            int next = ((int)cur + step + 3) % 3; // wrap 0..2
            heat_focus_tab((HeatTab)next);        // blau umsetzen
            DBG("[rotary] TAB focus -> %d\n", next);
        }
    }

    // ---------- BUTTON FSM ----------
    static bool pressedNow = false;
    static bool longPending = false; // Long erkannt, Toggle beim Release
    static uint32_t pressStartMs = 0;

    if (ev.evPressed)
    {
        pressedNow = true;
        longPending = false;
        pressStartMs = millis();
        DBG("[rotary] PRESS  pressed=%d  mode=%s\n",
            ev.pressed, mode_name(mode == InputMode::Tabs));
    }

    // LONG erkannt (vom Treiber) ODER Sicherheits-Timeout (falls evLongClick nicht käme)
    if ((ev.evLongClick && pressedNow) ||
        (pressedNow && !longPending && (millis() - pressStartMs > 2500)))
    {
        longPending = true; // Toggle wird beim Release ausgeführt
        DBG("[rotary] LONG detected (will toggle on release)\n");
    }

    if (ev.evReleased)
    {
        DBG("[rotary] RELEASE  pressed=%d  longPending=%d  mode(before)=%s\n",
            ev.pressed, longPending, mode_name(mode == InputMode::Tabs));

        if (longPending)
        {
            // ---- Mode toggle NUR hier (nach Loslassen) ----
            if (mode == InputMode::Buttons)
            {
                mode = InputMode::Tabs;
                heat_buttons_set_active(false);        // alle Buttons hellblau
                heat_focus_tab(heat_get_active_tab()); // aktiven Tab blau markieren
                heat_set_log("TAB MODE", "Rotate: HEAT / CFG / STAT");
            }
            else
            {
                mode = InputMode::Buttons;
                heat_buttons_set_active(true);        // Buttons wieder aktiv
                heat_set_selected(HeatButton::Start); // START wieder blau
                heat_set_log("BUTTON MODE", "Rotate: CANCEL / START / PAUSE");
            }
            DBG("[rotary] MODE -> %s (after long)\n", mode_name(mode == InputMode::Tabs));
        }
        else
        {
            // ---- normaler ShortClick ----
            if (mode == InputMode::Buttons)
            {
                // (Aktion folgt in Schritt 3.3; aktuell nur Log)
                heat_set_log("Button selected", "LONG CLICK to change TAB");
            }
            else
            {
                // TAB aktivieren: blauer Fokus -> grün (Content-Wechsel)
                HeatTab chosen = heat_get_focused_tab();
                heat_set_tab(chosen); // wird grün
                heat_set_log("Tab selected", "Rotate to change, LONG to exit");
            }
        }

        // Zyklus zurücksetzen
        pressedNow = false;
        longPending = false;
        pressStartMs = 0;
    }
}