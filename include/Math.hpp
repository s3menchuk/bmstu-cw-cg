#pragma once

#include "Types.hpp"

#include <numbers>

template <typename T>
T clamp(T x, T min, T max) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

template <typename T>
T lerp(T a, T b, T t) {
    return a + (b - a) * t;
}

template <typename T>
T map(T val, T cl, T cr, T tl, T tr) {
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
