/**
 * ============================================================================
 *  Filament-Dryer ESP32 (ESP32-S3) – Serial CLI Test Utility (with Encoder)
 *  --------------------------------------------------------------------------
 *  Features:
 *    - Serial CLI with local echo and backspace handling
 *    - Heater SSR: direct ON/OFF or time-proportional modulation (TPM)
 *    - 230 V outputs: FAN (P7), FAN-L (P10), LAMP (P8), MOTOR (P9)
 *    - 5 V fan (P5)
 *    - Door switch (active LOW)
 *    - K-Type thermocouple via MAX31856 (software SPI)
 *    - NEW: Rotary encoder (A/B) + push switch (ON/OFF), event-driven prints:
 *           "Switch: ON/OFF", "Encoder: +N ticks (CW) / -N ticks (CCW)"
 *
 *  Safety:
 *    - SSR blocked when DOOR is OPEN (HIGH).
 *    - High voltage switching – operate with caution.
 *
 *  Build:
 *    - Place as src/test_cli.cpp (Arduino/PlatformIO).
 *    - Baud 115200.
 * ============================================================================
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MAX31856.h>

// ============================ Pin Mapping ===================================
// Match your hardware (USB CDC disabled => GPIO19/20 usable as GPIO)
#define PIN_SSR 7
#define PIN_FAN5V 6
#define PIN_FAN230 15   // P7
#define PIN_LAMP230 16  // P8
#define PIN_MOTOR230 17 // P9
#define PIN_FANL230 18  // P10
#define PIN_DOOR 19     // active LOW: LOW=closed, HIGH=open

// Rotary encoder (quadrature) + push switch
#define PIN_ENC_A 20 // encoder channel A (use INPUT_PULLUP)
#define PIN_ENC_B 21 // encoder channel B (use INPUT_PULLUP)
#define PIN_ENC_SW 5 // encoder push switch (active LOW, INPUT_PULLUP)

// MAX31856 (software SPI), separate from LCD SPI
#define PIN_SPI_SCK 40
#define PIN_SPI_MISO 42
#define PIN_SPI_MOSI 48
#define PIN_MAX_CS 47

// ============================ Defaults / Limits ==============================
#define SERIAL_BAUD 115200
#define TPM_WINDOW_MS_DEFAULT 1000
#define TPM_WINDOW_MS_MIN 200
#define TPM_WINDOW_MS_MAX 5000
#define PULSE_MS_MIN 10
#define PULSE_MS_MAX 5000

// ============================ Thermocouple ==================================
static Adafruit_MAX31856 thermo(PIN_MAX_CS, PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK);
static bool g_thermo_ok = false;

// ============================ CLI State =====================================
static String g_line; // input line buffer
static uint32_t g_tpm_window_ms = TPM_WINDOW_MS_DEFAULT;
static float g_ssr_duty = 0.0f; // 0..1
static uint32_t g_window_start_ms = 0;
static bool g_tpm_enabled = false; // TPM active if true

// ============================ Encoder State =================================
// Quadrature decoder: we use a transition table on CHANGE interrupts for A & B.
// We protect against mechanical bounce with a short microsecond guard.
static volatile long g_enc_count = 0;        // accumulated ticks (+CW / -CCW)
static volatile int8_t g_enc_last_dir = 0;   // +1 = CW, -1 = CCW, 0 = none yet
static volatile uint8_t g_enc_state = 0;     // last AB state: bit0=A, bit1=B
static volatile uint32_t g_enc_last_us = 0;  // last ISR timestamp for debounce
static const uint16_t ENC_DEBOUNCE_US = 300; // ignore transitions <300us apart

// Push switch state (active LOW)
static volatile bool g_sw_is_on = false;   // true when pressed (LOW)
static volatile bool g_sw_changed = false; // edge flag for printing

// ============================ Helpers =======================================

static inline int door_read() { return digitalRead(PIN_DOOR); } // HIGH=open

static inline void ssr_on() { digitalWrite(PIN_SSR, HIGH); }
static inline void ssr_off() { digitalWrite(PIN_SSR, LOW); }

static void all_off()
{
    g_tpm_enabled = false;
    ssr_off();
    digitalWrite(PIN_FAN5V, LOW);
    digitalWrite(PIN_FAN230, LOW);
    digitalWrite(PIN_FANL230, LOW);
    digitalWrite(PIN_LAMP230, LOW);
    digitalWrite(PIN_MOTOR230, LOW);
}

/** Return "ON"/"OFF" for digitalRead state */
static const char *onoff(int state) { return (state == HIGH) ? "ON" : "OFF"; }

