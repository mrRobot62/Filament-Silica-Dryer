#pragma once
#include <Arduino.h>

/* ============================================================
 *  ESP32-S3 HMI FilamentDryer  — Pin mapping
 *  Zentralisierte Definition aller Hardware-Pins
 * ============================================================ */

// --- LCD / ST7789 SPI panel ---
#define LCD_SCK 12
#define LCD_MOSI 13
#define LCD_CS 10
#define LCD_DC 11
#define LCD_RST 1
#define LCD_BL 14

// Display parameters
#define LCD_WIDTH 320
#define LCD_HEIGHT 170
#define LCD_COL_OFFSET 35
#define LCD_ROW_OFFSET 0
#define LCD_ROTATION 1
#define LCD_BACKLIGHT_ON HIGH

// --- Rotary encoder ---
#define ENC_PIN_A 2
#define ENC_PIN_B 42
#define ENC_PIN_SW 5 // BOOT button (active LOW)

// Rotary behaviour
#define ROTARY_LONG_MS 2500
#define ROTARY_DEBOUNCE_MS 20
#define ROTARY_LATCH_MODE RotaryEncoder::LatchMode::FOUR3

// --- Temperature sensor (Adafruit MAX6675) ---
#define MAX6675_CS 40 // 7
// #define MAX31856_DI 40 // 8
#define MAX6675_DO 39  // 9
#define MAX6675_CLK 41 // 15

// --- Fan / Heater / Lamp control (placeholders) ---
#define PIN_FAN_12V 16
#define PIN_FAN_230V 17
#define PIN_HEATER 18
#define PIN_LAMP 19
#define PIN_DOOR_SW 20
#define PIN_BEEPER 21
