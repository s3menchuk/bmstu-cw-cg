#pragma once

#include "Types.hpp"

#include <numbers>

inline Real lerp(Real a, Real b, Real t) {
    return a + (b - a) * t;
}

inline Real map(Real val, Real cl, Real cr, Real tl, Real tr) {
    return tl + (val - cl) / (cr - cl) * (tr - tl);
}

template <typename T>
T radians2degrees(T radians) {
    return radians * 180 / std::numbers::pi;
}

template <typename T>
T degrees2radians(T degrees) {
    return degrees * std::numbers::pi / 180;
}
