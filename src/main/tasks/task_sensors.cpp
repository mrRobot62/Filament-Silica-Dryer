#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MAX31856.h>
#include "config.h"
#include "types.h"
#include "ipc.h"

static Adafruit_MAX31856 thermo = Adafruit_MAX31856(PIN_MAX_CS, PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK);

void task_sensors(void*) {
  // init SPI thermocouple
  if (!thermo.begin()) {
    // set sensor fail bit
    xEventGroupSetBits(eg_safety, SB_SENSORFAIL);
  } else {
    thermo.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo.setNoiseFilter(MAX31856_NOISE_FILTER_50HZ);
  }

  TickType_t last = xTaskGetTickCount();
  for(;;) {
    meas_t m{};
    if (xEventGroupGetBits(eg_safety) & SB_SENSORFAIL) {
      m.T_chamber = NAN;
    } else {
      m.T_chamber = thermo.readThermocoupleTemperature();
      m.T_cj = thermo.readCJTemperature();
      if (isnan(m.T_chamber) || thermo.readFault()) {
        xEventGroupSetBits(eg_safety, SB_SENSORFAIL);
      } else {
        xEventGroupClearBits(eg_safety, SB_SENSORFAIL);
      }
    }
    m.t_ms = millis();
    xQueueSend(q_measurements, &m, 0);

    // door polling as simple input (ISR optional)
    if (digitalRead(PIN_DOOR) == HIGH) {
      xEventGroupSetBits(eg_safety, SB_DOOR_OPEN);
    } else {
      xEventGroupClearBits(eg_safety, SB_DOOR_OPEN);
    }

    vTaskDelayUntil(&last, pdMS_TO_TICKS(SENS_LOOP_MS));
  }
}