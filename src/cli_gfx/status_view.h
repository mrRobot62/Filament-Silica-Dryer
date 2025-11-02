#pragma once
#include <Arduino.h>
#include <type_traits>

// Detect presence of a member by name (pointer-to-member SFINAE)
#define DEFINE_HAS_MEMBER(member) \
template <typename T, typename = int> struct has_##member : std::false_type {}; \
template <typename T> struct has_##member<T, decltype((void) &T::member, 0)> : std::true_type {};

// Define detectors for possible field names in SystemStatus
DEFINE_HAS_MEMBER(temperatureC)
DEFINE_HAS_MEMBER(tempC)
DEFINE_HAS_MEMBER(temp_c)

DEFINE_HAS_MEMBER(fan12vRunning)
DEFINE_HAS_MEMBER(fan12v_running)
DEFINE_HAS_MEMBER(fan12v)

DEFINE_HAS_MEMBER(heater)
DEFINE_HAS_MEMBER(heater_on)

DEFINE_HAS_MEMBER(fan230)
DEFINE_HAS_MEMBER(fan230_on)

DEFINE_HAS_MEMBER(motor)
DEFINE_HAS_MEMBER(motor_on)

DEFINE_HAS_MEMBER(lastFault)
DEFINE_HAS_MEMBER(fault)
DEFINE_HAS_MEMBER(fault_code)

// ---- Accessors ----
template <typename S> inline float sv_getTempC(const S& s) {
    if constexpr (has_temperatureC<S>::value) return s.temperatureC;
    else if constexpr (has_tempC<S>::value)   return s.tempC;
    else if constexpr (has_temp_c<S>::value)  return s.temp_c;
    else return NAN;
}

template <typename S> inline bool sv_getFan12V(const S& s) {
    if constexpr (has_fan12vRunning<S>::value) return s.fan12vRunning;
    else if constexpr (has_fan12v_running<S>::value) return s.fan12v_running;
    else if constexpr (has_fan12v<S>::value) return s.fan12v;
    else return false;
}

template <typename S> inline bool sv_getHeater(const S& s) {
    if constexpr (has_heater<S>::value) return static_cast<bool>(s.heater);
    else if constexpr (has_heater_on<S>::value) return s.heater_on;
    else return false;
}

template <typename S> inline bool sv_getFan230(const S& s) {
    if constexpr (has_fan230<S>::value) return static_cast<bool>(s.fan230);
    else if constexpr (has_fan230_on<S>::value) return s.fan230_on;
    else return false;
}

template <typename S> inline bool sv_getMotor(const S& s) {
    if constexpr (has_motor<S>::value) return static_cast<bool>(s.motor);
    else if constexpr (has_motor_on<S>::value) return s.motor_on;
    else return false;
}

template <typename S> inline int sv_getFault(const S& s) {
    if constexpr (has_lastFault<S>::value) return static_cast<int>(s.lastFault);
    else if constexpr (has_fault_code<S>::value) return static_cast<int>(s.fault_code);
    else if constexpr (has_fault<S>::value) return static_cast<int>(s.fault);
    else return 0;
}
