#include <Arduino.h>
#include "config.h"
#include "types.h"
#include "ipc.h"
#include "actuators.h"

static float T_set = PRESET_SILICA_T; // default
static float duty = 0.0f;

static inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void safety_check(const meas_t &m)
{
  EventBits_t s = xEventGroupGetBits(eg_safety);
  if (m.T_chamber > TEMP_MAX_CUTOFF || (s & SB_DOOR_OPEN) || (s & SB_SENSORFAIL))
  {
    // broadcast alarm
    uint32_t reason = 0;
    if (m.T_chamber > TEMP_MAX_CUTOFF)
      reason |= (1u << 0);
    if (s & SB_DOOR_OPEN)
      reason |= (1u << 1);
    if (s & SB_SENSORFAIL)
      reason |= (1u << 2);
    xQueueSend(q_alarms, &reason, 0);
    // zero duty immediately
    duty = 0.0f;
  }
}

void task_control(void *)
{
  TickType_t last = xTaskGetTickCount();
  for (;;)
  {
    meas_t m;
    if (xQueueReceive(q_measurements, &m, 0) == pdTRUE)
    {
      // very simple P-only placeholder; replace with PID later
      float e = T_set - m.T_chamber;
      float u = 0.1f * e; // Kp placeholder
      duty = clampf(u, 0.0f, 1.0f);
      safety_check(m);
    }
    // publish duty via a simple shared mechanism (here: event bits not used; use a proper setter in real impl)
    // For now store to a global in task_actuators via a function (decl will be provided).
    extern void heater_set_duty(float);
    heater_set_duty(duty);

    vTaskDelayUntil(&last, pdMS_TO_TICKS(CTRL_LOOP_MS));
  }
}