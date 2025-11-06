#include "RotarySwitch.h"

// =============================== Debug Macros ===============================
#ifdef ROTARYSWITCHDBG
#define RS_DBG_PRINT(x) Serial.print(x)
#define RS_DBG_PRINTLN(x) Serial.println(x)
#define RS_DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define RS_DBG_PRINT(x)
#define RS_DBG_PRINTLN(x)
#define RS_DBG_PRINTF(...)
#endif

// Optional: categorized debug helpers (clean, filterable output)
#ifdef ROTARYSWITCHDBG
#define RS_DBG_EVENT(fmt, ...) RS_DBG_PRINTF("[EVENT] " fmt, ##__VA_ARGS__)
#define RS_DBG_EDGE(fmt, ...)  RS_DBG_PRINTF("[EDGE ] " fmt, ##__VA_ARGS__)
#define RS_DBG_FSM(fmt, ...)   RS_DBG_PRINTF("[FSM  ] " fmt, ##__VA_ARGS__)
#define RS_DBG_ENC(fmt, ...)   RS_DBG_PRINTF("[ENC  ] " fmt, ##__VA_ARGS__)
#else
#define RS_DBG_EVENT(fmt, ...)
#define RS_DBG_EDGE(fmt, ...)
#define RS_DBG_FSM(fmt, ...)
#define RS_DBG_ENC(fmt, ...)
#endif

// =============================== Constructors ===============================
RotarySwitch::RotarySwitch(uint8_t pinA, uint8_t pinB, uint8_t pinSW, bool buttonActiveLow,
                           RotaryEncoder::LatchMode latch, bool reverseDirection)
    : _pinA(pinA), _pinB(pinB), _pinSW(pinSW), _activeLow(buttonActiveLow), _encoder(pinA, pinB, latch),
      _dirSign(reverseDirection ? -1 : +1) {}

RotarySwitch::RotarySwitch(uint8_t pinA, uint8_t pinB, uint8_t pinSW, bool buttonActiveLow,
                           RotaryEncoder::LatchMode latch)
    : RotarySwitch(pinA, pinB, pinSW, buttonActiveLow, latch, /*reverseDirection*/ false) {}

// =============================== Init / Update ==============================
void RotarySwitch::begin(bool useInternalPullups) {
  if (useInternalPullups) {
    pinMode(_pinA, INPUT_PULLUP);
    pinMode(_pinB, INPUT_PULLUP);
    pinMode(_pinSW, INPUT_PULLUP);
  } else {
    pinMode(_pinA, INPUT);
    pinMode(_pinB, INPUT);
    pinMode(_pinSW, INPUT);
  }

  _encoder.setPosition(0);
  _lastReportedPosDisplay = 0;

  // Initialize delta tracking
  _rawPrev = _encoder.getPosition();
  _deltaDisplay = 0;

  _btn.raw = readRawPressed();
  _btn.stable = _btn.raw;
  _btn.rawChange = millis();
  _btn.state = _btn.stable ? PRESSED : RELEASED;
  _btn.armed = false;

  _btn.dcStartMs = 0;
  _btn.shortCount = 0;

  RS_DBG_FSM("begin(): raw=%d stable=%d state=%d activeLow=%d dirSign=%d\n", (int)_btn.raw, (int)_btn.stable,
             (int)_btn.state, (int)_activeLow, (int)_dirSign);
}

void RotarySwitch::update() {
  // Tick encoder and accumulate delta in display units (dirSign * step)
  _encoder.tick();

  const long rawNow = _encoder.getPosition();
  const long dispPrev = _dirSign * _rawPrev * _step;
  const long dispNow  = _dirSign * rawNow  * _step;
  const long dDisp    = dispNow - dispPrev;
  if (dDisp != 0) {
    _deltaDisplay += (int32_t)dDisp;
    _rawPrev = rawNow;
  }

  updateButtonFSM();
}

int32_t RotarySwitch::consumeDelta() {
  const int32_t d = _deltaDisplay;
  _deltaDisplay = 0;
  return d;
}

