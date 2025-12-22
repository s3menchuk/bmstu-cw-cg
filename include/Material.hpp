#pragma once

#include "Color.hpp"
#include "Types.hpp"

class Material {
  public:
    Material(const Color &color, T reflectance) : color(color), reflectance(reflectance) {}

    Color color;
    T reflectance;
};