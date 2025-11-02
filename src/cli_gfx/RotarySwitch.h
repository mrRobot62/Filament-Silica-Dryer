#pragma once
#include <Arduino.h>
#include <RotaryEncoder.h>

/**
 * @class RotarySwitch
 * @brief High-level wrapper around RotaryEncoder + a debounced push-button
 *        with robust event detection (single press, double-click, long-press).
 *
 * Features:
 *  - Fully debounced button state machine (no false triggers on boot)
 *  - Parameterized long-click threshold and double-click window
 *  - RotaryEncoder integration with simple pollPositionChange()
 *  - No interrupts required
 *
 * Typical usage:
 * @code
 *   #include "RotarySwitch.h"
 *   RotarySwitch rs(2, 42, 5, false); // A=2, B=42, SW=5, button active-high
 *
 *   void setup() {
 *     Serial.begin(115200);
 *     rs.begin();
 *   }
 *
 *   void loop() {
 *     rs.update();
 *     long pos;
 *     if (rs.pollPositionChange(pos)) Serial.printf("Pos: %ld\n", pos);
 *     if (rs.wasPressed()) Serial.println("Pressed");
 *     if (rs.wasReleased()) Serial.println("Released");
 *     if (rs.wasDoubleClicked()) Serial.println("DoubleClick");
 *     if (rs.wasLongClicked()) Serial.println("LongClick");
 *   }
 * @endcode
 */
class RotarySwitch
{
public:
    RotarySwitch(uint8_t pinA,
                 uint8_t pinB,
                 uint8_t pinSW,
                 bool buttonActiveLow = true,
                 RotaryEncoder::LatchMode latch = RotaryEncoder::LatchMode::FOUR3);

    void begin(bool useInternalPullups = true);
    void update();

    // --- Encoder ---
    long getPosition();
    void setPosition(long pos);
    bool pollPositionChange(long &outNewPosition);

    // --- Button configuration ---
    void setLongClickMs(uint32_t ms);
    void setLongClickSeconds(float seconds);
    void setDoubleClickWindowMs(uint32_t ms);
    void setDebounceMs(uint32_t ms);
    void setButtonActiveLow(bool activeLow);

    // --- Button events & state ---
    bool isPressed() const;
    bool wasPressed();
    bool wasReleased();
    bool wasDoubleClicked();
    bool wasLongClicked();

private:
    enum BtnState : uint8_t
    {
        RELEASED,
        PRESSED,
        LONGED
    };

    struct ButtonFSM
    {
        bool raw = false;
        bool stable = false;
        uint32_t rawChange = 0;
        BtnState state = RELEASED;
        bool armed = false;
        uint32_t pressMs = 0;
        uint32_t lastShortReleaseMs = 0;
        bool evPress = false;
        bool evRelease = false;
        bool evLong = false;
        bool evDouble = false;
    } _btn;

    const uint8_t _pinA, _pinB, _pinSW;
    bool _activeLow;
    RotaryEncoder _encoder;

    uint32_t _debounceMs = 15;
    uint32_t _dclickWindowMs = 350;
    uint32_t _longMs = 2500;
    long _lastReportedPos = 0;

    inline bool readRawPressed() const;
    void updateButtonFSM();
};