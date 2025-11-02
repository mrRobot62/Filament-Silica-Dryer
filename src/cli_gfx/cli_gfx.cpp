#include <Arduino.h>
#include "gfx_ui.h"

// -----------------------------------------------------------------------------
// Test selection via compile-time flags
// Define one of the TEST_* flags to run a specific test scenario.
// Example: build with -DTEST_ROTARY
// -----------------------------------------------------------------------------

#ifdef TEST_ROTARY
#include "RotarySwitch.h"
#include "rotary_input.h"

#ifndef PIN_ROT_A
#define PIN_ROT_A 2
#endif
#ifndef PIN_ROT_B
#define PIN_ROT_B 42
#endif
#ifndef PIN_ROT_SW
#define PIN_ROT_SW 5
#endif

RotarySwitch gRot(PIN_ROT_A, PIN_ROT_B, PIN_ROT_SW, /*buttonActiveLow=*/false);
RotaryInput gRotInput(gRot);

static void uiInit()
{
    gfx_begin();
    gfx_show(NAN, /*fan12v*/ false, /*heater*/ false, /*fan230*/ false, /*motor*/ false, /*fault*/ 0);
    gfx_log("Rotary test ready");
}

static void testRotary()
{
    static uint32_t lastPrint = 0;
    RotaryEvent ev{};

    if (gRotInput.poll(ev))
    {
        if (ev.moved)
        {
            String msg = String("Moved: ") + ev.delta + " | Pos: " + ev.newPosition;
            gfx_log(msg.c_str());
            Serial.println(msg);
        }
        if (ev.evPressed)
        {
            gfx_log("Button: PRESS");
            Serial.println(F("Button: PRESS"));
        }
        if (ev.evReleased)
        {
            gfx_log("Button: RELEASE");
            Serial.println(F("Button: RELEASE"));
        }
        if (ev.evDoubleClick)
        {
            gfx_log("Button: DOUBLE");
            Serial.println(F("Button: DOUBLE"));
        }
        if (ev.evLongClick)
        {
            gfx_log("Button: LONG");
            Serial.println(F("Button: LONG"));
        }
    }

    uint32_t now = millis();
    if (now - lastPrint > 5000)
    {
        lastPrint = now;
        Serial.println(F("[RotaryTest alive]"));
    }

    // Keep the UI up
    gfx_show(NAN, /*fan12v*/ false, /*heater*/ false, /*fan230*/ false, /*motor*/ false, /*fault*/ 0);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(5);
    }

    Serial.println(F("=== CLI_GFX: TEST_ROTARY ==="));
    gRotInput.begin();
    uiInit();

    Serial.println(F("Rotate the encoder or press the button."));
}

void loop()
{
    testRotary();
    delay(5);
}

#else

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(5);
    }
    Serial.println(F("=== CLI_GFX: idle (no TEST_* selected) ==="));

    gfx_begin();
    gfx_log("No test selected");
    gfx_show(NAN, false, false, false, false, 0);
}

void loop()
{
    gfx_show(NAN, false, false, false, false, 0);
    delay(50);
}

#endif
