#include "sensors.h"

// TODO: wire your real temperature sensor here (MAX31856 etc.)
static float readThermoC_Fake()
{
    // Placeholder: return a dummy value to make UI alive
    static float t = 25.0f;
    t += 0.05f;
    if (t > 40)
        t = 25.0f;
    return t;
}

Sensors::Sensors(uint8_t pinFan12vSense, uint8_t pinHeater, uint8_t pinFan230, uint8_t pinMotor, uint8_t pinDoor)
    : pinFan12vSense_(pinFan12vSense), pinHeater_(pinHeater), pinFan230_(pinFan230), pinMotor_(pinMotor), pinDoor_(pinDoor) {}

void Sensors::begin()
{
    pinMode(pinFan12vSense_, INPUT);
    pinMode(pinHeater_, OUTPUT);
    pinMode(pinFan230_, OUTPUT);
    pinMode(pinMotor_, OUTPUT);
    // default OFF
    digitalWrite(pinHeater_, LOW);
    digitalWrite(pinFan230_, LOW);
    digitalWrite(pinMotor_, LOW);
}

float Sensors::readTempImpl_()
{
    // Replace with real sensor call (e.g., Adafruit_MAX31856.readThermocoupleTemperature())
    return readThermoC_Fake();
}

float Sensors::readTemperatureC()
{
    return readTempImpl_(); // NAN on failure in real impl
}

bool Sensors::readDoorOpen()
{
    int v = digitalRead(pinDoor_);
    doorOpen_ = (v == HIGH);
    return doorOpen_;
}

bool Sensors::readFan12vRunning()
{
    int v = digitalRead(pinFan12vSense_);
    return (v == HIGH);
}

bool Sensors::setHeater(bool on)
{
    digitalWrite(pinHeater_, on ? HIGH : LOW);
    heaterOn_ = on;
    return true;
}

bool Sensors::setFan230(bool on)
{
    digitalWrite(pinFan230_, on ? HIGH : LOW);
    fan230On_ = on;
    return true;
}

bool Sensors::setMotor(bool on)
{
    digitalWrite(pinMotor_, on ? HIGH : LOW);
    motorOn_ = on;
    return true;
}

SensorSnapshot Sensors::snapshot()
{
    SensorSnapshot s;
    s.tempC = readTemperatureC();
    s.fan12vRunning = readFan12vRunning();
    s.heaterOn = heaterOn_;
    s.fan230On = fan230On_;
    s.motorOn = motorOn_;
    s.door_open = doorOpen_;
    return s;
}