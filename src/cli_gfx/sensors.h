#pragma once
#include <Arduino.h>

struct SensorSnapshot
{
    float tempC = NAN;
    bool fan12vRunning = false;
    bool heaterOn = false;
    bool fan230On = false;
    bool motorOn = false;
};

class Sensors
{
public:
    // Adjust pin numbers to your board; or move to a config header if gewünscht
    Sensors(uint8_t pinFan12vSense, uint8_t pinHeater, uint8_t pinFan230, uint8_t pinMotor);

    // Init pins/libs (call once in setup)
    void begin();

    // READ
    float readTemperatureC(); // returns NaN on failure
    bool readFan12vRunning(); // 0/1

    // WRITE
    bool setHeater(bool on);
    bool setFan230(bool on);
    bool setMotor(bool on);

    // Combined snapshot (reads what is needed; cheap)
    SensorSnapshot snapshot();

private:
    uint8_t pinFan12vSense_;
    uint8_t pinHeater_;
    uint8_t pinFan230_;
    uint8_t pinMotor_;

    // If you have a MAX31856, call into that lib in readTemperatureC()
    float readTempImpl_();

    // track states (for display without extra digitalRead on output pins)
    bool heaterOn_ = false;
    bool fan230On_ = false;
    bool motorOn_ = false;
};