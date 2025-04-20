#pragma once

#include "Vector.h"
#include "Material.h"
#include "Ray.h"

#include <stdexcept>

class HitRecord {
public:
	Vector point;
	Vector normal;
	float dist;
};

class Object
{
public:
	Object(const Material& material) : material(material) {}
	Material material;
	virtual bool hit(const Ray& ray, HitRecord& hit_record) const = 0;

	virtual ~Object() {};
};

class Sphere : public Object
{
public:
	Vector center;

	Sphere(const Vector& center, float radius, const Material& material) : Object(material) {
		if (radius <= 0)
			throw std::runtime_error("Radius must be positive");
		this->radius = radius;
		this->center = center;
	}

	float get_radius() const { return radius; }
	void set_radius(float value) {
		// throw exception with value < (<= ???) 0
		if (value > 0)
			radius = value;
	}

	bool hit(const Ray& ray, HitRecord& hit_record) const override {
		Vector co = ray.origin - center;

		float a = ray.direction.sqr();
		float b = 2 * ray.direction.dot(co);
		float c = co.sqr() - std::pow(radius, 2);

		float D = b * b - 4 * a * c;
		if (D < 0)
			return false;
		float t1 = (-b - std::sqrt(D)) / (2 * a);
		float t2 = (-b + std::sqrt(D)) / (2 * a);

		float t_min = FLT_MAX;
		if (t1 >= 0)
			t_min = t1;
		if (t2 >= 0 && (t_min == FLT_MAX || t2 < t_min))
			t_min = t2;
		hit_record.dist = t_min;
		hit_record.point = ray.at(t_min);
		hit_record.normal = hit_record.point - center;
		//if (hit_record.normal.length() + EPSILON < radius)
		//	hit_record.normal = -hit_record.normal;
		hit_record.normal.normalize();
		return true;
	}
private:
	float radius;
};

class Plane : public Object
{
public:
	Vector normal;
	float offset;

	Plane(const Vector& normal, float offset, const Material& material) : normal(normal), offset(offset), Object(material) {};

	bool hit(const Ray& ray, HitRecord& hit_record) const override {
		float N = normal.dot(ray.origin) + offset;
		float D = normal.dot(ray.direction);
		if (D == 0)
			return false;
		float t = -N / D;
		if (t < 0)
			return false;
		hit_record.normal = normal;
		hit_record.dist = t;
		hit_record.point = ray.at(t);
		return true;
	}
};

// ...