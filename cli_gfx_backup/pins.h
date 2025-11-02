#pragma once

// ============================== Pin Mapping =================================
// Powerboard I/Os
#define PIN_SSR 7
#define PIN_FAN5V 6
#define PIN_FAN230 15   // P7
#define PIN_LAMP230 16  // P8
#define PIN_MOTOR230 17 // P9
#define PIN_FANL230 18  // P10
#define PIN_DOOR 19     // input, active LOW (LOW=closed, HIGH=open)

// Encoder + Switch (per Wunsch: GPIO20/21; USB CDC = disabled in platformio.ini)
#define PIN_ENC_A 20
#define PIN_ENC_B 21
#define PIN_ENC_SW 5 // active LOW

// MAX31856 (Software SPI)
#define PIN_TC_SCK 40
#define PIN_TC_MISO 42
#define PIN_TC_MOSI 48
#define PIN_TC_CS 47

// ST7789 170x320 (ESP32-S3 HMI LCD)
#define PIN_LCD_SCK 12
#define PIN_LCD_MOSI 13
#define PIN_LCD_CS 10
#define PIN_LCD_DC 11
#define PIN_LCD_RST 1
#define PIN_LCD_BL 14