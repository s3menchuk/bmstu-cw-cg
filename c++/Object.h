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
	virtual void move(const Vector& vector) = 0;
	//virtual void rotate(float angle) = 0;

	virtual ~Object() = default;
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

	void move(const Vector& vector) override { this->center += vector; }
private:
	float radius;
};

class Plane : public Object
{
public:
	Vector normal;
	float offset;

	Plane(const Vector& normal, float offset, const Material& material) : normal(normal), offset(offset), Object(material) {}

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

	void move(const Vector& vector) override { offset -= normal.dot(vector); }
};

class Quad : public Object {
public:
	Quad(const Vector& Q, const Vector& u, const Vector& v, const Material& material) : Q(Q), u(u), v(v), Object(material) {
		Vector n = u.cross(v);
		normal = n;
		normal.normalize();
		D = normal.dot(Q);
		w = n / n.dot(n);
	}

	bool hit(const Ray& ray, HitRecord& hit_record) const override {
		auto denom = normal.dot(ray.direction);

		if (std::fabs(denom) < 1e-8)
			return false;

		auto t = (D - normal.dot(ray.origin)) / denom;

		if (t < 0)
			return false;

		auto intersection = ray.at(t);
		Vector planar_hitpt_vector = intersection - Q;
		auto alpha = w.dot(planar_hitpt_vector.cross(v));
		auto beta = w.dot(u.cross(planar_hitpt_vector));

		if (!(0 <= alpha && alpha <= 1 && 0 <= beta && beta <= 1))
			return false;

		hit_record.dist = t;
		hit_record.point = intersection;
		hit_record.normal = normal;

		return true;
	}

	void move(const Vector& vector) override { Q += vector; }
private:
	Vector Q;
	Vector u, v;
	Vector w;
	Vector normal;
	double D;
};

class Box : public Object {
public:
	std::array<std::shared_ptr<Quad>, 6> sides;

	Box(const Vector& a, const Vector& b, const Material& material) : Object(material) {
		auto min = Vector(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
		auto max = Vector(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

		auto dx = Vector(max.x - min.x, 0, 0);
		auto dy = Vector(0, max.y - min.y, 0);
		auto dz = Vector(0, 0, max.z - min.z);

		sides[0] = std::make_shared<Quad>(Vector(min.x, min.y, max.z), dx, dy, material);  // front
		sides[1] = std::make_shared<Quad>(Vector(max.x, min.y, max.z), -dz, dy, material);  // right
		sides[2] = std::make_shared<Quad>(Vector(max.x, min.y, min.z), -dx, dy, material);  // back
		sides[3] = std::make_shared<Quad>(Vector(min.x, min.y, min.z), dz, dy, material);  // left
		sides[4] = std::make_shared<Quad>(Vector(min.x, max.y, max.z), dx, -dz, material);  // top
		sides[5] = std::make_shared<Quad>(Vector(min.x, min.y, min.z), dx, dz, material);  // bottom
	}

	bool hit(const Ray& ray, HitRecord& hit_record) const override {
		float min_dist = FLT_MAX;
		HitRecord temp_hit_record;
		for (const auto& side : sides) {
			if (side->hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
				min_dist = temp_hit_record.dist;
				hit_record = temp_hit_record;
			}
		}
		return min_dist != FLT_MAX;
	}

	void move(const Vector& vector) override {
		for (const auto& side : sides)
			side->move(vector);
	}
};

class RightPrism : public Object {
public:
	RightPrism(const Vector& center, float height, size_t sides, const Material& material) : center(center), sides(sides), Object(material) {
		if (height <= 0)
			throw std::invalid_argument("Height must be positive");
		this->height = height;
	}
private:
	Vector center;
	float height;
	size_t sides;
};

// Pyramid