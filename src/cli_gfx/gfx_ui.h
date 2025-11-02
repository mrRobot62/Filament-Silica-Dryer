#pragma once
#include "status.h"

// ST7789 initialisieren
void gfx_init();

// Volles Redraw mit aktuellem Status
void gfx_draw_full(const SystemStatus &s);

// Diff-basiertes Redraw (ruft intern ggf. full)
void gfx_tick(const SystemStatus &s);