/** TPM engine for SSR (non-blocking) */
static void apply_ssr_tpm()
{
    if (!g_tpm_enabled)
        return;
    if (door_read() == HIGH)
    {
        ssr_off();
        return;
    } // safety: door open => off

    const uint32_t now = millis();
    uint32_t elapsed = now - g_window_start_ms;

    if (elapsed >= g_tpm_window_ms)
    {
        g_window_start_ms = now;
        elapsed = 0;
        if (g_ssr_duty > 0.0f)
            ssr_on();
        else
            ssr_off();
    }
    const uint32_t on_ms = (uint32_t)(g_ssr_duty * g_tpm_window_ms);
    if (elapsed >= on_ms)
        ssr_off();
}

/** Pretty banner + help */
static void print_banner()
{
    Serial.println();
    Serial.println(F("=== Filament-Dryer ESP32 – Serial CLI Test (Encoder) ==="));
    Serial.println(F("Type 'help' for commands. SSR blocked if DOOR is OPEN."));
}

static void print_help()
{
    Serial.println(F("\nCommands:"));
    Serial.println(F("  help                      - show this help"));
    Serial.println(F("  stat                      - show pin states, door, temp, encoder"));
    Serial.println(F("  temp                      - read MAX31856 temperature"));
    Serial.println(F("  door                      - read door input (LOW=closed, HIGH=open)"));
    Serial.println(F("  ssr on|off                - heater SSR on/off (direct)"));
    Serial.println(F("  ssr duty <0-100>          - enable TPM, set duty percent"));
    Serial.println(F("  window <ms>               - set TPM window (200..5000, default 1000)"));
    Serial.println(F("  fan5 on|off               - 5V fan (P5)"));
    Serial.println(F("  fan230 on|off             - 230V fan (P7)"));
    Serial.println(F("  fanl on|off               - 230V fan-L (P10)"));
    Serial.println(F("  lamp on|off               - 230V lamp (P8)"));
    Serial.println(F("  motor on|off              - 230V motor (P9)"));
    Serial.println(F("  pulse <tgt> <ms>          - pulse target: ssr|fan5|fan230|fanl|lamp|motor"));
    Serial.println(F("  enc stat                  - show encoder count & last direction"));
    Serial.println(F("  enc reset                 - reset encoder count to zero"));
    Serial.println(F("  alloff                    - switch off all outputs"));
}

/** Print system status in human-friendly form (ON/OFF labels) */
static void print_stat()
{
    float tc = NAN, cj = NAN;
    if (g_thermo_ok)
    {
        tc = thermo.readThermocoupleTemperature();
        cj = thermo.readCJTemperature();
    }

    // Snapshot volatile to avoid tearing while printing
    long encCount = g_enc_count;
    int8_t encDir = g_enc_last_dir;
    bool swOn = g_sw_is_on;

    Serial.println(F("\n--- System Status ---"));
    Serial.printf("Door: %s\n", (door_read() == LOW) ? "CLOSED (LOW)" : "OPEN (HIGH)");
    Serial.printf("SSR: %s  | duty=%.1f%%  | window=%ums  | TPM=%s\n",
                  onoff(digitalRead(PIN_SSR)),
                  g_ssr_duty * 100.0f, g_tpm_window_ms,
                  g_tpm_enabled ? "ENABLED" : "DISABLED");

    Serial.println(F("Outputs:"));
    Serial.printf("  FAN5V (P5):   %s\n", onoff(digitalRead(PIN_FAN5V)));
    Serial.printf("  FAN230 (P7):  %s\n", onoff(digitalRead(PIN_FAN230)));
    Serial.printf("  LAMP  (P8):   %s\n", onoff(digitalRead(PIN_LAMP230)));
    Serial.printf("  MOTOR (P9):   %s\n", onoff(digitalRead(PIN_MOTOR230)));
    Serial.printf("  FAN-L (P10):  %s\n", onoff(digitalRead(PIN_FANL230)));

    const char *dirStr = (encDir > 0) ? "CW" : (encDir < 0 ? "CCW" : "N/A");
    Serial.printf("Encoder: count=%ld  last=%s\n", encCount, dirStr);
    Serial.printf("Switch:  %s\n", swOn ? "ON" : "OFF");

    if (!isnan(tc))
    {
        Serial.printf("Temp: %.2f °C  (ColdJunction: %.2f °C)\n", tc, cj);
    }
    else if (g_thermo_ok)
    {
        Serial.println(F("Temp: read failed (NaN)."));
    }
    else
    {
        Serial.println(F("Temp: MAX31856 not initialized or not connected."));
    }
    Serial.println(F("----------------------\n"));
}

