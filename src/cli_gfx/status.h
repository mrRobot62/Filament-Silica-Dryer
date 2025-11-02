#pragma once
#include <Arduino.h>

struct SystemStatus
{
    // Inputs/Outputs
    bool door_low;
    bool ssr_on, fan5_on, fan230_on, fanl_on, lamp_on, motor_on;

    // Encoder/Switch
    long enc_count;
    int8_t enc_last_dir; // +1/-1/0
    bool sw_on;

    // TPM
    bool tpm_enabled;
    uint32_t tpm_window_ms;
    float ssr_duty_pct; // 0..100

    // Thermo
    bool thermo_ok;
    float tc, cj;
};