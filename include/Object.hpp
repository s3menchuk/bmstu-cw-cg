#pragma once

#include "Vec3.hpp"
#include "Material.hpp"
#include "Ray.hpp"

#include <stdexcept>
#include <numbers>

void move_points_by(std::vector<Point3>& points, const Vec3& offset);
std::vector<Point3> get_points_on_circle(const Vec3& center, float radius, size_t order);

class HitRecord {
public:
	Vec3 point;
	Vec3 normal;
	float dist;
};

class Hittable {
public:
	virtual bool hit(const Ray3& ray, HitRecord& hit_record) const = 0;
	virtual ~Hittable() = default;
};

class Movable {
public:
	virtual void set_pos(const Vec3& new_pos) = 0;
	virtual Vec3 get_pos() const = 0;
	virtual ~Movable() = default;
};

class GeometricPrimitive : public Hittable, public Movable {
public:
	virtual ~GeometricPrimitive() = default;
};

class Object {
public:
	Object(const std::shared_ptr<GeometricPrimitive>& primitive, const Material& material) : primitive(primitive), material(material) {}
	std::shared_ptr<GeometricPrimitive> operator->() { return primitive; }
	std::shared_ptr<const GeometricPrimitive> operator->() const { return primitive; }
	Material material;
private:
	std::shared_ptr<GeometricPrimitive> primitive;
};

class Sphere : public GeometricPrimitive
{
public:
	Sphere(const Point3& center, float radius) {
		set_radius(radius);
		this->center = center;
	}

	float get_radius() const { return radius; }

	void set_radius(float value) {
		if (value <= 0)
			throw std::invalid_argument("Radius must be positive");
		radius = value;
	}

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
		Vec3 co = ray.origin - center;

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

	void set_pos(const Vec3& new_pos) override { center = new_pos; }

	Vec3 get_pos() const override { return center; }
private:
	Vec3 center;
	float radius;
};

class Plane : public GeometricPrimitive
{
public:
	Vec3 normal;
	float offset;

	Plane(const Vec3& normal, float offset) : normal(normal), offset(offset) {}

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
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

	void set_pos(const Vec3& new_pos) override { offset = -normal.dot(new_pos); }

	Vec3 get_pos() const override { return normal.x ? Vec3(-offset / normal.x, 0, 0) : Vec3(0, 0, 0); }
};

class Quad : public GeometricPrimitive {
public:
	Quad(const Vec3& Q, const Vec3& u, const Vec3& v) : Q(Q), u(u), v(v) {
		Vec3 n = u.cross(v);
		normal = n;
		normal.normalize();
		D = normal.dot(Q);
		w = n / n.dot(n);
	}

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
		auto denom = normal.dot(ray.direction);

		if (std::fabs(denom) < 1e-8)
			return false;

		auto t = (D - normal.dot(ray.origin)) / denom;

		if (t < 0)
			return false;

		auto intersection = ray.at(t);
		Vec3 planar_hitpt_vector = intersection - Q;
		auto alpha = w.dot(planar_hitpt_vector.cross(v));
		auto beta = w.dot(u.cross(planar_hitpt_vector));

		if (!(0 <= alpha && alpha <= 1 && 0 <= beta && beta <= 1))
			return false;

		hit_record.dist = t;
		hit_record.point = intersection;
		hit_record.normal = normal;

		return true;
	}

	void set_pos(const Vec3& new_pos) override { Q = new_pos; }

	Vec3 get_pos() const override { return Q; }
private:
	Vec3 Q;
	Vec3 u, v;
	Vec3 w;
	Vec3 normal;
	double D;
};

class Triangle : public GeometricPrimitive {
public:
	Triangle(const Point3& a, const Point3& b, const Point3& c) : a(a), b(b), c(c) {
		const Vec3 v = (b - a).cross(c - a);
		normal = v / v.length();
	}

