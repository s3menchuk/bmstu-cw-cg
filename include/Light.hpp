#pragma once

#include "Color.hpp"
#include "Vec.hpp"

// TODO: More types of light sources

class Light {
  public:
    Light(const Color &color, float intensity) : color(color), intensity(intensity) {}

    Color get_color() const {
        return color;
    }
    virtual float get_intensity(const Point3 &point) const = 0;
    virtual Vec3 get_direction(const Point3 &point) const = 0;
    virtual float get_distance(const Point3 &point) const = 0;

    virtual ~Light() = default;

  protected:
    Color color;
    float intensity;
};

class AttenuationLight : public Light {
  public:
    AttenuationLight(const Color &color, float intensity, float kq = 1, float kl = 0, float kc = 0)
        : Light(color, intensity), kq(kq), kl(kl), kc(kc) {}

    float get_intensity(const Point3 &point) const override {
        float dist = get_distance(point);
        return this->intensity / (kq * dist * dist + kl * dist + kc);
    }

  private:
    float kq, kl, kc;
};

class NonAttenuationLight : public Light {
  public:
    NonAttenuationLight(const Color &color, float intensity) : Light(color, intensity) {}

    float get_intensity(const Point3 &point) const override {
        return this->intensity;
    }
};

class PointLight : public AttenuationLight {
  public:
    PointLight(const Point3 &pos, const Color &color, float intensity) : pos(pos), AttenuationLight(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const override {
        return (point - pos).normalized();
    }

    float get_distance(const Point3 &point) const override {
        return (point - pos).length();
    }

  private:
    Vec3 pos;
};

class DirectionLight : public NonAttenuationLight {
  public:
    DirectionLight(const Vec3 &dir, const Color &color, float intensity) : dir(dir.normalized()), NonAttenuationLight(color, intensity) {}

    Vec3 get_direction(const Point3 &point) const override {
        return dir;
    }

    float get_distance(const Point3 &point) const override {
        return std::numeric_limits<float>::infinity();
    }

  private:
    Vec3 dir;
};
