#include <Arduino.h>
#include "ui/gfx_ui.h"
#include "ui/heat_screen.h"
#include "rotary_handler.h"

// Rotary hooks (this file)
void rotary_setup();
void rotary_update();

void setup()
{
  Serial.begin(115200);
  gfx_begin();    // panel init (your working config)
  heat_init();    // draw HEAT screen
  rotary_setup(); // sets default focus + log
}

void loop()
{
  static uint32_t t = 0;
  rotary_update(); // poll rotary & button

  // if (millis() - t > 1000)
  // {
  //   t = millis();
  //   static uint32_t secs = 0;
  //   secs++;
  //   heat_set_timer_seconds(secs); // just to see the label update
  // }

  // // emulate focus toggle every 2s
  // static uint32_t tf = 0;
  // if (millis() - tf > 2000)
  // {
  //   tf = millis();
  //   static int i = 0;
  //   heat_set_selected(static_cast<HeatButton>(i % 3));
  //   i++;
  // }
}