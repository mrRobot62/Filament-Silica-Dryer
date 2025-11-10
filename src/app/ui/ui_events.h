#pragma once
#include "../pins.h"
#include <RotarySwitch.h>

static RotarySwitch rs(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW);
static long lastPosShown = LONG_MIN;

void ui_event_task();
void ui_event_init();