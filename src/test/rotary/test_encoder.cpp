#include "test/rotary/RotarySwitch.h"
#include <Arduino.h>

#ifdef ROTARYSWITCHDBG
#define RS_DBG_BEGIN(baud) Serial.begin(baud)
#define RS_DBG_PRINT(x) Serial.print(x)
#define RS_DBG_PRINTLN(x) Serial.println(x)
#define RS_DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define RS_DBG_BEGIN(baud)
#define RS_DBG_PRINT(x)
#define RS_DBG_PRINTLN(x)
#define RS_DBG_PRINTF(...)
#endif

#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 42
#define ROTARY_BTN_PIN 5 // active LOW (adjust if needed)

RotarySwitch rs(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_BTN_PIN,
                /*buttonActiveLow=*/true, RotaryEncoder::LatchMode::FOUR3,
                /*reverseDirection=*/false);

void setup() {
  Serial.begin(115200);
  rs.begin(false);                // keine internen Pullups aktivieren
  pinMode(ROTARY_BTN_PIN, INPUT); // externe/Board-Beschaltung nutzen

  rs.setButtonActiveLow(false);   // gedrückt = HIGH  (Idle = LOW)
  rs.setDebounceMs(30);           // ggf. 25–40 ms je nach Taster
  rs.setDoubleClickWindowMs(350); // 300–400 ms üblich
  rs.setLongClickSeconds(2.0f);   // Long (anpassen nach Gefühl)

  rs.setStep(1); // jede Rasterposition zählt +/- 5

  Serial.println("=== RotarySwitch Intent Test ===");
  Serial.println("Events: ShortClick, DoubleClick, LongClick");
  Serial.println("-----------------------------------------");
}

void loop() {
  rs.update();

  long pos;
  if (rs.pollPositionChange(pos)) {
    Serial.printf("Clicks: %ld\n", pos);
  }

  // Intent-first: eindeutig und robust
  if (rs.wasDoubleClicked()) {
    Serial.println("DoubleClick");
    rs.setStep(5);
  } else if (rs.wasLongClicked()) {
    Serial.println("LongClick");
    rs.setStep(15);
  } else if (rs.wasShortClicked()) {
    Serial.println("ShortClick");
    rs.setStep(1);
  }

  // Debug bei Bedarf aus:
  // if (rs.wasPressed())  Serial.println("Switch: ON (1)");
  // if (rs.wasReleased()) Serial.println("Switch: OFF (0)");

  delay(1);
}