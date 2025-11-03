#include "RotarySwitch.h"

RotarySwitch::RotarySwitch(uint8_t pinA,
                           uint8_t pinB,
                           uint8_t pinSW,
                           bool buttonActiveLow,
                           RotaryEncoder::LatchMode latch)
    : _pinA(pinA),
      _pinB(pinB),
      _pinSW(pinSW),
      _activeLow(buttonActiveLow),
      _encoder(pinA, pinB, latch)
{
}

void RotarySwitch::begin(bool useInternalPullups)
{
    if (useInternalPullups)
    {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        pinMode(_pinSW, INPUT_PULLUP);
    }
    else
    {
        pinMode(_pinA, INPUT);
        pinMode(_pinB, INPUT);
        pinMode(_pinSW, INPUT);
    }

    _encoder.setPosition(0);

    _btn.raw = readRawPressed();
    _btn.stable = _btn.raw;
    _btn.rawChange = millis();
    _btn.state = _btn.stable ? PRESSED : RELEASED;
    _btn.armed = false;
}

void RotarySwitch::update()
{
    _encoder.tick();
    updateButtonFSM();
}

// --- Encoder ---
long RotarySwitch::getPosition()
{
    return _encoder.getPosition();
}

void RotarySwitch::setPosition(long pos)
{
    _encoder.setPosition(pos);
}

bool RotarySwitch::pollPositionChange(long &outNewPosition)
{
    long p = _encoder.getPosition();
    if (p != _lastReportedPos)
    {
        _lastReportedPos = p;
        outNewPosition = p;
        return true;
    }
    return false;
}

// --- Configuration ---
void RotarySwitch::setLongClickMs(uint32_t ms) { _longMs = ms; }
void RotarySwitch::setLongClickSeconds(float s) { _longMs = (uint32_t)(s * 1000.0f); }
void RotarySwitch::setDoubleClickWindowMs(uint32_t ms) { _dclickWindowMs = ms; }
void RotarySwitch::setDebounceMs(uint32_t ms) { _debounceMs = ms; }
void RotarySwitch::setButtonActiveLow(bool a) { _activeLow = a; }

// --- Button state/events ---
bool RotarySwitch::isPressed() const { return _btn.stable; }
bool RotarySwitch::wasPressed()
{
    bool v = _btn.evPress;
    _btn.evPress = false;
    return v;
}
bool RotarySwitch::wasReleased()
{
    bool v = _btn.evRelease;
    _btn.evRelease = false;
    return v;
}
bool RotarySwitch::wasDoubleClicked()
{
    bool v = _btn.evDouble;
    _btn.evDouble = false;
    return v;
}
bool RotarySwitch::wasLongClicked()
{
    bool v = _btn.evLong;
    _btn.evLong = false;
    return v;
}

// --- Private helpers ---
inline bool RotarySwitch::readRawPressed() const
{
    int v = digitalRead(_pinSW);
    return _activeLow ? (v == LOW) : (v == HIGH);
}

void RotarySwitch::updateButtonFSM()
{
    uint32_t now = millis();

    // Debounce
    bool r = readRawPressed();
    if (r != _btn.raw)
    {
        _btn.raw = r;
        _btn.rawChange = now;
    }
    bool deb = _btn.stable;
    if (now - _btn.rawChange >= _debounceMs)
        deb = _btn.raw;

    // Reset one-shot events
    _btn.evPress = _btn.evRelease = _btn.evLong = _btn.evDouble = false;

    // Edge detection
    if (deb != _btn.stable)
    {
        _btn.stable = deb;

        if (_btn.stable)
        {
            // Press
            if (_btn.armed)
                _btn.evPress = true;
            _btn.state = PRESSED;
            _btn.pressMs = now;
        }
        else
        {
            // Release
            if (_btn.armed)
                _btn.evRelease = true;

            if (_btn.state != LONGED && _btn.armed)
            {
                uint32_t hold = now - _btn.pressMs;
                if (hold < _longMs)
                {
                    if (_btn.lastShortReleaseMs && (now - _btn.lastShortReleaseMs <= _dclickWindowMs))
                    {
                        _btn.evDouble = true;
                        _btn.lastShortReleaseMs = 0;
                    }
                    else
                    {
                        _btn.lastShortReleaseMs = now;
                    }
                }
                else
                {
                    _btn.lastShortReleaseMs = 0;
                }
            }
            else
            {
                _btn.lastShortReleaseMs = 0;
            }

            _btn.state = RELEASED;
            if (!_btn.armed)
                _btn.armed = true;
        }
    }
    else
    {
        // Stable pressed: check long-click
        if (_btn.state == PRESSED && _btn.armed)
        {
            if ((now - _btn.pressMs) >= _longMs)
            {
                _btn.evLong = true;
                _btn.state = LONGED;
                _btn.lastShortReleaseMs = 0;
            }
        }
    }
}