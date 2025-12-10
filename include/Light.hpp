#pragma once

#include "Color.hpp"
#include "Vec3.hpp"

using T = float;

class Light {
  public:
    Light(const Color &color, T intensity) : color(color), intensity(intensity) {}

    Color color;
    virtual Vec3 get_direction(const Point3 &point) const = 0;
    virtual T get_distance(const Point3 &point) const = 0;
    virtual T get_intensity(const Point3 &point) const = 0;
    void set_intensity(T value) {
        // throw exception with value < 0
        if (value >= 0)
            intensity = value;
    }

    virtual ~Light() = default;

  protected:
    T intensity;
};

class PointLight : public Light {
  public:
    Vec3 pos;
    PointLight(const Point3 &pos, const Color &color, T intensity) : pos(pos), Light(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const override {
        return (point - pos).normalized();
    }

    T get_distance(const Point3 &point) const override {
        return (point - pos).length();
    }

    T get_intensity(const Point3 &point) const override {
        T dist = get_distance(point);
        return this->intensity / (dist * dist);
    }
};

class DirectionLight : public Light {
  public:
    Vec3 dir;
    DirectionLight(const Vec3 &dir, const Color &color, T intensity) : dir(dir.normalized()), Light(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const override {
        return dir;
    }

    T get_distance(const Point3 &point) const override {
        return std::numeric_limits<T>::infinity();
    }

    T get_intensity(const Point3 &point) const override {
        return this->intensity;
    }
};

// More types of light sources
