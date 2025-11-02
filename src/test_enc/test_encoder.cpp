#include <Arduino.h>
#include "RotarySwitch.h"

// A=2, B=42, SW=5, buttonActiveLow=false (pressed = HIGH)
RotarySwitch rs(2, 42, 5, /*buttonActiveLow*/ false);

void setup()
{
    Serial.begin(115200);
    delay(50);

    rs.begin();                   // configure pins & internal state
    rs.setLongClickSeconds(2.5f); // your long-press threshold
    rs.setDoubleClickWindowMs(350);

    Serial.println("=== RotarySwitch Demo ===");
    Serial.println("Rotate: CW +1 / CCW -1");
    Serial.println("Button: ON(1)=pressed, OFF(0)=released");
    Serial.println("Events: DoubleClick, LongClick");
    Serial.println("-------------------------");
}

void loop()
{
    rs.update();

    long pos;
    if (rs.pollPositionChange(pos))
    {
        Serial.printf("Clicks: %ld\n", pos);
    }

    if (rs.wasPressed())
        Serial.println("Switch: ON (1)");
    if (rs.wasReleased())
        Serial.println("Switch: OFF (0)");

    if (rs.wasDoubleClicked())
        Serial.println("DoubleClick");
    if (rs.wasLongClicked())
        Serial.println("LongClick");

    delay(1);
}