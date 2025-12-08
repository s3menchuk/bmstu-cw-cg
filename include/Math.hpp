#pragma once

#include "Types.hpp"

#include <numbers>

inline T lerp(T a, T b, T t) {
    return a + (b - a) * t;
}

inline T map(T val, T cl, T cr, T tl, T tr) {
    return tl + (val - cl) / (cr - cl) * (tr - tl);
}

template <typename T> T radians2degrees(T radians) {
    return radians * 180 / std::numbers::pi;
}

template <typename T> T degrees2radians(T degrees) {
    return degrees * std::numbers::pi / 180;
}
