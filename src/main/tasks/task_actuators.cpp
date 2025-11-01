#include <Arduino.h>
#include "config.h"
#include "ipc.h"
#include "actuators.h"

static volatile float s_duty = 0.0f;
static uint32_t window_start = 0;

extern "C" void heater_set_duty(float d)
{
  s_duty = d;
}

static void ssr_on() { digitalWrite(PIN_SSR, HIGH); }
static void ssr_off() { digitalWrite(PIN_SSR, LOW); }

void task_actuators(void *)
{
  window_start = millis();
  for (;;)
  {
    uint32_t now = millis();
    uint32_t elapsed = now - window_start;
    if (elapsed >= TPM_WINDOW_MS)
    {
      window_start = now;
      elapsed = 0;
      if (s_duty > 0.0f)
        ssr_on();
      else
        ssr_off();
    }
    uint32_t on_time = (uint32_t)(s_duty * TPM_WINDOW_MS);
    if (elapsed >= on_time)
      ssr_off();

    // TODO: implement fan logic and safety-driven defaults (e.g., safe fan on alarm)

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}