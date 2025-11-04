#pragma once
#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Global handles (declared in gfx_ui.cpp)
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;

// Public API
void gfx_begin();
void gfx_show(float tempC, bool fan12v, bool heaterOn, bool fan230On,
              bool motorOn, int fault = 0, bool doorOpen = false);
void gfx_footer(int32_t encoderClicks, const char *buttonState);

// Optional getter for modules needing direct access
Arduino_GFX *gfx_handle();