/** Pulse helper with clamped duration */
static void cmd_pulse(const String &tgt, uint32_t ms)
{
    if (ms < PULSE_MS_MIN)
        ms = PULSE_MS_MIN;
    if (ms > PULSE_MS_MAX)
        ms = PULSE_MS_MAX;

    auto pulse = [&](int pin)
    {
        digitalWrite(pin, HIGH);
        delay(ms);
        digitalWrite(pin, LOW);
    };

    if (tgt == "ssr")
    {
        if (door_read() == HIGH)
        {
            Serial.println(F("Door OPEN → SSR blocked."));
            return;
        }
        pulse(PIN_SSR);
    }
    else if (tgt == "fan5")
        pulse(PIN_FAN5V);
    else if (tgt == "fan230")
        pulse(PIN_FAN230);
    else if (tgt == "fanl")
        pulse(PIN_FANL230);
    else if (tgt == "lamp")
        pulse(PIN_LAMP230);
    else if (tgt == "motor")
        pulse(PIN_MOTOR230);
    else
        Serial.println(F("Unknown target. Use: ssr|fan5|fan230|fanl|lamp|motor"));
}

// ============================ Encoder ISR ===================================
// 2-bit state machine with transition table to derive +1 / -1 steps.
// States: 0..3 map to AB = 00, 01, 10, 11 (bit0=A, bit1=B)
static inline uint8_t readAB()
{
    uint8_t a = (uint8_t)digitalRead(PIN_ENC_A);
    uint8_t b = (uint8_t)digitalRead(PIN_ENC_B);
    return (uint8_t)((b << 1) | a);
}

// Transition table: rows=prev, cols=curr → delta
// Valid transitions: 00->01(+1), 01->11(+1), 11->10(+1), 10->00(+1)
// Reverse transitions give -1; others (including bounces) yield 0
static const int8_t ENC_TRANS[4][4] = {
    /*prev\curr: 00  01  10  11 */
    /*00*/ {0, +1, -1, 0},
    /*01*/ {-1, 0, 0, +1},
    /*10*/ {+1, 0, 0, -1},
    /*11*/ {0, -1, +1, 0}};

void IRAM_ATTR isr_encoder()
{
    uint32_t now = micros();
    // simple microsecond guard against contact bounce "storms"
    if ((now - g_enc_last_us) < ENC_DEBOUNCE_US)
        return;
    g_enc_last_us = now;

    uint8_t curr = readAB();
    uint8_t prev = g_enc_state;
    int8_t d = ENC_TRANS[prev & 0x03][curr & 0x03];

    if (d != 0)
    {
        g_enc_count = g_enc_count + d;
        g_enc_last_dir = (d > 0) ? +1 : -1;
        g_enc_state = curr;
    }
    else
    {
        // For ambiguous transitions, still update state to help resync
        g_enc_state = curr;
    }
}

void IRAM_ATTR isr_switch()
{
    // Active LOW switch: LOW=pressed(ON), HIGH=released(OFF)
    bool nowLow = (digitalRead(PIN_ENC_SW) == LOW);
    if (nowLow != g_sw_is_on)
    {
        g_sw_is_on = nowLow;
        g_sw_changed = true; // notify main loop to print status
    }
}

