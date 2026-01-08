#pragma once

#include "Color.hpp"
#include "Types.hpp"

class Material {
  public:
    Material(const Color &color, Real reflectance) : color(color), reflectance(reflectance) {}

    Color color;
    Real reflectance;
};