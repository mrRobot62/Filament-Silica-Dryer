#pragma once
#include <Arduino.h>
#include "sensors.h"
#include "gfx_ui.h"

// Minimal, synchronous, zeilenbasiert. Nutzt direkt Serial.
class Commands
{
public:
    explicit Commands(Sensors &sensors);

    void begin(unsigned long baud = 115200);
    void poll(); // call frequently in loop()

private:
    Sensors &sensors_;
    String inbuf_;

    void handleLine_(const String &line);
    void reply_(const String &s);
    void doHelp_();
    void doRead_(const String &what);
    void doWrite_(const String &target, const String &state);
    void doStatus_();
};