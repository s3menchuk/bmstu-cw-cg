#pragma once

#include "Color.hpp"
#include "Types.hpp"

struct Material {
    Color color;
    Real reflectance = 0;

    Real shininess = 32;

    Color emission_color = Color(1, 1, 1);
    Real emission_strength = 0;
};