	bool hit(const Ray3& ray, HitRecord& hit_record) const {
		const float EPSILON = 1e-6f;
		Vec3 edge1 = b - a;
		Vec3 edge2 = c - a;

		Vec3 h = ray.direction.cross(edge2);
		float det = edge1.dot(h);

		if (std::abs(det) < EPSILON)
			return false;  // ��� ���������� ��������� ������������

		float inv_det = 1.0f / det;
		Vec3 s = ray.origin - a;
		float u = inv_det * s.dot(h);
		if (u < 0.0f || u > 1.0f)
			return false;

		Vec3 q = s.cross(edge1);
		float v = inv_det * ray.direction.dot(q);
		if (v < 0.0f || u + v > 1.0f)
			return false;

		float t = inv_det * edge2.dot(q);
		if (t > EPSILON) {
			hit_record.dist = t;
			hit_record.point = ray.at(t);
			hit_record.normal = edge1.cross(edge2);
			hit_record.normal.normalize();
			return true;
		}

		return false;
	}

	void set_pos(const Point3& new_pos) override { a = new_pos; };
	Vec3 get_pos() const override { return a; };
private:
	Point3 a, b, c;
	Vec3 normal;
};

class Mesh : public GeometricPrimitive {
public:
	Mesh(const std::vector<Triangle>& triangles) : triangles(triangles) {
		if (triangles.empty())
			throw std::invalid_argument("Must be at least 1 triangle");
	}

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
		float min_dist = FLT_MAX;
		HitRecord temp_hit_record;
		for (const auto& t : triangles) {
			if (t.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
				min_dist = temp_hit_record.dist;
				hit_record = temp_hit_record;
			}
		}
		return min_dist != FLT_MAX;
	}

	void set_pos(const Point3& new_pos) override { /*TODO*/ };
	Vec3 get_pos() const override { return triangles[0].get_pos(); /*TODO*/ };

	static Mesh get_base(const Vec3& center, float radius, size_t order, float offset) {
		std::vector<Point3> points = get_points_on_circle(center, radius, order);
		move_points_by(points, Vec3(offset, 0, 0));
		const Point3 lower_base_center = center + Vec3(offset, 0, 0);
		std::vector<Triangle> triangles;
		for (size_t i = 0; i < points.size(); ++i)
			triangles.push_back({ points[i], points[(i + 1) % order], lower_base_center });
		return Mesh(triangles);
	}
private:
	std::vector<Triangle> triangles;
};

class Box : public GeometricPrimitive {
public:
	Box(const Vec3& a, const Vec3& b) : a(a), b(b), center((a + b) / 2), sides(get_sides(a, b)) {}

	static std::array<Quad, 6> get_sides(const Vec3&a, const Vec3& b) {
		auto min = Vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
		auto max = Vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

		auto dx = Vec3(max.x - min.x, 0, 0);
		auto dy = Vec3(0, max.y - min.y, 0);
		auto dz = Vec3(0, 0, max.z - min.z);

		std::array<Quad, 6> res = {
			Quad(Vec3(min.x, min.y, max.z), dx, dy),  // front
			Quad(Vec3(max.x, min.y, max.z), -dz, dy),  // right
			Quad(Vec3(max.x, min.y, min.z), -dx, dy),  // back
			Quad(Vec3(min.x, min.y, min.z), dz, dy),  // left
			Quad(Vec3(min.x, max.y, max.z), dx, -dz),  // top
			Quad(Vec3(min.x, min.y, min.z), dx, dz),  // bottom
		};

		return res;
	}

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
		float min_dist = FLT_MAX;
		HitRecord temp_hit_record;
		for (const auto& side : sides) {
			if (side.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
				min_dist = temp_hit_record.dist;
				hit_record = temp_hit_record;
			}
		}
		return min_dist != FLT_MAX;
	}

	void set_pos(const Vec3& new_pos) override {
		const Vec3 offset = new_pos - center;
		*this = Box(a + offset, b + offset);
	}

	Vec3 get_pos() const override { return center; }
private:
	std::array<Quad, 6> sides;
	Vec3 center;
	Vec3 a;
	Vec3 b;
};

