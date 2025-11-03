#pragma once
#include <Arduino.h>

// Flackerfreie, simple STATUS-UI (zweispaltig) mit Footer (Encoder/BTN).
// Statisches Layout wird einmal gezeichnet; danach werden NUR Wertebereiche
// als kleine Rechtecke neu gefüllt + Text geschrieben (kein Screen-Blinken).

void gfx_begin();
void gfx_show(float tempC, bool fan12v, bool heaterOn, bool fan230On, bool motorOn, int fault = 0, bool doorOpen = false);
void gfx_footer(int32_t encoderClicks, const char *buttonState);

class Arduino_GFX;         // forward decl to avoid header pollution
Arduino_GFX *gfx_handle(); // access to internal gfx for other UI modules