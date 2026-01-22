#pragma once

#include "Color.hpp"
#include "Types.hpp"

class Material {
  public:
    Material(const Color &color, Real reflectance = 0, Color emission_color = Color(1, 1, 1), Real emission_strength = 0)
        : color(color), reflectance(reflectance), emission_color(emission_color), emission_strength(emission_strength) {}

    Color color;
    Real reflectance;
    Color emission_color;
    Real emission_strength;
};