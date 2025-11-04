#pragma once
#include <Arduino.h>

extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;

// Flackerfreie, simple STATUS-UI (zweispaltig) mit Footer (Encoder/BTN).
// Statisches Layout wird einmal gezeichnet; danach werden NUR Wertebereiche
// als kleine Rechtecke neu gefüllt + Text geschrieben (kein Screen-Blinken).

void gfx_begin();
void gfx_show(float tempC, bool fan12v, bool heaterOn, bool fan230On, bool motorOn, int fault = 0, bool doorOpen = false);
void gfx_footer(int32_t encoderClicks, const char *buttonState);

// Legacy-Shims
// inline void gfxui_log(const char * /*line*/) {}
// inline void gfxui_showStatus(float t, bool f12, bool h, bool f230, bool m)
//{
//    gfx_show(t, f12, h, f230, m, 0);
//}
