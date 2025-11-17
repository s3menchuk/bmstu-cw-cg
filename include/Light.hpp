#pragma once

#include "Color.hpp"
#include "Vec3.hpp"

using T = float;

class Light {
  public:
	Light(const Color &color, T intensity) : color(color), intensity(intensity) {}

	Color color;
	virtual Vec3 get_direction(const Point3 &point) const = 0;
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

	T get_intensity(const Point3 &point) const override {
		T dist = (point - pos).length();
		return this->intensity / (dist * dist);
	}

	Vec3 get_direction(const Point3 &point) const override {
		Vec3 res = point - pos;
		res.normalize();
		return res;
	};
};

class DirectionLight : public Light {
  public:
	Vec3 dir;
	DirectionLight(const Vec3 &dir, const Color &color, T intensity) : dir(dir), Light(color, intensity) {
		this->dir.normalize();
	}
	T get_intensity(const Point3 &point) const override { return this->intensity; }
	Vec3 get_direction(const Point3 &point) const override { return dir; }
};

// More types of light sources