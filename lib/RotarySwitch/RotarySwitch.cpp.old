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

// Optional: kategorisierte Debug-Helfer (für saubere, filterbare Ausgaben)
#ifdef ROTARYSWITCHDBG
#define RS_DBG_EVENT(fmt, ...) RS_DBG_PRINTF("[EVENT] " fmt, ##__VA_ARGS__)
#define RS_DBG_EDGE(fmt, ...) RS_DBG_PRINTF("[EDGE ] " fmt, ##__VA_ARGS__)
#define RS_DBG_FSM(fmt, ...) RS_DBG_PRINTF("[FSM  ] " fmt, ##__VA_ARGS__)
#define RS_DBG_ENC(fmt, ...) RS_DBG_PRINTF("[ENC  ] " fmt, ##__VA_ARGS__)
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
  _encoder.tick();
  updateButtonFSM();
}

// =============================== Encoder API ================================
long RotarySwitch::getPosition() {
  const long disp = _dirSign * _encoder.getPosition();
  return disp;
}

void RotarySwitch::setPosition(long pos) {
  _encoder.setPosition(_dirSign * pos);
  _lastReportedPosDisplay = pos;
  RS_DBG_ENC("setPosition(): disp=%ld raw=%ld\n", pos, _dirSign * pos);
}

bool RotarySwitch::pollPositionChange(long &outNewPosition) {
  const long raw = _encoder.getPosition();
  const long disp = _dirSign * raw * _step; // <-- NEU: step multiplier
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
  const long currentDisplay = _dirSign * _encoder.getPosition();
  _dirSign = newSign;
  _encoder.setPosition(_dirSign * currentDisplay);
  _lastReportedPosDisplay = currentDisplay;
  RS_DBG_ENC("setReverseDirection(): dirSign=%d keep disp=%ld\n", (int)_dirSign, currentDisplay);
}

void RotarySwitch::setStep(int8_t step) {
  if (step == 0)
    step = 1;

  // Wir wollen den aktuellen *Anzeige*-Wert (disp) beibehalten.
  // Dazu berechnen wir den Ist-Displaywert mit *altem* step
  // und setzen anschließend die raw-Position so, dass disp gleich bleibt.
  const int8_t oldStep = _step;
  const long rawNow = _encoder.getPosition();
  const long dispNow = _dirSign * rawNow * oldStep; // aktueller angezeigter Wert (vor Step-Wechsel)

  _step = step;

  // Neue raw-Position so wählen, dass: dispNow = dirSign * rawNew * _step
  // => rawNew = dispNow / (dirSign * step)
  long denom = (long)_dirSign * (long)_step; // ±step
  if (denom == 0)
    denom = 1;                   // defensive (sollte nie passieren)
  long rawNew = dispNow / denom; // Rundung Richtung 0 ist ok

  _encoder.setPosition(rawNew);

  // Wichtig: lastReported auf den *gleichen* Displaywert setzen,
  // damit pollPositionChange() keinen Sprung meldet.
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
 * Robust, klassische Click-Politik:
 * - LongClick   : beim RELEASE, wenn hold >= _longMs (konsumiert Zyklus)
 * - DoubleClick : beim zweiten kurzen RELEASE innerhalb _dclickWindowMs (konsumiert Zyklus)
 * - ShortClick  : wenn _dclickWindowMs abläuft und genau ein kurzer Klick ansteht
 * Low-level Press/Release stehen weiterhin zur Verfügung, werden aber von Double/Long nicht zusätzlich emittiert.
 */
void RotarySwitch::updateButtonFSM() {
  const uint32_t now = millis();

  // --- Debounce Rohsignal ---
  const bool r = readRawPressed();
  if (r != _btn.raw) {
    _btn.raw = r;
    _btn.rawChange = now;
  }

  bool deb = _btn.stable;
  if (now - _btn.rawChange >= _debounceMs) {
    deb = _btn.raw;
  }

  // One-shot Events zurücksetzen
  _btn.evPress = _btn.evRelease = _btn.evShort = _btn.evDouble = _btn.evLong = false;

  // --- Timeout des offenen Double-Click-Fensters ---
  if (_btn.dcStartMs && (now - _btn.dcStartMs > _dclickWindowMs)) {
    if (_btn.shortCount == 1) {
      _btn.evShort = true; // bestätigter einzelner kurzer Klick
      RS_DBG_EVENT("ShortClick (timeout)\n");
    }
    _btn.dcStartMs = 0;
    _btn.shortCount = 0;
  }

  // --- Kantenbehandlung ---
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
        // Double/Short-Fenster schließen & Zähler zurücksetzen
        _btn.dcStartMs = 0;
        _btn.shortCount = 0;
        _btn.state = RELEASED;
        RS_DBG_EVENT("LongClick\n");
        return; // Zyklus konsumiert; kein zusätzliches Release nötig
      }

      // Nicht lang: Double/Short-Logik (kurzer Release)
      if (_btn.armed) {
        if (_btn.dcStartMs == 0) {
          // Fenster neu öffnen
          _btn.dcStartMs = now;
          _btn.shortCount = 1;
          RS_DBG_FSM("start DC window (shortCount=1)\n");
        } else {
          // Fenster ist offen -> Prüfe DoubleClick
          if (now - _btn.dcStartMs <= _dclickWindowMs) {
            _btn.shortCount++;
            if (_btn.shortCount >= 2) {
              _btn.evDouble = true;
              _btn.dcStartMs = 0;
              _btn.shortCount = 0;
              _btn.state = RELEASED;
              RS_DBG_EVENT("DoubleClick\n");
              return; // Zyklus konsumiert; kein zusätzliches Release nötig
            } else {
              RS_DBG_FSM("second short within window but shortCount=%u\n", (unsigned)_btn.shortCount);
            }
          } else {
            // Fenster ist gerade ausgelaufen – ShortClick wäre oben (Timeout) gemeldet worden.
            // Für diesen Release starten wir ein neues Fenster.
            _btn.dcStartMs = now;
            _btn.shortCount = 1;
            RS_DBG_FSM("restart DC window (expired previously)\n");
          }
        }
      }

      // Low-level Release optional verfügbar lassen (nicht von Intent abhängig)
      if (_btn.armed)
        _btn.evRelease = true;

      _btn.state = RELEASED;
      if (!_btn.armed)
        _btn.armed = true;
    }

  } else {
    // --- stabiler Zustand: nichts zu tun (Long wird bei Release entschieden) ---
  }
}