// ============================ CLI Parser ====================================
static void parse_line(String s)
{
    s.trim();
    if (s.length() == 0)
        return;

    String cmd, a1, a2;
    int sp = s.indexOf(' ');
    if (sp < 0)
    {
        cmd = s;
    }
    else
    {
        cmd = s.substring(0, sp);
        a1 = s.substring(sp + 1);
        a1.trim();
    }
    int sp2 = a1.indexOf(' ');
    if (sp2 >= 0)
    {
        a2 = a1.substring(sp2 + 1);
        a2.trim();
        a1 = a1.substring(0, sp2);
    }

    cmd.toLowerCase();
    a1.toLowerCase();
    a2.toLowerCase();

    if (cmd == "help" || cmd == "?")
    {
        print_help();
    }
    else if (cmd == "stat")
    {
        print_stat();
    }
    else if (cmd == "temp")
    {
        if (!g_thermo_ok)
            Serial.println(F("MAX31856 not initialized / not found."));
        else
            Serial.printf("Thermo: %.2f C (CJ: %.2f C)\n",
                          thermo.readThermocoupleTemperature(),
                          thermo.readCJTemperature());
    }
    else if (cmd == "door")
    {
        Serial.println((door_read() == LOW) ? F("CLOSED (LOW)") : F("OPEN (HIGH)"));
    }
    else if (cmd == "ssr")
    {
        if (a1 == "on")
        {
            if (door_read() == HIGH)
                Serial.println(F("Door OPEN → SSR blocked."));
            else
            {
                g_tpm_enabled = false;
                ssr_on();
                Serial.println(F("SSR ON"));
            }
        }
        else if (a1 == "off")
        {
            g_tpm_enabled = false;
            ssr_off();
            Serial.println(F("SSR OFF"));
        }
        else if (a1 == "duty")
        {
            float pct = a2.toFloat();
            if (pct < 0)
                pct = 0;
            if (pct > 100)
                pct = 100;
            if (door_read() == HIGH)
            {
                Serial.println(F("Door OPEN → SSR blocked."));
                return;
            }
            g_ssr_duty = pct / 100.0f;
            g_tpm_enabled = true;
            g_window_start_ms = millis();
            Serial.printf("SSR duty set to %.1f%% (TPM enabled)\n", pct);
        }
        else
            Serial.println(F("Usage: ssr on|off | ssr duty <0-100>"));
    }
    else if (cmd == "window")
    {
        long w = a1.toInt();
        if (w < (long)TPM_WINDOW_MS_MIN)
            w = TPM_WINDOW_MS_MIN;
        if (w > (long)TPM_WINDOW_MS_MAX)
            w = TPM_WINDOW_MS_MAX;
        g_tpm_window_ms = (uint32_t)w;
        Serial.printf("TPM window = %u ms\n", g_tpm_window_ms);
    }
    else if (cmd == "fan5")
    {
        digitalWrite(PIN_FAN5V, (a1 == "on") ? HIGH : LOW);
        Serial.printf("fan5V %s\n", a1.c_str());
    }
    else if (cmd == "fan230")
    {
        digitalWrite(PIN_FAN230, (a1 == "on") ? HIGH : LOW);
        Serial.printf("fan230 %s\n", a1.c_str());
    }
    else if (cmd == "fanl")
    {
        digitalWrite(PIN_FANL230, (a1 == "on") ? HIGH : LOW);
        Serial.printf("fanL %s\n", a1.c_str());
    }
    else if (cmd == "lamp")
    {
        digitalWrite(PIN_LAMP230, (a1 == "on") ? HIGH : LOW);
        Serial.printf("lamp %s\n", a1.c_str());
    }
    else if (cmd == "motor")
    {
        digitalWrite(PIN_MOTOR230, (a1 == "on") ? HIGH : LOW);
        Serial.printf("motor %s\n", a1.c_str());
    }
    else if (cmd == "pulse")
    {
        uint32_t ms = (uint32_t)a2.toInt();
        cmd_pulse(a1, ms);
    }
    else if (cmd == "enc")
    {
        if (a1 == "stat")
        {
            long c = g_enc_count;
            int8_t d = g_enc_last_dir;
            Serial.printf("Encoder: count=%ld  last=%s\n", c, (d > 0) ? "CW" : (d < 0) ? "CCW"
                                                                                       : "N/A");
            Serial.printf("Switch:  %s\n", g_sw_is_on ? "ON" : "OFF");
        }
        else if (a1 == "reset")
        {
            g_enc_count = 0;
            g_enc_last_dir = 0;
            Serial.println(F("Encoder count reset."));
        }
        else
        {
            Serial.println(F("Usage: enc stat | enc reset"));
        }
    }
    else if (cmd == "alloff")
    {
        all_off();
        Serial.println(F("All outputs OFF."));
    }
    else
    {
        Serial.println(F("Unknown command. Type 'help'."));
    }
}

