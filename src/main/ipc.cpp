#include "ipc.h"

QueueHandle_t q_measurements = nullptr;
QueueHandle_t q_alarms = nullptr;
EventGroupHandle_t eg_safety = nullptr;
EventGroupHandle_t eg_mode = nullptr;
SemaphoreHandle_t mtx_spi = nullptr;
SemaphoreHandle_t mtx_lvgl = nullptr;

void ipc_create() {
  q_measurements = xQueueCreate(8, sizeof(meas_t)); // forward decl in GCC ok if included before
  q_alarms = xQueueCreate(8, sizeof(uint32_t));
  eg_safety = xEventGroupCreate();
  eg_mode = xEventGroupCreate();
  mtx_spi = xSemaphoreCreateMutex();
  mtx_lvgl = xSemaphoreCreateMutex();
}