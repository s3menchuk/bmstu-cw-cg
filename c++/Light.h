#pragma once

#include "Color.h"
#include "Vec3.h"

class Light
{
public:
	Light(const Color& color, float intensity) : color(color), intensity(intensity) {}

	Color color;
	virtual Vec3 get_direction(const Point3& point) const = 0;
	virtual float get_intensity(const Point3& point) const = 0;
	void set_intensity(float value) {
		// throw exception with value < 0
		if (value >= 0)
			intensity = value;
	}

	virtual ~Light() = default;
protected:
	float intensity;
};

class PointLight : public Light {
public:
	Vec3 pos;
	PointLight(const Point3& pos, const Color& color, float intensity) : pos(pos), Light(color, intensity) {}

	float get_intensity(const Point3& point) const override {
		float dist = (point - pos).length();
		return intensity / (dist * dist);
	}

	Vec3 get_direction(const Point3& point) const override {
		Vec3 res = point - pos;
		res.normalize();
		return res;
	};
};

class DirectionLight : public Light {
public:
	Vec3 dir;
	DirectionLight(const Vec3& dir, const Color& color, float intensity) : dir(dir), Light(color, intensity) { this->dir.normalize(); }
	float get_intensity(const Vec3& point) const override { return intensity; }
	Vec3 get_direction(const Vec3& point) const override { return dir; }
};

// More types of light sources