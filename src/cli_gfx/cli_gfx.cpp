// ============================================================================
// Filament-Dryer ESP32-S3 – Serial CLI + Orchestrierung (split: control/gfx)
// ============================================================================
#include "status.h"
#include "control.h"
#include "gfx_ui.h"
#include <Arduino.h>

static String g_line;
static SystemStatus g_status; // zentraler Snapshot für UI & CLI

static void print_help()
{
    Serial.println(F("\nCommands:"));
    Serial.println(F("  help                      - show this help"));
    Serial.println(F("  stat                      - print current states"));
    Serial.println(F("  refresh                   - reinit screen + redraw"));
    Serial.println(F("  temp                      - read thermocouple"));
    Serial.println(F("  door                      - read door (LOW=closed, HIGH=open)"));
    Serial.println(F("  ssr on|off                - heater SSR direct on/off"));
    Serial.println(F("  ssr duty <0-100>          - enable TPM %"));
    Serial.println(F("  window <ms>               - set TPM window (200..5000)"));
    Serial.println(F("  fan5 on|off               - 5V fan"));
    Serial.println(F("  fan230 on|off             - 230V fan"));
    Serial.println(F("  fanl on|off               - 230V fan-L"));
    Serial.println(F("  lamp on|off               - 230V lamp"));
    Serial.println(F("  motor on|off              - 230V motor"));
    Serial.println(F("  pulse <tgt> <ms>          - tgt: ssr|fan5|fan230|fanl|lamp|motor"));
    Serial.println(F("  enc stat|reset            - encoder info / reset"));
    Serial.println(F("  alloff                    - switch off all outputs"));
}

static void print_stat_serial(const SystemStatus &s)
{
    Serial.println(F("\n--- System Status ---"));
    Serial.printf("Door : %s\n", s.door_low ? "CLOSED (LOW)" : "OPEN (HIGH)");
    Serial.printf("SSR  : %s\n", s.ssr_on ? "ON" : "OFF");
    Serial.printf("FAN5 : %s\n", s.fan5_on ? "ON" : "OFF");
    Serial.printf("FAN230: %s\n", s.fan230_on ? "ON" : "OFF");
    Serial.printf("LAMP : %s\n", s.lamp_on ? "ON" : "OFF");
    Serial.printf("MOTOR: %s\n", s.motor_on ? "ON" : "OFF");
    Serial.printf("FAN-L: %s\n", s.fanl_on ? "ON" : "OFF");
    Serial.printf("Encoder: count=%ld last=%s\n", s.enc_count,
                  (s.enc_last_dir > 0) ? "CW" : (s.enc_last_dir < 0) ? "CCW"
                                                                     : "N/A");
    Serial.printf("Switch : %s\n", s.sw_on ? "ON" : "OFF");
    if (s.thermo_ok && !isnan(s.tc))
    {
        Serial.printf("Temp: %.2f C (CJ: %.2f C)\n", s.tc, s.cj);
    }
    else
    {
        Serial.println(F("Temp: N/A"));
    }
    Serial.println(F("----------------------\n"));
}

