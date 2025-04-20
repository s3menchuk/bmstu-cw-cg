#pragma once

#include "Color.h"
#include "Vector.h"

class Light
{
public:
	Light(const Color& color, float intensity) : color(color), intensity(intensity) {}

	Color color;
	virtual Vector get_direction(const Vector& point) const = 0;
	virtual float get_intensity(const Vector& point) const = 0;
	void set_intensity(float value) {
		// throw exception with value < 0
		if (value >= 0)
			intensity = value;
	}

	virtual ~Light() {};
protected:
	float intensity;
};

class PointLight : public Light {
public:
	Vector pos;
	PointLight(const Vector& pos, const Color& color, float intensity) : pos(pos), Light(color, intensity) {}
	virtual float get_intensity(const Vector& point) const override {
		float dist = (point - pos).length();
		return intensity / (dist * dist);
	}
	virtual Vector get_direction(const Vector& point) const override {
		Vector res = point - pos;
		res.normalize();
		return res;
	};
};

class DirectionLight : public Light {
public:
	Vector dir;
	DirectionLight(const Vector& dir, const Color& color, float intensity) : dir(dir), Light(color, intensity) { this->dir.normalize(); }
	virtual float get_intensity(const Vector& point) const override { return intensity; }
	virtual Vector get_direction(const Vector& point) const override { return dir; }
};

// More types of light sources