class RightPrism : public GeometricPrimitive {
public:
	RightPrism(const Vec3& center, float radius, float height, size_t order) :
		upper_base(get_upper_base(center, radius, height, order)),
		lower_base(get_lower_base(center, radius, height, order)) {

		if (radius <= 0)
			throw std::invalid_argument("Radius must be positive");
		this->radius = radius;

		if (height <= 0)
			throw std::invalid_argument("Height must be positive");
		this->height = height;

		if (order < 3)
			throw std::invalid_argument("Vertex count must be at least 3");
		this->order = order;

		this->center = center;

		for (size_t i = 0; i < order; ++i) {
			float radians1 = static_cast<float>(i) / order * (2 * std::numbers::pi);
			float dx1 = std::cos(radians1);
			float dy1 = std::sin(radians1);
			Point3 q1 = center + Vec3(0, dx1, dy1) * radius + Vec3(height / 2, 0, 0);

			float radians2 = static_cast<float>((i + 1) % order) / order * (2 * std::numbers::pi);
			float dx2 = std::cos(radians2);
			float dy2 = std::sin(radians2);
			Point3 q2 = center + Vec3(0, dx2, dy2) * radius + Vec3(height / 2, 0, 0);
			
			Vec3 u = q2 - q1;
			Vec3 v(-height, 0, 0);
			Quad side_face(q1, u, v);
			side_faces.push_back(side_face);
		}
	}

	static Mesh get_upper_base(const Vec3& center, float radius, float height, size_t order) {
		return Mesh::get_base(center, radius, order, height / 2);
	}
	static Mesh get_lower_base(const Vec3& center, float radius, float height, size_t order) {
		return Mesh::get_base(center, radius, order, -height / 2);
	}

	float get_radius() const { return radius; }

	float get_height() const { return height; }

	size_t get_order() const { return order; }

	void set_pos(const Vec3& new_pos) override { center = new_pos; }

	Vec3 get_pos() const override { return center; }

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
		float min_dist = FLT_MAX;
		HitRecord temp_hit_record;
		for (const auto& side : side_faces) {
			if (side.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
				min_dist = temp_hit_record.dist;
				hit_record = temp_hit_record;
			}
		}
		if (upper_base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
			min_dist = temp_hit_record.dist;
			hit_record = temp_hit_record;
		}
		if (lower_base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
			min_dist = temp_hit_record.dist;
			hit_record = temp_hit_record;
		}
		return min_dist != FLT_MAX;
	}
private:
	Vec3 center;
	float radius;
	float height;
	size_t order;
	
	std::vector<Quad> side_faces;
	Mesh upper_base;
	Mesh lower_base;
};

class RightPyramid : public GeometricPrimitive {
public:
	RightPyramid(const Point3& base_center, float radius, float height, size_t order) : base(Mesh::get_base(base_center, radius, order, 0)) {
		if (radius <= 0)
			throw std::invalid_argument("Radius must be positive");
		if (height <= 0)
			throw std::invalid_argument("Height must be positive");
		if (order < 3)
			throw std::invalid_argument("Order must be more than 3");
		this->base_center = base_center;
		this->radius = radius;
		this->height = height;
		this->order = order;

		const Point3 top = base_center - height;
		const std::vector<Point3> points = get_points_on_circle(base_center, radius, order);
		for (size_t i = 0; i < points.size(); ++i) {
			side_faces.push_back( {points[i], points[(i + 1) % order], top} );
		}
	}

	bool hit(const Ray3& ray, HitRecord& hit_record) const override {
		float min_dist = FLT_MAX;
		HitRecord temp_hit_record;
		for (const auto& side : side_faces) {
			if (side.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
				min_dist = temp_hit_record.dist;
				hit_record = temp_hit_record;
			}
		}
		if (base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
			min_dist = temp_hit_record.dist;
			hit_record = temp_hit_record;
		}
		return min_dist != FLT_MAX;
	}

	void set_pos(const Vec3& new_pos) override { base_center = new_pos; }

	Vec3 get_pos() const override { return base_center; }
private:
	Point3 base_center;
	float radius;
	float height;
	size_t order;

	std::vector<Triangle> side_faces;
	Mesh base;
};