static void parse_line(String s)
{
    s.trim();
    if (s.isEmpty())
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
        print_stat_serial(control_snapshot());
    }
    else if (cmd == "refresh")
    {
        gfx_init();
        gfx_draw_full(control_snapshot());
        Serial.println(F("Screen refreshed."));
    }
    else if (cmd == "temp")
    {
        auto s2 = control_snapshot();
        if (!s2.thermo_ok)
            Serial.println(F("MAX31856 not initialized / not found."));
        else
            Serial.printf("Thermo: %.2f C (CJ: %.2f C)\n", s2.tc, s2.cj);
    }
    else if (cmd == "door")
    {
        auto s2 = control_snapshot();
        Serial.println(s2.door_low ? F("CLOSED (LOW)") : F("OPEN (HIGH)"));
    }
    else if (cmd == "ssr")
    {
        if (a1 == "on")
        {
            control_set_ssr(true);
            Serial.println(F("SSR ON"));
        }
        else if (a1 == "off")
        {
            control_set_ssr(false);
            Serial.println(F("SSR OFF"));
        }
        else if (a1 == "duty")
        {
            float pct = a2.toFloat();
            if (pct < 0)
                pct = 0;
            if (pct > 100)
                pct = 100;
            control_set_ssr_duty_percent(pct);
            Serial.printf("SSR duty set to %.1f%%\n", pct);
        }
        else
            Serial.println(F("Usage: ssr on|off | ssr duty <0-100>"));
    }
    else if (cmd == "window")
    {
        long w = a1.toInt();
        control_set_tpm_window_ms((uint32_t)w);
        Serial.printf("TPM window = %ld ms\n", w);
    }
    else if (cmd == "fan5")
    {
        control_set_fan5(a1 == "on");
        Serial.printf("fan5 %s\n", a1.c_str());
    }
    else if (cmd == "fan230")
    {
        control_set_fan230(a1 == "on");
        Serial.printf("fan230 %s\n", a1.c_str());
    }
    else if (cmd == "fanl")
    {
        control_set_fanl(a1 == "on");
        Serial.printf("fanl %s\n", a1.c_str());
    }
    else if (cmd == "lamp")
    {
        control_set_lamp(a1 == "on");
        Serial.printf("lamp %s\n", a1.c_str());
    }
    else if (cmd == "motor")
    {
        control_set_motor(a1 == "on");
        Serial.printf("motor %s\n", a1.c_str());
    }
    else if (cmd == "pulse")
    {
        uint32_t ms = (uint32_t)a2.toInt();
        control_pulse(a1, ms);
    }
    else if (cmd == "enc")
    {
        if (a1 == "stat")
        {
            auto s2 = control_snapshot();
            Serial.printf("Encoder: count=%ld last=%s\n", s2.enc_count,
                          (s2.enc_last_dir > 0) ? "CW" : (s2.enc_last_dir < 0) ? "CCW"
                                                                               : "N/A");
            Serial.printf("Switch : %s\n", s2.sw_on ? "ON" : "OFF");
        }
        else if (a1 == "reset")
        {
            control_enc_reset(); // <--- sauber über API
            Serial.println(F("Encoder count reset."));
        }
        else
        {
            Serial.println(F("Usage: enc stat | enc reset"));
        }
    }
    else if (cmd == "alloff")
    {
        control_all_off();
        Serial.println(F("All outputs OFF."));
    }
    else
    {
        Serial.println(F("Unknown command. Type 'help'."));
    }

    Serial.print(F("> "));
}

void setup()
{
    Serial.begin(115200);
    delay(50);
    control_init(); // Pins, Encoder-ISR, TPM, Thermo
    gfx_init();     // Display initialisieren
    control_tick(g_status);
    gfx_draw_full(g_status);

    Serial.println();
    Serial.println(F("=== Filament-Dryer – CLI (split: control/gfx) ==="));
    print_help();
    Serial.print(F("\n> "));
}

void loop()
{
    control_tick(g_status);
    gfx_tick(g_status);

    // kleiner Eventlog für Encoder
    static long lastPrintedCount = LONG_MIN;
    auto s = control_snapshot();
    if (s.enc_count != lastPrintedCount)
    {
        long base = (lastPrintedCount == LONG_MIN) ? s.enc_count : lastPrintedCount;
        long delta = s.enc_count - base;
        if (delta != 0)
            Serial.printf("Encoder: %+ld (%s)\r\n", delta, (delta > 0) ? "CW" : "CCW");
        lastPrintedCount = s.enc_count;
    }

    // Mini-CLI
    while (Serial.available())
    {
        char c = (char)Serial.read();
        if (c == '\r')
            continue;
        if (c == '\n')
        {
            Serial.println();
            String cmd = g_line;
            g_line.remove(0);
            parse_line(cmd);
        }
        else if (c == '\b' || c == 127)
        {
            if (g_line.length() > 0)
            {
                g_line.remove(g_line.length() - 1);
                Serial.print("\b \b");
            }
        }
        else
        {
            g_line += c;
            Serial.print(c);
            if (g_line.length() > 160)
            {
                g_line.remove(0);
                Serial.println(F("\n(Line cleared: too long)"));
                Serial.print(F("> "));
            }
        }
    }
}