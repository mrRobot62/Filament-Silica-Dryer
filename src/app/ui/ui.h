#pragma once

// Public UI API
void ui_init(); // build LVGL widgets + init encoder
void ui_task(); // call once per loop (does update+poll exactly once)