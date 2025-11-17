#pragma once

#include <numbers>

float map(float val, float cl, float cr, float tl, float tr);

template <typename T>
T radians2degrees(T radians) { return radians * 180 / std::numbers::pi; }

template <typename T>
T degrees2radians(T degrees) { return degrees * std::numbers::pi / 180; }