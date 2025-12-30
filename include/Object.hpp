#pragma once

#include "AABB.hpp"
#include "Interval.hpp"
#include "Material.hpp"
#include "Ray3.hpp"
#include "Types.hpp"
#include "Vec3.hpp"

#include <limits>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <vector>

Vec3 compute_centroid(const std::vector<Vec3> &points);

void sort_vertices_ccw3d(std::vector<Vec3> &vertices);

void move_points_by(std::vector<Point3> &points, const Vec3 &offset);

std::vector<Point3> get_points_on_circle(const Vec3 &center, T radius, size_t order);

// class Movable {
// public:
//	virtual void set_pos(const Vec3& new_pos) = 0;
//	virtual Vec3 get_pos() const = 0;
//	virtual ~Movable() = default;
// };

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    T dist;
};

class Hittable {
  public:
    virtual bool hit(const Ray3 &ray, HitRecord &hit_record) const = 0;
    // virtual AABB bounding_box() const = 0;
    virtual ~Hittable() = default;
};

template <typename Iter> bool hit(HitRecord &hit_record, const Ray3 &ray, Iter begin, Iter end) {
    HitRecord closest, current;
    closest.dist = std::numeric_limits<T>::max();
    for (Iter it = begin; it != end; ++it) {
        if (it.hit(ray, current) && current.dist < closest.dist) {
            closest = current;
        }
    }
    if (closest.dist == std::numeric_limits<T>::max()) {
        return false;
    }
    hit_record = closest;
    return true;
}

class Object {
  public:
    Object(const std::shared_ptr<Hittable> &hittable, const Material &material, bool visible = true)
        : hittable(hittable), material(material), visible(visible) {}

    bool hit(const Ray3 &ray, HitRecord &hit_record) const {
        return hittable->hit(ray, hit_record);
    }

    std::shared_ptr<Hittable> get() {
        return hittable;
    }

    Material material;
    bool visible;

  private:
    std::shared_ptr<Hittable> hittable;
};

class Sphere : public Hittable {
  public:
    Sphere(const Point3 &center, T radius) : center(center) {
        set_radius(radius);
    }

    Point3 center;

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    T get_radius() const {
        return radius;
    }

    void set_radius(T value) {
        if (value <= 0)
            throw std::invalid_argument("Radius must be positive");
        radius = value;
    }

  private:
    float radius;
};

class Plane : public Hittable {
  public:
    T offset;

    Plane(const Vec3 &normal, T offset) {
        set_normal(normal);
        this->offset = offset;
    }

    Vec3 get_normal() const {
        return normal;
    }

    void set_normal(const Vec3 &new_normal) {
        if (new_normal.length() == 0)
            throw std::invalid_argument("Normal vector must be non-zero");
        this->normal = new_normal;
        this->normal.normalize();
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

  private:
    Vec3 normal;
};

class Quad : public Hittable {
  public:
    Quad(const Point3 &Q, const Vec3 &u, const Vec3 &v) : Q(Q), u(u), v(v) {
        auto n = u.cross(v);
        normal = n.normalized();
        D = normal.dot(Q);
        w = n / n.dot(n);
    }

    Vec3 Q;
    Vec3 u, v;

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

  private:
    Vec3 w;
    Vec3 normal;
    T D;
};

class Triangle : public Hittable {
  public:
    Triangle(const Point3 &a, const Point3 &b, const Point3 &c) : a(a), b(b), c(c) {
        const auto v = (b - a).cross(c - a);
        normal = v / v.length();
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

  private:
    Point3 a, b, c;
    Vec3 normal;
};

class Mesh : public Hittable {
  public:
    Mesh(const std::vector<Triangle> &triangles) : triangles(triangles) {
        if (triangles.empty())
            throw std::invalid_argument("Must be at least 1 triangle");
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    static Mesh get_base(const Vec3 &center, T radius, size_t order, T offset) {
        std::vector<Point3> points = get_points_on_circle(center, radius, order);
        move_points_by(points, Vec3(offset, 0, 0));
        const Point3 lower_base_center = center + Vec3(offset, 0, 0);
        std::vector<Triangle> triangles;
        for (size_t i = 0; i < points.size(); ++i)
            triangles.push_back({points[i], points[(i + 1) % order], lower_base_center});
        return Mesh(triangles);
    }

  private:
    std::vector<Triangle> triangles;
};

class Box : public Hittable {
  public:
    Box(const Point3 &a, const Point3 &b) : a(a), b(b), sides(get_sides(a, b)), center((a + b) / 2.0f) {}

