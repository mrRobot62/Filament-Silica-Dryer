#include <Arduino.h>
#include "config.h"
#include "ipc.h"

// Minimal placeholder UI task; LVGL init not included here to keep skeleton compact.
void task_ui(void*) {
  for(;;) {
    // Placeholder: handle encoder, draw minimal status over Serial
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 1000) {
      lastPrint = millis();
      Serial.println("[UI] alive");
    }
    vTaskDelay(pdMS_TO_TICKS(UI_LOOP_MS));
  }
}