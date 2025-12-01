#pragma once

#include <numbers>

inline float map(float val, float cl, float cr, float tl, float tr) { return tl + (val - cl) / (cr - cl) * (tr - tl); }

template <typename T> T radians2degrees(T radians) { return radians * 180 / std::numbers::pi; }

template <typename T> T degrees2radians(T degrees) { return degrees * std::numbers::pi / 180; }