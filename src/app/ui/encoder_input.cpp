#include "encoder_input.h"
#include <RotarySwitch.h>
#include "../pins.h"

// Single global instance — pins come from pins.h
static RotarySwitch enc(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW);

static inline int8_t sign_i32(int32_t v) {
  return (v > 0) - (v < 0);
}

void encoder_init() {
  enc.begin(true);            // use internal pull-ups
  // Timings (tune if needed)
  // enc.setDebounceMs(15);
  // enc.setDoubleClickWindowMs(300);
  // enc.setLongClickMs(1500);
  enc.setStep(1);             // default step per detent
  enc.setReverseDirection(false); // CW increases (set true if wiring inverted)
}

EncEvent encoder_poll() {
  enc.update();

  EncEvent e{};
  // Intent-first clicks
  if (enc.wasDoubleClicked()) {
    e.doubleClick = true;
  } else if (enc.wasLongPressed()) {
    e.longClick = true;
  } else if (enc.wasClicked()) {
    e.shortClick = true;
  }

  // Absolute position and delta since last poll
  e.pos   = enc.getPosition();
  e.delta = enc.delta();         // consume accumulated delta
  e.unitDelta = sign_i32(e.delta);

  // Low-level edges
  e.pressed  = enc.wasPressed();
  e.released = enc.wasReleased();
  return e;
}

// Monotone change reporting using the Library's own coalescing
bool encoder_poll_position_change(long* new_pos) {
  if (!new_pos) return false;
  enc.update();
  long p = 0;
  if (enc.pollPositionChange(p)) {
    *new_pos = p;
    return true;
  }
  return false;
}

void encoder_set_step(int8_t step) {
  enc.setStep(step);
}

void encoder_set_reverse(bool reverseCW_is_negative) {
  enc.setReverseDirection(reverseCW_is_negative);
}

int32_t encoder_get_position() {
  return (int32_t)enc.getPosition();
}
