#pragma once
#include <Arduino.h>
#include <RotaryEncoder.h>

/**
 * RotarySwitch: RotaryEncoder + debounced push button with intent events.
 *
 * Events policy (robust, classic):
 *  - LongClick   : on RELEASE if hold >= longMs
 *  - DoubleClick : on second short RELEASE within doubleClickWindow
 *  - ShortClick  : when the window expires with exactly one short click
 *
 * Low-level edges (Press/Release) are available for debugging but not required for logic.
 */
class RotarySwitch {
public:
  RotarySwitch(uint8_t pinA, uint8_t pinB, uint8_t pinSW, bool buttonActiveLow = true,
               RotaryEncoder::LatchMode latch = RotaryEncoder::LatchMode::FOUR3, bool reverseDirection = false);

  RotarySwitch(uint8_t pinA, uint8_t pinB, uint8_t pinSW, bool buttonActiveLow, RotaryEncoder::LatchMode latch);

  // Initialization
  void begin(bool useInternalPullups = true);
  // Compatibility overload to match older call sites: begin(a,b,s,usePullups)
  inline void begin(uint8_t /*a*/, uint8_t /*b*/, uint8_t /*s*/, bool useInternalPullups) {
    begin(useInternalPullups);
  }

  // Periodic update (call as often as possible)
  void update();

  // Encoder - absolute position in display units (dirSign * raw * step)
  long getPosition();
  void setPosition(long pos);
  bool pollPositionChange(long &outNewPosition);

  // Direction
  void setReverseDirection(bool reverse);
  bool getReverseDirection() const { return _dirSign < 0; }

  // Button config
  void setLongClickMs(uint32_t ms);
  void setLongClickSeconds(float seconds);
  void setDoubleClickWindowMs(uint32_t ms);
  void setDebounceMs(uint32_t ms);
  void setButtonActiveLow(bool activeLow);

  // Low-level state/events
  bool isPressed() const;
  bool wasPressed();
  bool wasReleased();

  // Intent events
  bool wasShortClicked();
  bool wasDoubleClicked();
  bool wasLongClicked();

  // -------------------------------------------------------------------------
  // Delta API (since last consume; measured in display units = dirSign*step)
  // -------------------------------------------------------------------------
  /**
   * @brief Returns the accumulated delta since the last call to consumeDelta()
   *        without resetting it.
   */
  int32_t getDelta() const { return _deltaDisplay; }

  /**
   * @brief Returns and clears the accumulated delta since last consume.
   */
  int32_t consumeDelta();

  // Backward-compatibility aliases expected by older app code
  inline int32_t delta() { return consumeDelta(); }
  inline bool wasClicked() { return wasShortClicked(); }
  inline bool wasLongPressed() { return wasLongClicked(); }

  // --- Step configuration ---
  /**
   * @brief Sets the step size applied to each encoder detent.
   * @param step The increment/decrement per encoder tick (default = 1).
   */
  void setStep(int8_t step);
  int8_t getStep() const { return _step; }

private:
  enum BtnState : uint8_t { RELEASED, PRESSED };

  struct ButtonFSM {
    // debouncing
    bool raw = false;
    bool stable = false;
    uint32_t rawChange = 0;

    // state
    BtnState state = RELEASED;
    bool armed = false;

    // timing
    uint32_t pressMs = 0;   // last press time
    uint32_t dcStartMs = 0; // start of double-click window (0 = none)
    uint8_t shortCount = 0; // number of short clicks collected in window

    // one-shot events
    bool evPress = false;
    bool evRelease = false;
    bool evShort = false;
    bool evDouble = false;
    bool evLong = false;
  } _btn;

  // pins & polarity
  const uint8_t _pinA, _pinB, _pinSW;
  bool _activeLow;

  // encoder & direction
  RotaryEncoder _encoder;
  int8_t _dirSign = +1;

  // timing config
  uint32_t _debounceMs = 15;
  uint32_t _dclickWindowMs = 350;
  uint32_t _longMs = 2500;

  // position tracking
  long _lastReportedPosDisplay = 0;
  long _rawPrev = 0;               // previous raw encoder ticks
  volatile int32_t _deltaDisplay = 0; // accumulated delta in display units

  // helpers
  inline bool readRawPressed() const;
  void updateButtonFSM();

  int8_t _step = 1; // step multiplier per encoder tick
};