// =============================== Encoder API ================================
long RotarySwitch::getPosition() {
  const long disp = _dirSign * _encoder.getPosition() * _step;
  return disp;
}

void RotarySwitch::setPosition(long pos) {
  // pos is in display units (already includes step and direction)
  // Convert back to raw ticks: raw = pos / (dirSign * step)
  long denom = (long)_dirSign * (long)_step;
  if (denom == 0) denom = 1;
  const long raw = pos / denom;

  _encoder.setPosition(raw);
  _lastReportedPosDisplay = pos;
  _rawPrev = raw; // keep delta tracking consistent

  RS_DBG_ENC("setPosition(): disp=%ld raw=%ld\n", pos, raw);
}

bool RotarySwitch::pollPositionChange(long &outNewPosition) {
  const long raw = _encoder.getPosition();
  const long disp = _dirSign * raw * _step;
  if (disp != _lastReportedPosDisplay) {
    _lastReportedPosDisplay = disp;
    outNewPosition = disp;
    RS_DBG_ENC("position change: disp=%ld (raw=%ld, step=%d)\n", disp, raw, (int)_step);
    return true;
  }
  return false;
}

void RotarySwitch::setReverseDirection(bool reverse) {
  const int8_t newSign = reverse ? -1 : +1;
  if (newSign == _dirSign)
    return;
  const long currentDisplay = _dirSign * _encoder.getPosition() * _step;
  _dirSign = newSign;

  // Keep the current display value constant when flipping direction
  long denom = (long)_dirSign * (long)_step;
  if (denom == 0) denom = 1;
  const long rawNew = currentDisplay / denom;
  _encoder.setPosition(rawNew);
  _rawPrev = rawNew;
  _lastReportedPosDisplay = currentDisplay;
  RS_DBG_ENC("setReverseDirection(): dirSign=%d keep disp=%ld\n", (int)_dirSign, currentDisplay);
}

void RotarySwitch::setStep(int8_t step) {
  if (step == 0)
    step = 1;

  // Preserve current *display* value across step changes.
  const int8_t oldStep = _step;
  const long rawNow = _encoder.getPosition();
  const long dispNow = _dirSign * rawNow * oldStep; // display value before changing step

  _step = step;

  // Compute new raw so that: dispNow = dirSign * rawNew * _step
  long denom = (long)_dirSign * (long)_step; // ±step
  if (denom == 0) denom = 1;
  long rawNew = dispNow / denom; // truncation toward 0 is acceptable

  _encoder.setPosition(rawNew);

  // Keep delta tracking and last-reported display in sync
  _rawPrev = rawNew;
  _lastReportedPosDisplay = dispNow;

  RS_DBG_ENC("setStep(): old=%d new=%d  rawNow=%ld -> rawNew=%ld  keep disp=%ld\n", (int)oldStep, (int)_step, rawNow,
             rawNew, dispNow);
}

// ========================= Button config & events ===========================
void RotarySwitch::setLongClickMs(uint32_t ms) {
  _longMs = ms;
}
void RotarySwitch::setLongClickSeconds(float s) {
  _longMs = (uint32_t)(s * 1000.0f);
}
void RotarySwitch::setDoubleClickWindowMs(uint32_t ms) {
  _dclickWindowMs = ms;
}
void RotarySwitch::setDebounceMs(uint32_t ms) {
  _debounceMs = ms;
}
void RotarySwitch::setButtonActiveLow(bool a) {
  _activeLow = a;
}

bool RotarySwitch::isPressed() const {
  return _btn.stable;
}

bool RotarySwitch::wasPressed() {
  const bool v = _btn.evPress;
  _btn.evPress = false;
  return v;
}
bool RotarySwitch::wasReleased() {
  const bool v = _btn.evRelease;
  _btn.evRelease = false;
  return v;
}
bool RotarySwitch::wasShortClicked() {
  const bool v = _btn.evShort;
  _btn.evShort = false;
  return v;
}
bool RotarySwitch::wasDoubleClicked() {
  const bool v = _btn.evDouble;
  _btn.evDouble = false;
  return v;
}
bool RotarySwitch::wasLongClicked() {
  const bool v = _btn.evLong;
  _btn.evLong = false;
  return v;
}

