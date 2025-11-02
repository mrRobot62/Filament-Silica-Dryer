// src/cli_gfx/control.cpp
#include <Arduino.h>
#include <Adafruit_MAX31856.h>
#include "status.h"
#include "control.h"

// -----------------------------------------------------------------------------
// Fallback-Pins, falls nicht schon global definiert.
// (Greifen nur, wenn die Symbole nirgends definiert waren.)
// -----------------------------------------------------------------------------
#ifndef PIN_SSR
#define PIN_SSR 7
#define PIN_FAN12V 6
#define PIN_FAN230 15
#define PIN_LAMP230 16
#define PIN_MOTOR230 17
#define PIN_FANL230 18
#define PIN_DOOR 19

// Encoder + Switch
#define PIN_ENC_A 20
#define PIN_ENC_B 21
#define PIN_ENC_SW 5
#define SWITCH_ACTIVE_LOW false

// MAX31856 (Software SPI)
#define PIN_TC_SCK 40
#define PIN_TC_MISO 42
#define PIN_TC_MOSI 48
#define PIN_TC_CS 47
#endif

// -----------------------------------------------------------------------------
// Thermocouple (optional vorhanden)
// -----------------------------------------------------------------------------
static Adafruit_MAX31856 s_thermo(PIN_TC_CS, PIN_TC_MOSI, PIN_TC_MISO, PIN_TC_SCK);
static bool s_thermo_ok = false;
static float s_tc = NAN, s_cj = NAN;
static uint32_t s_tc_last_ms = 0;
static const uint32_t TC_PERIOD_MS = 500; // ~2 Hz

// -----------------------------------------------------------------------------
// SSR / TPM (Time Proportional Modulation)
// -----------------------------------------------------------------------------
static bool s_tpm_enabled = false;
static float s_ssr_duty01 = 0.0f;       // 0..1
static uint32_t s_tpm_window_ms = 1000; // Standard
static uint32_t s_window_start_ms = 0;

// -----------------------------------------------------------------------------
// RotarySwitch (basiert auf RotaryEncoder)
// zusätzliche funktionen DoubleClick, LongClick
// -----------------------------------------------------------------------------
static RotarySwitch s_encoder(PIN_ENC_A, PIN_ENC_B, PIN_ENC_SW, SWITCH_ACTIVE_LOW, RotaryEncoder::LatchMode::TWO03);
static volatile long s_enc_pos_last = 0;
static volatile int8_t s_enc_last_dir = 0; // +1 / -1 / 0 bei “keine Bewegung”

// static void IRAM_ATTR isr_enc()
// {
//     s_encoder.tick(); // sehr kurz, ISR-sicher in der Lib
// }

// Switch (aktiv LOW)
static bool s_sw_on = false;

// -----------------------------------------------------------------------------
// Helper
// -----------------------------------------------------------------------------
static inline bool door_is_open() { return digitalRead(PIN_DOOR) == HIGH; } // HIGH=open

static inline void ssr_on() { digitalWrite(PIN_SSR, HIGH); }
static inline void ssr_off() { digitalWrite(PIN_SSR, LOW); }

static void apply_tpm()
{
    if (!s_tpm_enabled)
        return;
    if (door_is_open())
    {
        ssr_off();
        return;
    } // Safety

    const uint32_t now = millis();
    uint32_t elapsed = now - s_window_start_ms;

    if (elapsed >= s_tpm_window_ms)
    {
        s_window_start_ms = now;
        elapsed = 0;
        if (s_ssr_duty01 > 0.0f)
            ssr_on();
        else
            ssr_off();
    }
    const uint32_t on_ms = (uint32_t)(s_ssr_duty01 * s_tpm_window_ms);
    if (elapsed >= on_ms)
        ssr_off();
}

static void update_thermo()
{
    const uint32_t now = millis();
    if (!s_thermo_ok)
        return;
    if (now - s_tc_last_ms < TC_PERIOD_MS)
        return;

    s_tc_last_ms = now;
    s_tc = s_thermo.readThermocoupleTemperature();
    s_cj = s_thermo.readCJTemperature();
}

// -----------------------------------------------------------------------------
// API-Implementierung (control.h)
// -----------------------------------------------------------------------------
void control_init()
{
    // Ausgänge default LOW
    pinMode(PIN_SSR, OUTPUT);
    digitalWrite(PIN_SSR, LOW);
    pinMode(PIN_FAN12V, OUTPUT);
    digitalWrite(PIN_FAN12V, LOW);
    pinMode(PIN_FAN230, OUTPUT);
    digitalWrite(PIN_FAN230, LOW);
    pinMode(PIN_FANL230, OUTPUT);
    digitalWrite(PIN_FANL230, LOW);
    pinMode(PIN_LAMP230, OUTPUT);
    digitalWrite(PIN_LAMP230, LOW);
    pinMode(PIN_MOTOR230, OUTPUT);
    digitalWrite(PIN_MOTOR230, LOW);

    // Eingänge
    pinMode(PIN_DOOR, INPUT_PULLUP);

    // Encoder + Switch
    pinMode(PIN_ENC_A, INPUT_PULLUP);
    pinMode(PIN_ENC_B, INPUT_PULLUP);
    pinMode(PIN_ENC_SW, INPUT_PULLUP);

    // RotarySwitch initialisieren
    s_encoder.begin();
    s_encoder.setPosition(0);
    s_encoder.setLongClickSeconds(2.5f);
    s_encoder.setDoubleClickWindowMs(350);
    s_enc_pos_last = 0;
    s_enc_last_dir = 0;

    // Switch initial lesen
    s_sw_on = s_encoder.isPressed() == LOW; // aktiv LOW

    // Thermocouple
    s_thermo_ok = s_thermo.begin();
    if (s_thermo_ok)
    {
        s_thermo.setThermocoupleType(MAX31856_TCTYPE_K);
        s_thermo.setNoiseFilter(MAX31856_NOISE_FILTER_50HZ);
    }

    // TPM start
    s_window_start_ms = millis();
}

