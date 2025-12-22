#pragma once

#include "Color.hpp"
#include "Types.hpp"
#include "Vec3.hpp"

// TODO: More types of light sources

class Light {
  public:
    Light(const Color &color, T intensity) : color(color), intensity(intensity) {}

    Color get_color() const {
        return color;
    }
    virtual T get_intensity(const Point3 &point) const = 0;
    virtual Vec3 get_direction(const Point3 &point) const = 0;
    virtual T get_distance(const Point3 &point) const = 0;

    virtual ~Light() = default;

  protected:
    Color color;
    T intensity;
};

class AttenuationLight : public Light {
  public:
    AttenuationLight(const Color &color, T intensity, T kq = 1, T kl = 0, T kc = 0) : Light(color, intensity), kq(kq), kl(kl), kc(kc) {}

    T get_intensity(const Point3 &point) const override {
        T dist = get_distance(point);
        return this->intensity / (kq * dist * dist + kl * dist + kc);
    }

  private:
    T kq, kl, kc;
};

class NonAttenuationLight : public Light {
  public:
    NonAttenuationLight(const Color &color, T intensity) : Light(color, intensity) {}

    T get_intensity(const Point3 &point) const override {
        return this->intensity;
    }
};

class PointLight : public AttenuationLight {
  public:
    PointLight(const Point3 &pos, const Color &color, T intensity) : pos(pos), AttenuationLight(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const override {
        return (point - pos).normalized();
    }

    T get_distance(const Point3 &point) const override {
        return (point - pos).length();
    }

  private:
    Vec3 pos;
};

class DirectionLight : public NonAttenuationLight {
  public:
    DirectionLight(const Vec3 &dir, const Color &color, T intensity) : dir(dir.normalized()), NonAttenuationLight(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const override {
        return dir;
    }

    T get_distance(const Point3 &point) const override {
        return std::numeric_limits<T>::infinity();
    }

  private:
    Vec3 dir;
};
