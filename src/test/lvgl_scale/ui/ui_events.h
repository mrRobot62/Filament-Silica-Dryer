#pragma once
#include "../logging/log_events.h"
#include "../pins.h"

enum class EState { IDLE, RUN, COOLING, ERROR };
enum class EConfigWidget { None, Filament, Time, Temp };

static EConfigWidget configWidget = EConfigWidget::None;
static EState g_state = EState::IDLE;

void ui_event_init();
void ui_event_task();
