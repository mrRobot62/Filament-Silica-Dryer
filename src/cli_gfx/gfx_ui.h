#pragma once
#include <Arduino.h>

// Tiny GFX UI for temporary CLI_GFX app.
// No dependency on SystemStatus to keep things simple.
//
// API:
//   gfx_begin();
//   gfx_show(tempC, fan12v, heaterOn, fan230On, motorOn, fault=0);
//   gfx_log("text");
//
void gfx_begin();
void gfx_log(const char* line);
void gfx_show(float tempC, bool fan12v, bool heaterOn, bool fan230On, bool motorOn, int fault = 0);

// Backward-compat for existing code calling old names:
inline void gfxui_log(const char* line) { gfx_log(line); }
inline void gfxui_showStatus(float tempC, bool fan12v, bool heaterOn, bool fan230On, bool motorOn) {
    gfx_show(tempC, fan12v, heaterOn, fan230On, motorOn, /*fault=*/0);
}
