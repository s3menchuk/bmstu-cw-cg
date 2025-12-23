#pragma once

#include "Light.hpp"
#include "Object.hpp"

class SphereLight : public AttenuationLight {
  public:
    SphereLight(const Sphere &sphere, const Color &color, float intensity) : sphere(sphere), AttenuationLight(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const {
        return (point - sphere.center).normalized();
    }

    virtual T get_distance(const Point3 &point) const {
        return (point - sphere.center).length() - sphere.get_radius();
    }

  private:
    Sphere sphere;
};

// TODO: QuadLight