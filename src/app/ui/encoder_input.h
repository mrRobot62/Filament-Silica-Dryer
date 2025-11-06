#pragma once
#include <stdint.h>
#include <stdbool.h>

// Unified encoder event returned each poll
struct EncEvent {
  int32_t pos = 0;       // absolute position in display units (dirSign * raw * step)
  int32_t delta = 0;     // accumulated steps since last poll (same units as pos)
  int8_t  unitDelta = 0; // normalized step: -1, 0, +1
  bool shortClick = false;
  bool longClick  = false;
  bool doubleClick = false;
  bool pressed = false;   // low-level edge
  bool released = false;  // low-level edge
};

void encoder_init();
void encoder_update();                         // call ONCE per loop
EncEvent encoder_poll();                       // read events/delta AFTER encoder_update()
bool encoder_poll_position_change(long* new_pos); // read pos-change AFTER encoder_update()

void encoder_set_step(int8_t step);
void encoder_set_reverse(bool reverseCW_is_negative);
int32_t encoder_get_position();