    static std::array<Quad, 6> get_sides(const Vec3 &a, const Vec3 &b) {
        auto min = Vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
        auto max = Vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

        auto dx = Vec3(max.x - min.x, 0, 0);
        auto dy = Vec3(0, max.y - min.y, 0);
        auto dz = Vec3(0, 0, max.z - min.z);

        std::array<Quad, 6> res = {
            Quad(Vec3(min.x, min.y, max.z), dx, dy),   // front
            Quad(Vec3(max.x, min.y, max.z), -dz, dy),  // right
            Quad(Vec3(max.x, min.y, min.z), -dx, dy),  // back
            Quad(Vec3(min.x, min.y, min.z), dz, dy),   // left
            Quad(Vec3(min.x, max.y, max.z), dx, -dz),  // top
            Quad(Vec3(min.x, min.y, min.z), dx, dz),   // bottom
        };

        return res;
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    void set_center(const Vec3 &new_center) {
        const Vec3 offset = new_center - center;
        *this = Box(a + offset, b + offset);
    }

    Point3 get_center() const {
        return center;
    }

  private:
    Point3 a;
    Point3 b;
    std::array<Quad, 6> sides;
    Point3 center;
};

class RightPrism : public Hittable {
  public:
    RightPrism(const Point3 &base_center, T radius, T height, size_t order)
        : upper_base(get_upper_base(base_center, radius, height, order)), lower_base(get_lower_base(base_center, radius, height, order)) {

        if (radius <= 0)
            throw std::invalid_argument("Radius must be positive");
        if (height <= 0)
            throw std::invalid_argument("Height must be positive");
        if (order < 3)
            throw std::invalid_argument("Vertex count must be at least 3");

        this->base_center = base_center;
        this->radius = radius;
        this->height = height;
        this->order = order;

        for (size_t i = 0; i < order; ++i) {
            T radians1 = static_cast<T>(i) / order * (2 * std::numbers::pi);
            T dx1 = std::cos(radians1);
            T dy1 = std::sin(radians1);
            Point3 q1 = base_center + Vec3(0, dx1, dy1) * radius + Vec3(height / 2, 0, 0);

            T radians2 = static_cast<T>((i + 1) % order) / order * (2 * std::numbers::pi);
            T dx2 = std::cos(radians2);
            T dy2 = std::sin(radians2);
            Point3 q2 = base_center + Vec3(0, dx2, dy2) * radius + Vec3(height / 2, 0, 0);

            Vec3 u = q2 - q1;
            Vec3 v(-height, 0, 0);
            Quad side_face(q1, u, v);
            side_faces.push_back(side_face);
        }
    }

    static Mesh get_upper_base(const Vec3 &center, T radius, T height, size_t order) {
        return Mesh::get_base(center, radius, order, height / 2);
    }
    static Mesh get_lower_base(const Vec3 &center, T radius, T height, size_t order) {
        return Mesh::get_base(center, radius, order, -height / 2);
    }

    Point3 get_base_center() const {
        return base_center;
    }

    void set_base_center(const Point3 &new_base_center) {
        *this = RightPrism(new_base_center, radius, height, order);
    }

    T get_radius() const {
        return radius;
    }

    void set_radius(T new_radius) {
        *this = RightPrism(base_center, new_radius, height, order);
    }

    T get_height() const {
        return height;
    }

    void set_height(T new_height) {
        *this = RightPrism(base_center, radius, new_height, order);
    }

    size_t get_order() const {
        return order;
    }