// ============================ Arduino Entry =================================
void setup()
{
    Serial.begin(SERIAL_BAUD);
    delay(50);

    // Safe IO defaults
    pinMode(PIN_SSR, OUTPUT);
    digitalWrite(PIN_SSR, LOW);
    pinMode(PIN_FAN5V, OUTPUT);
    digitalWrite(PIN_FAN5V, LOW);
    pinMode(PIN_FAN230, OUTPUT);
    digitalWrite(PIN_FAN230, LOW);
    pinMode(PIN_FANL230, OUTPUT);
    digitalWrite(PIN_FANL230, LOW);
    pinMode(PIN_LAMP230, OUTPUT);
    digitalWrite(PIN_LAMP230, LOW);
    // Correct lamp pin:
    pinMode(PIN_LAMP230, OUTPUT);
    digitalWrite(PIN_LAMP230, LOW);
    pinMode(PIN_MOTOR230, OUTPUT);
    digitalWrite(PIN_MOTOR230, LOW);
    pinMode(PIN_DOOR, INPUT_PULLUP);

    // Encoder pins with pull-ups (mechanical encoders need it)
    pinMode(PIN_ENC_A, INPUT_PULLUP);
    pinMode(PIN_ENC_B, INPUT_PULLUP);
    pinMode(PIN_ENC_SW, INPUT_PULLUP);

    // Initialize encoder state before enabling interrupts
    g_enc_state = readAB();

    attachInterrupt(digitalPinToInterrupt(PIN_ENC_A), isr_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_B), isr_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_SW), isr_switch, CHANGE);

    // Thermocouple init (optional hardware)
    g_thermo_ok = thermo.begin();
    if (g_thermo_ok)
    {
        thermo.setThermocoupleType(MAX31856_TCTYPE_K);
        thermo.setNoiseFilter(MAX31856_NOISE_FILTER_50HZ);
    }

    g_window_start_ms = millis();

    print_banner();
    print_help();
    Serial.print("\n> ");
}

void loop()
{
    // Run SSR TPM engine
    apply_ssr_tpm();

    // --- Event-driven prints for encoder & switch -----------------------------
    static long lastPrintedCount = 0;
    static int8_t lastPrintedDir = 0;
    static bool lastSwState = false;

    // Encoder movement: print delta and direction
    long nowCount = g_enc_count;
    if (nowCount != lastPrintedCount)
    {
        long delta = nowCount - lastPrintedCount;
        int8_t dir = (delta > 0) ? +1 : -1;
        Serial.printf("Encoder: %ld ticks (%s)\r\n", delta, (dir > 0) ? "CW" : "CCW");
        lastPrintedCount = nowCount;
        lastPrintedDir = dir;
    }

    // Switch edge: print ON/OFF when changed
    if (g_sw_changed)
    {
        g_sw_changed = false;
        bool swOn = g_sw_is_on;
        Serial.printf("Switch: %s\r\n", swOn ? "ON" : "OFF");
        lastSwState = swOn;
    }

    // --- CLI line input with local echo & backspace ---------------------------
    while (Serial.available())
    {
        char c = (char)Serial.read();
        if (c == '\r')
            continue; // ignore CR
        if (c == '\n')
        {
            Serial.println(); // echo newline
            String cmd = g_line;
            g_line.remove(0);
            parse_line(cmd);
            Serial.print("> ");
        }
        else if (c == '\b' || c == 127)
        {
            if (g_line.length() > 0)
            {
                g_line.remove(g_line.length() - 1);
                Serial.print("\b \b"); // erase last char on terminal
            }
        }
        else
        {
            g_line += c;     // append char
            Serial.print(c); // local echo
            if (g_line.length() > 160)
            {
                g_line.remove(0);
                Serial.println(F("\n(Line cleared: too long)"));
                Serial.print("> ");
            }
        }
    }

    // keep loop responsive (no delay)
}