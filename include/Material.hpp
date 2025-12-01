#pragma once

#include "Color.hpp"

class Material {
public:
  Material(const Color &ambient, const Color &diffuse, const Color &specular, float reflective, float shininess)
      : ambient(ambient), diffuse(diffuse), specular(specular), reflective(reflective), shininess(shininess) {}
  Color ambient;
  Color diffuse;
  Color specular;
  float reflective;
  float shininess;
};

class MattePlastic : public Material {
public:
  MattePlastic(const Color &diffuse) : Material(diffuse / 6, diffuse, Color::WHITE * 0.3, 0.1, 20) {}
};

class GlossyPlastic : public Material {
public:
  GlossyPlastic(const Color &diffuse) : Material(diffuse / 4, diffuse, Color::WHITE * 0.8, 0.2, 75) {}
};

class Metal : public Material {
public:
  Metal(const Color &diffuse) : Material(diffuse / 4, diffuse, diffuse * 5 / 4, 0.7, 350) {}
};

class Glass : public Material {
public:
  Glass(const Color &diffuse) : Material(Color::BLACK, diffuse, diffuse * 10, 0.4, 750) {}
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