#pragma once

#include "Color.hpp"
#include "Types.hpp"

class Material {
  public:
    Material(Color base_color, Real metallic, Color emission_color = Color(1, 1, 1), Real emission_strength = 0)
        : base_color(base_color), metallic(metallic), emission_color(emission_color), emission_strength(emission_strength) {}

    Color base_color;
    Real metallic;
    Color emission_color;
    Real emission_strength;
};