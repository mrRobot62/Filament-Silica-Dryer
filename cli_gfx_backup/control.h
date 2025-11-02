#pragma once
#include <Arduino.h>
#include "status.h" // <-- hier kommt SystemStatus her
#include "RotarySwitch.h"

// Pins werden in cli_gfx.cpp via #define festgelegt.

// Lebenszyklus / Abtaktung
void control_init();
void control_tick(SystemStatus &out);

// Aktorsteuerung
void control_set_ssr(bool on);
void control_enable_tpm(float duty_0_1);
void control_set_tpm_window_ms(uint32_t ms);
void control_set_fan12(bool on);
void control_set_fan230(bool on);
void control_set_fanl(bool on);
void control_set_lamp(bool on);
void control_set_motor(bool on);
void control_pulse(const String &tgt, uint32_t ms);
void control_all_off();

// Nur Status-Snapshot ohne I/O-Änderung
SystemStatus control_snapshot();

// Duty in Prozent (0..100), bequem für die CLI:
void control_set_ssr_duty_percent(float pct);

// Encoder-Zähler sauber auf 0 setzen:
void control_enc_reset();

SystemStatus control_snapshot(); // legacy alias for control_get_status()
// Initialisiert Hardware (Pins), Encoder, Switch-ISR, Thermocouple, TPM-Zeitbasis.
void control_init();