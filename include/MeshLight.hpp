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

class QuadLight : public AttenuationLight {
  public:
    QuadLight(const Quad &quad, const Color &color, float intensity) : quad(quad), AttenuationLight(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const {
        auto center = quad.Q + (quad.u + quad.v) / 2.0f;
        return (point - center).normalized();
    }

    virtual T get_distance(const Point3 &point) const {
        auto center = quad.Q + (quad.u + quad.v) / 2.0f;
        return (point - center).length();
    }

  private:
    Quad quad;
};