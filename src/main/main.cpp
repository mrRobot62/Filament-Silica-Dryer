#include <Arduino.h>
#include "config.h"
#include "types.h"
#include "ipc.h"

// forward task declarations
void task_control(void *);
void task_sensors(void *);
void task_actuators(void *);
void task_ui(void *);

static void hw_init();
static void timers_start();

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("\n[FilamentDryer] Booting...");

  hw_init();
  ipc_create();
  timers_start();

  xTaskCreatePinnedToCore(task_control, "ctrl", 8192, nullptr, 4, nullptr, 1);
  xTaskCreatePinnedToCore(task_sensors, "sens", 6144, nullptr, 3, nullptr, 1);
  xTaskCreatePinnedToCore(task_actuators, "act", 4096, nullptr, 3, nullptr, 1);
  xTaskCreatePinnedToCore(task_ui, "ui", 12288, nullptr, 3, nullptr, 0);
}

void loop()
{
  // empty; all work done in tasks
}

static void hw_init()
{
  pinMode(PIN_SSR, OUTPUT);
  digitalWrite(PIN_SSR, LOW);
  pinMode(PIN_FAN5V, OUTPUT);
  digitalWrite(PIN_FAN5V, LOW);
  pinMode(PIN_FAN230, OUTPUT);
  digitalWrite(PIN_FAN230, LOW);
  pinMode(PIN_LAMP230, OUTPUT);
  digitalWrite(PIN_LAMP230, LOW);
  pinMode(PIN_MOTOR230, OUTPUT);
  digitalWrite(PIN_MOTOR230, LOW);
  pinMode(PIN_FANL230, OUTPUT);
  digitalWrite(PIN_FANL230, LOW);
  pinMode(PIN_DOOR, INPUT_PULLUP); // active LOW

  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
}

static void timers_start()
{
  // LVGL tick configured via LV_TICK_CUSTOM in lv_conf.h
  // TPM window start can be implemented in task_actuators using millis()
  // or esp_timer if needed (left for implementation detail).
}