#pragma once
#include <Arduino.h>
#include "RotarySwitch.h"

struct RotaryEvent {
    long newPosition = 0;    // absolute
    long delta       = 0;    // since last poll
    bool moved       = false;

    bool pressed       = false;  // current state
    bool evPressed     = false;  // rising edge
    bool evReleased    = false;  // falling edge
    bool evDoubleClick = false;
    bool evLongClick   = false;
};

class RotaryInput {
public:
    explicit RotaryInput(RotarySwitch& rs) : rs_(rs) {}

    void begin(bool useInternalPullups = true,
               uint32_t debounceMs = 15,
               uint32_t dClickMs = 350,
               uint32_t longMs = 2500)
    {
        rs_.begin(useInternalPullups);
        rs_.setDebounceMs(debounceMs);
        rs_.setDoubleClickWindowMs(dClickMs);
        rs_.setLongClickMs(longMs);
        lastPos_ = rs_.getPosition();
        lastPressed_ = rs_.isPressed();
    }

    // Non-blocking; returns true if any movement or button event happened
    bool poll(RotaryEvent& ev) {
        // VERY IMPORTANT: drive the FSM and encoder by updating first
        rs_.update();

        // Position / delta
        long pos = rs_.getPosition();
        ev.newPosition = pos;
        ev.delta = pos - lastPos_;
        ev.moved = (ev.delta != 0);
        if (ev.moved) lastPos_ = pos;

        // Button edges / state (update() must have run before these calls)
        ev.pressed       = rs_.isPressed();
        ev.evPressed     = rs_.wasPressed();
        ev.evReleased    = rs_.wasReleased();
        ev.evDoubleClick = rs_.wasDoubleClicked();
        ev.evLongClick   = rs_.wasLongClicked();

        bool changed = ev.moved || ev.evPressed || ev.evReleased || ev.evDoubleClick || ev.evLongClick;
        if (ev.pressed != lastPressed_) { lastPressed_ = ev.pressed; changed = true; }
        return changed;
    }

private:
    RotarySwitch& rs_;
    long  lastPos_{0};
    bool  lastPressed_{false};
};
