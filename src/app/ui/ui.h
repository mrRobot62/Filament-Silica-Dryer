#pragma once

#include <stdbool.h>
#include <stdint.h>

// Public UI API (call from your app)
void ui_init();                  // build widgets, attach to LVGL, init encoder handling
void ui_task();                  // call periodically from loop() (1-10 ms cadence)
void ui_set_focus(bool focused); // switch encoder coarse/fine step
void apply_focus_step();         // apply step to encoder according to focus mode

// Backward-compat wrapper expected by main.cpp in older code paths
// void ui_update_from_encoder();