// =============================== Private ====================================
inline bool RotarySwitch::readRawPressed() const {
  const int v = digitalRead(_pinSW);
  return _activeLow ? (v == LOW) : (v == HIGH);
}

/**
 * Robust click policy:
 * - LongClick   : at RELEASE, when hold >= _longMs (consumes cycle)
 * - DoubleClick : at second short RELEASE within _dclickWindowMs (consumes cycle)
 * - ShortClick  : when the window expires and exactly one short click is pending
 * Low-level Press/Release remain available, but are not emitted in addition to Double/Long.
 */
void RotarySwitch::updateButtonFSM() {
  const uint32_t now = millis();

  // --- Debounce raw signal ---
  const bool r = readRawPressed();
  if (r != _btn.raw) {
    _btn.raw = r;
    _btn.rawChange = now;
  }

  bool deb = _btn.stable;
  if (now - _btn.rawChange >= _debounceMs) {
    deb = _btn.raw;
  }

  // Reset one-shot events
  _btn.evPress = _btn.evRelease = _btn.evShort = _btn.evDouble = _btn.evLong = false;

  // --- Timeout of open Double-Click window ---
  if (_btn.dcStartMs && (now - _btn.dcStartMs > _dclickWindowMs)) {
    if (_btn.shortCount == 1) {
      _btn.evShort = true; // confirmed single short click
      RS_DBG_EVENT("ShortClick (timeout)\n");
    }
    _btn.dcStartMs = 0;
    _btn.shortCount = 0;
  }

  // --- Edge handling ---
  if (deb != _btn.stable) {
    _btn.stable = deb;

    if (_btn.stable) {
      // ===================== PRESS =====================
      if (_btn.armed)
        _btn.evPress = true;
      _btn.state = PRESSED;
      _btn.pressMs = now;
      RS_DBG_EDGE("PRESS: raw=%d stable=%d\n", (int)_btn.raw, (int)_btn.stable);

    } else {
      // ===================== RELEASE ===================
      const uint32_t hold = now - _btn.pressMs;

      RS_DBG_EDGE("RELEASE: hold=%ums (dcOpen=%u shortCount=%u)\n", (unsigned)hold, (unsigned)(_btn.dcStartMs != 0),
                  (unsigned)_btn.shortCount);

      // LongClick?
      if (_btn.armed && (hold >= _longMs)) {
        _btn.evLong = true;
        // Close DC window & reset counters
        _btn.dcStartMs = 0;
        _btn.shortCount = 0;
        _btn.state = RELEASED;
        RS_DBG_EVENT("LongClick\n");
        return; // consumed; don't also emit Release
      }

      // Not long: Double/Short logic (short release)
      if (_btn.armed) {
        if (_btn.dcStartMs == 0) {
          // Open window
          _btn.dcStartMs = now;
          _btn.shortCount = 1;
          RS_DBG_FSM("start DC window (shortCount=1)\n");
        } else {
          // Window open -> check DoubleClick
          if (now - _btn.dcStartMs <= _dclickWindowMs) {
            _btn.shortCount++;
            if (_btn.shortCount >= 2) {
              _btn.evDouble = true;
              _btn.dcStartMs = 0;
              _btn.shortCount = 0;
              _btn.state = RELEASED;
              RS_DBG_EVENT("DoubleClick\n");
              return; // consumed; don't also emit Release
            } else {
              RS_DBG_FSM("second short within window but shortCount=%u\n", (unsigned)_btn.shortCount);
            }
          } else {
            // Window just expired – ShortClick would have been reported above (timeout).
            // For this release, start a new window.
            _btn.dcStartMs = now;
            _btn.shortCount = 1;
            RS_DBG_FSM("restart DC window (expired previously)\n");
          }
        }
      }

      // Low-level Release optionally available (independent of intent)
      if (_btn.armed)
        _btn.evRelease = true;

      _btn.state = RELEASED;
      if (!_btn.armed)
        _btn.armed = true;
    }

  } else {
    // --- stable state: nothing to do (Long decided at Release) ---
  }
}
