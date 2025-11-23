#pragma once
#include "../logging/log_events.h"
#include "../pins.h"
#include <RotarySwitch.h>

static RotarySwitch rs(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW);

enum class EState { IDLE, RUN, COOLING, ERROR };
enum class EConfigWidget { None, Filament, Time, Temp };

static EConfigWidget configWidget = EConfigWidget::None;
static EState g_state = EState::IDLE;

void ui_event_init();
void ui_event_task();
void on_enter_state(EState state);
void on_change_state(EState state);