void control_tick(SystemStatus &out)
{

    // Immer ticken (Polling)
    s_encoder.update();
    long pos;
    if (s_encoder.pollPositionChange(pos))
    {
        if (pos < s_enc_pos_last)
        {
            Serial.printf("CCW (%ld)\n", pos);
            s_enc_pos_last = pos;
            out.enc_count = pos;
            out.enc_last_dir = -1;
        }
        else if (pos > s_enc_pos_last)
        {
            Serial.printf("CW (%ld)\n", pos);
            s_enc_pos_last = pos;
            s_enc_pos_last = pos;
            out.enc_count = pos;
            out.enc_last_dir = +1;
        }
    }
    out.sw_on = s_encoder.isPressed();

    // Richtu
    // TPM
    apply_tpm();

    // Thermo (optional)
    update_thermo();

    // Snapshot ins 'out'
    out.door_low = (digitalRead(PIN_DOOR) == LOW);
    out.ssr_on = (digitalRead(PIN_SSR) == HIGH);
    out.fan5_on = (digitalRead(PIN_FAN12V) == HIGH);
    out.fan230_on = (digitalRead(PIN_FAN230) == HIGH);
    out.fanl_on = (digitalRead(PIN_FANL230) == HIGH);
    out.lamp_on = (digitalRead(PIN_LAMP230) == HIGH);
    out.motor_on = (digitalRead(PIN_MOTOR230) == HIGH);

    out.enc_count = pos;
    out.enc_last_dir = s_enc_last_dir;
    out.sw_on = s_sw_on;

    out.thermo_ok = s_thermo_ok;
    out.tc = s_tc;
    out.cj = s_cj;
}

SystemStatus control_snapshot()
{
    SystemStatus s{};
    control_tick(s); // einheitlicher Weg
    return s;
}

// --- Outputs / Control -------------------------------------------------------
void control_set_ssr(bool on)
{
    s_tpm_enabled = false;
    if (on && door_is_open())
    {
        ssr_off();
        return;
    }
    digitalWrite(PIN_SSR, on ? HIGH : LOW);
}

void control_set_ssr_duty_percent(float pct)
{
    if (pct < 0)
        pct = 0;
    if (pct > 100)
        pct = 100;
    if (pct == 0)
    {
        s_tpm_enabled = false;
        ssr_off();
        s_ssr_duty01 = 0.0f;
    }
    else
    {
        if (door_is_open())
        { // Safety: kein Start bei offener Tür
            s_tpm_enabled = false;
            ssr_off();
            s_ssr_duty01 = 0.0f;
            return;
        }
        s_ssr_duty01 = pct / 100.0f;
        s_tpm_enabled = true;
        s_window_start_ms = millis();
        ssr_on(); // Zyklusbeginn: sofort an
    }
}

void control_set_tpm_window_ms(uint32_t ms)
{
    if (ms < 200)
        ms = 200;
    if (ms > 5000)
        ms = 5000;
    s_tpm_window_ms = ms;
}

void control_enable_tpm(bool enable)
{
    s_tpm_enabled = enable && (s_ssr_duty01 > 0.0f);
    if (!s_tpm_enabled)
        ssr_off();
    else
    {
        s_window_start_ms = millis();
        ssr_on();
    }
}

void control_set_fan12(bool on) { digitalWrite(PIN_FAN12V, on ? HIGH : LOW); }
void control_set_fan230(bool on) { digitalWrite(PIN_FAN230, on ? HIGH : LOW); }
void control_set_fanl(bool on) { digitalWrite(PIN_FANL230, on ? HIGH : LOW); }
void control_set_lamp(bool on) { digitalWrite(PIN_LAMP230, on ? HIGH : LOW); }
void control_set_motor(bool on) { digitalWrite(PIN_MOTOR230, on ? HIGH : LOW); }

void control_all_off()
{
    s_tpm_enabled = false;
    ssr_off();
    digitalWrite(PIN_FAN12V, LOW);
    digitalWrite(PIN_FAN230, LOW);
    digitalWrite(PIN_FANL230, LOW);
    digitalWrite(PIN_LAMP230, LOW);
    digitalWrite(PIN_MOTOR230, LOW);
}

void control_pulse(const String &tgt, uint32_t ms)
{
    if (ms < 10)
        ms = 10;
    if (ms > 5000)
        ms = 5000;

    auto pulse = [&](int pin)
    {
        digitalWrite(pin, HIGH);
        delay(ms);
        digitalWrite(pin, LOW);
    };

    if (tgt == "ssr")
    {
        if (door_is_open())
            return;
        pulse(PIN_SSR);
    }
    else if (tgt == "fan5")
    {
        pulse(PIN_FAN12V);
    }
    else if (tgt == "fan230")
    {
        pulse(PIN_FAN230);
    }
    else if (tgt == "fanl")
    {
        pulse(PIN_FANL230);
    }
    else if (tgt == "lamp")
    {
        pulse(PIN_LAMP230);
    }
    else if (tgt == "motor")
    {
        pulse(PIN_MOTOR230);
    }
}

void control_enc_reset()
{
    s_encoder.setPosition(0);
    s_enc_pos_last = 0;
    s_enc_last_dir = 0;
}