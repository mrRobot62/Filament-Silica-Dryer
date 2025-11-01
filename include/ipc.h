#pragma once

#include "types.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

extern QueueHandle_t q_measurements;
extern QueueHandle_t q_alarms;
extern EventGroupHandle_t eg_safety;
extern EventGroupHandle_t eg_mode;
extern SemaphoreHandle_t mtx_spi;
extern SemaphoreHandle_t mtx_lvgl;

enum SafetyBits
{
  SB_DOOR_OPEN = (1 << 0),
  SB_OVERTEMP = (1 << 1),
  SB_SENSORFAIL = (1 << 2)
};

enum ModeBits
{
  MB_RUNNING = (1 << 0),
  MB_PAUSED = (1 << 1),
  MB_FINISHED = (1 << 2)
};

void ipc_create();