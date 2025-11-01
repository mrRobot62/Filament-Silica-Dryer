#pragma once

// ==== Hardware mapping (ESP32-1732S019) ====
#define PIN_SSR          7
#define PIN_FAN5V        6
#define PIN_FAN230       15
#define PIN_LAMP230      16
#define PIN_MOTOR230     17
#define PIN_FANL230      18
#define PIN_DOOR         19

#define PIN_ENC_A        20
#define PIN_ENC_B        21
#define PIN_ENC_SW       5

// MAX31856 (SPI)
#define PIN_SPI_SCK      40
#define PIN_SPI_MISO     42
#define PIN_SPI_MOSI     48
#define PIN_MAX_CS       47

// Display is wired internally on this board (SPI SCK=12, MOSI=13, CS=10, DC=11, RST=1, BL=14)

// ==== Control params ====
#define TPM_WINDOW_MS    1000
#define TEMP_MAX_CUTOFF  120.0f
#define UI_LOOP_MS       10
#define CTRL_LOOP_MS     100
#define SENS_LOOP_MS     200

// Preset: Silicagel
#define PRESET_SILICA_T      110.0f
#define PRESET_SILICA_MIN    90   // minutes

// Feature flags (toggle as needed)
#define FEATURE_COMM   0
#define FEATURE_LOGGER 0