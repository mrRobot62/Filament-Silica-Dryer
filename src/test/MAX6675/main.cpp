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
int vccPin = 3;
int gndPin = 2;

// make a cute degree symbol
uint8_t degree[8] = {140, 146, 146, 140, 128, 128, 128, 128};

void setup() {
  Serial.begin(9600);
  // use Arduino pins
  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin, LOW);

  UI_INFO("------ MAX6675 test ------");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp

  // go to line #1
  (thermocouple.readCelsius());

  delay(1000);
}