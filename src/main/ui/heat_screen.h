#pragma once
#include <Arduino.h>

// Simple HEAT screen (170x320 landscape) with 3 buttons.
// Static layout + focus highlight (no timer logic yet).

enum class HeatButton : uint8_t
{
    Cancel = 0,
    Start = 1,
    Pause = 2
};

enum class HeatTab : uint8_t
{
    Heat = 0,
    Cfg = 1,
    Stat = 2
};

void heat_init();                                        // draws static layout once
void heat_set_selected(HeatButton btn);                  // focus ring/outline update
void heat_set_filament(const char *name);                // center label (e.g., "PLA+")
void heat_set_timer_label(const char *hhmmss);           // big time "00:00:00"
void heat_set_log(const char *line1, const char *line2); // bottom 2 lines
// Steuert, ob die Button-Zeile aktiv (ein Button blau) oder inaktiv (alle hellblau) gezeichnet wird.
void heat_buttons_set_active(bool active);
void heat_set_tab(HeatTab t); // re-renders the tab row

// Optional quick helper (for tests)
inline void heat_set_timer_seconds(uint32_t s)
{
    char buf[16];
    uint32_t h = s / 3600;
    s %= 3600;
    uint32_t m = s / 60;
    uint32_t sec = s % 60;
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", (unsigned long)h, (unsigned long)m, (unsigned long)sec);
    heat_set_timer_label(buf);
}

// --- Navigation helpers for rotary input ---
void heat_cycle_button(int8_t dir); // dir: +1 next, -1 prev
void heat_cycle_tab(int8_t dir);

// Optional: names for logging
const char *heat_button_name(HeatButton b);
const char *heat_tab_name(HeatTab t);