#pragma once

#include "Color.hpp"

// TODO: MattePlastic, GlossyPlastic, Metal, Glass

class Material {
  public:
    Material(const Color &albedo, float metallic) : albedo(albedo), metallic(metallic) {}

    Color albedo;
    float metallic;
};

// class BaseMaterial {
// public:
//     virtual Color shade(const Ray& ray, const HitRecord& hit_record, const Light& light) const = 0;
//     virtual ~BaseMaterial() = default;
// };
//
// class DiffuseMaterial : public BaseMaterial {
//     // albedo
// };
//
// class SpecularMaterial : public BaseMaterial {
//     // reflectivity
// };
//
// class EmissiveMaterial : public BaseMaterial {
//     // intensity
// };
