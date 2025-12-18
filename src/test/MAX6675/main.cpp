// this example is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#include "logging/log_ui.h"
#include "pins.h"
#include <Wire.h>
#include <max6675.h>

int thermoDO = MAX6675_DO;
int thermoCS = MAX6675_CS;
int thermoCLK = MAX6675_CLK;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  Serial.begin(115200);

  Serial.println("-------- MAX6675 test ---------");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp

  Serial.print("C° = ");
  Serial.println(thermocouple.readCelsius());

  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(1000);
}