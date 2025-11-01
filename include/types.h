#pragma once
#include <stdint.h>

typedef struct {
  float T_chamber;  // °C
  float T_cj;       // °C (cold junction, optional)
  uint32_t t_ms;    // ms
} meas_t;

typedef enum {
  ST_BOOT=0,
  ST_INIT,
  ST_READY,
  ST_PREHEAT,
  ST_SOAK,
  ST_COOLDOWN,
  ST_PAUSED,
  ST_FINISHED,
  ST_ALARM
} app_state_t;