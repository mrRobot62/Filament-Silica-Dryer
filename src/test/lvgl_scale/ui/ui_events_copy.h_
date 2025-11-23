#pragma once
#include "../pins.h"
#include <RotarySwitch.h>

static RotarySwitch rs(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW);
enum class State { IDLE, EDIT, RUN, COOLING, ERROR };
enum class EditTarget { None, RollerType, TimeHH, TimeMM, TimeSS, Temp };

static EditTarget s_editTarget = EditTarget::None;

void ui_event_task();
void ui_event_init();
void on_enter_state(State state);
void on_change_state(State state);