    void set_order(size_t new_order) {
        *this = RightPrism(base_center, radius, height, new_order);
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

  private:
    Vec3 base_center;
    T radius;
    T height;
    size_t order;

    std::vector<Quad> side_faces;
    Mesh upper_base;
    Mesh lower_base;
};

class RightPyramid : public Hittable {
  public:
    RightPyramid(const Point3 &base_center, T radius, T height, size_t order) : base(Mesh::get_base(base_center, radius, order, 0)) {
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

        const Point3 top = base_center - Vec3(height, 0, 0);
        const std::vector<Point3> points = get_points_on_circle(base_center, radius, order);
        for (size_t i = 0; i < points.size(); ++i) {
            side_faces.push_back({points[i], points[(i + 1) % order], top});
        }
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    Point3 get_base_center() const {
        return base_center;
    }

    void set_base_center(const Point3 &new_base_center) {
        *this = RightPyramid(new_base_center, radius, height, order);
    }

    T get_radius() const {
        return radius;
    }

    void set_radius(T new_radius) {
        *this = RightPyramid(base_center, new_radius, height, order);
    }

    T get_height() const {
        return height;
    }

    void set_height(T new_height) {
        *this = RightPyramid(base_center, radius, new_height, order);
    }

    size_t get_order() const {
        return order;
    }

    void set_order(size_t new_order) {
        *this = RightPyramid(base_center, radius, height, new_order);
    }

  private:
    Point3 base_center;
    T radius;
    T height;
    size_t order;

    std::vector<Triangle> side_faces;
    Mesh base;
};

class Model : public Hittable {
  public:
    Model(const std::vector<Vec3> &vertices, const std::vector<std::vector<size_t>> &faces) {
        for (const auto &face : faces) {
            std::vector<Vec3> coords;
            for (size_t i : face) {
                coords.push_back(vertices[i]);
            }
            sort_vertices_ccw3d(coords);
            for (size_t i = 1; i < coords.size() - 1; ++i) {
                Triangle t(coords[0], coords[i], coords[i + 1]);
                sides.push_back(std::make_shared<Triangle>(t));
            }
        }

        Interval x = Interval::empty;
        Interval y = Interval::empty;
        Interval z = Interval::empty;
        for (const auto &vertex : vertices) {
            x.stretch(vertex.x);
            y.stretch(vertex.y);
            z.stretch(vertex.z);
        }
        bbox = AABB(x, y, z);
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

  private:
    std::vector<std::shared_ptr<Hittable>> sides;
    AABB bbox;
};

class Translate : public Hittable {
  public:
    Translate(std::shared_ptr<Hittable> hittable, const Vec3 &offset) : hittable(hittable), offset(offset) {}

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override {
        Ray3 local_ray(ray.origin - offset, ray.direction);
        bool is_hit = hittable->hit(local_ray, hit_record);
        if (is_hit) {
            hit_record.point += offset;
        }
        return is_hit;
    }

  private:
    std::shared_ptr<Hittable> hittable;
    Vec3 offset;
};

class Rotate : public Hittable {
  public:
    Rotate(std::shared_ptr<Hittable> hittable, const Vec3 &axis, float angle) : hittable(hittable), axis(axis), angle(angle) {}

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override {
        Ray3 local_ray(ray);
        local_ray.origin.rotate(axis, -angle);
        local_ray.direction.rotate(axis, -angle);
        local_ray.direction.normalize();
        bool is_hit = hittable->hit(local_ray, hit_record);
        if (is_hit) {
            hit_record.point.rotate(axis, angle);
            hit_record.normal.rotate(axis, angle);
            hit_record.normal.normalized();
        }
        return is_hit;
    }

  private:
    std::shared_ptr<Hittable> hittable;
    Vec3 axis;
    float angle;
};

class Scale : public Hittable {
  public:
    Scale(std::shared_ptr<Hittable> hittable, const Vec3 &factor) : hittable(hittable), factor(factor) {}

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override {
        Ray3 local_ray(ray.origin / factor, (ray.direction / factor).normalized());
        bool is_hit = hittable->hit(local_ray, hit_record);
        if (is_hit) {
            hit_record.point = factor * hit_record.point;
            hit_record.normal = (factor * hit_record.normal).normalized();
            hit_record.dist *= factor;
        }
        return is_hit;
    }

  private:
    std::shared_ptr<Hittable> hittable;
    Vec3 factor;
};