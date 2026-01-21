#pragma once

#include "AABB.hpp"
#include "Interval.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Types.hpp"
#include "Vec.hpp"

#include <limits>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <vector>

Vec3 compute_centroid(const std::vector<Vec3> &points);

void sort_vertices_ccw3d(std::vector<Vec3> &vertices);

void move_points_by(std::vector<Point3> &points, Vec3 offset);

std::vector<Point3> get_points_on_circle(Vec3 center, Real radius, size_t order);

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    Real dist;
};

class Hittable {
  public:
    virtual bool hit(const Ray3 &ray, HitRecord &hit_record) const = 0;
    virtual AABB bounding_box() const = 0;
    virtual ~Hittable() = default;
};

template <typename Iter>
bool hit(HitRecord &hit_record, const Ray3 &ray, Iter begin, Iter end) {
    HitRecord closest, current;
    closest.dist = std::numeric_limits<Real>::max();
    for (Iter it = begin; it != end; ++it) {
        if (it.hit(ray, current) && current.dist < closest.dist) {
            closest = current;
        }
    }
    if (closest.dist == std::numeric_limits<Real>::max()) {
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
    Sphere(const Point3 &center, Real radius) : center(center) {
        set_radius(radius);
    }

    Point3 center;

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    AABB bounding_box() const override {
        Vec3 v(radius, radius, radius);
        return AABB(center - v, center + v);
    }

    Real get_radius() const {
        return radius;
    }

    void set_radius(Real value) {
        if (value <= 0)
            throw std::invalid_argument("Radius must be positive");
        radius = value;
    }

  private:
    float radius;
};

class Plane : public Hittable {
  public:
    Real offset;

    Plane(const Vec3 &normal, Real offset) {
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

    AABB bounding_box() const override {
        return AABB(Interval::universe, Interval::universe, Interval::universe);
    }

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

    AABB bounding_box() const override {
        return AABB(Q, Q + u + v);
    }

  private:
    Vec3 w;
    Vec3 normal;
    Real D;
};

class Triangle : public Hittable {
  public:
    Triangle(const Point3 &a, const Point3 &b, const Point3 &c) : a(a), b(b), c(c) {
        const auto v = (b - a).cross(c - a);
        normal = v / v.length();
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    AABB bounding_box() const override {
        AABB aabb;
        aabb.add_vertex(a);
        aabb.add_vertex(b);
        aabb.add_vertex(c);
        return aabb;
    }

    float calc_area() const {
        auto l1 = (b - a).length();
        auto l2 = (c - b).length();
        auto l3 = (a - c).length();
        auto p = (l1 + l2 + l3) / 2;
        return std::sqrt(p * (p - l1) * (p - l2) * (p - l3));
    }

    Vec3 barycentric(const Point3 &p) const;

    Point3 a, b, c;
    Vec3 normal;

  private:
};

class Mesh : public Hittable {
  public:
    Mesh(const std::vector<Triangle> &triangles) : triangles(triangles) {
        if (triangles.empty())
            throw std::invalid_argument("Must be at least 1 triangle");
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    AABB bounding_box() const override {
        AABB aabb;
        for (const auto &t : triangles) {
            aabb.add_aabb(t.bounding_box());
        }
        return aabb;
    }

    static Mesh get_base(const Vec3 &center, Real radius, size_t order, Real offset) {
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
    Box(const Point3 &a, const Point3 &b) : a(a), b(b), faces(get_sides(a, b)), center((a + b) / 2.0f) {}

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

    AABB bounding_box() const override {
        AABB aabb;
        for (const auto &f : faces) {
            aabb.add_aabb(f.bounding_box());
        }
        return aabb;
    }

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
    std::array<Quad, 6> faces;
    Point3 center;
};

class RightPrism : public Hittable {
  public:
    RightPrism(const Point3 &base_center, Real radius, Real height, size_t order)
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
            Real radians1 = static_cast<Real>(i) / order * (2 * std::numbers::pi);
            Real dx1 = std::cos(radians1);
            Real dy1 = std::sin(radians1);
            Point3 q1 = base_center + Vec3(0, dx1, dy1) * radius + Vec3(height / 2, 0, 0);

            Real radians2 = static_cast<Real>((i + 1) % order) / order * (2 * std::numbers::pi);
            Real dx2 = std::cos(radians2);
            Real dy2 = std::sin(radians2);
            Point3 q2 = base_center + Vec3(0, dx2, dy2) * radius + Vec3(height / 2, 0, 0);

            Vec3 u = q2 - q1;
            Vec3 v(-height, 0, 0);
            Quad side_face(q1, u, v);
            side_faces.push_back(side_face);
        }
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    AABB bounding_box() const override {
        AABB aabb;
        for (const auto &f : side_faces) {
            aabb.add_aabb(f.bounding_box());
        }
        aabb.add_aabb(upper_base.bounding_box());
        aabb.add_aabb(lower_base.bounding_box());
        return aabb;
    }

    static Mesh get_upper_base(const Vec3 &center, Real radius, Real height, size_t order) {
        return Mesh::get_base(center, radius, order, height / 2);
    }
    static Mesh get_lower_base(const Vec3 &center, Real radius, Real height, size_t order) {
        return Mesh::get_base(center, radius, order, -height / 2);
    }

    Point3 get_base_center() const {
        return base_center;
    }

    void set_base_center(const Point3 &new_base_center) {
        *this = RightPrism(new_base_center, radius, height, order);
    }

    Real get_radius() const {
        return radius;
    }

    void set_radius(Real new_radius) {
        *this = RightPrism(base_center, new_radius, height, order);
    }

    Real get_height() const {
        return height;
    }

    void set_height(Real new_height) {
        *this = RightPrism(base_center, radius, new_height, order);
    }

    size_t get_order() const {
        return order;
    }

    void set_order(size_t new_order) {
        *this = RightPrism(base_center, radius, height, new_order);
    }

  private:
    Vec3 base_center;
    Real radius;
    Real height;
    size_t order;

    std::vector<Quad> side_faces;
    Mesh upper_base;
    Mesh lower_base;
};

class RightPyramid : public Hittable {
  public:
    RightPyramid(const Point3 &base_center, Real radius, Real height, size_t order) : base(Mesh::get_base(base_center, radius, order, 0)) {
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

    AABB bounding_box() const override {
        AABB aabb;
        for (const auto &f : side_faces) {
            aabb.add_aabb(f.bounding_box());
        }
        aabb.add_aabb(base.bounding_box());
        return aabb;
    }

    Point3 get_base_center() const {
        return base_center;
    }

    void set_base_center(const Point3 &new_base_center) {
        *this = RightPyramid(new_base_center, radius, height, order);
    }

    Real get_radius() const {
        return radius;
    }

    void set_radius(Real new_radius) {
        *this = RightPyramid(base_center, new_radius, height, order);
    }

    Real get_height() const {
        return height;
    }

    void set_height(Real new_height) {
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
    Real radius;
    Real height;
    size_t order;

    std::vector<Triangle> side_faces;
    Mesh base;
};

struct VertexIndex {
    int v;
    int vn;
    int vt;
};

struct TriangleIndex {
    VertexIndex a, b, c;
};

struct Vertex {
    Vec3 pos;
    Vec3 norm;
    // Vec3 text;
};

class Model : public Hittable {
  public:
    Model(const std::vector<Vec3> &coords, const std::vector<std::vector<VertexIndex>> &faces, const std::vector<Vec3> &normals)
        : coords(coords), normals(normals), faces(faces) {
        for (const auto &face : faces) {
            for (size_t i = 1; i + 1 < face.size(); ++i) {
                Triangle t(coords[face[0].v], coords[face[i].v], coords[face[i + 1].v]);
                triangles.push_back(t);
                index_triangles.push_back({face[0], face[i], face[i + 1]});
            }
        }

        for (const auto &t : triangles) {
            aabb.add_aabb(t.bounding_box());
        }
    }

    bool hit(const Ray3 &ray, HitRecord &hit_record) const override;

    AABB bounding_box() const override {
        return aabb;
    }

  private:
    std::vector<Triangle> triangles;
    std::vector<TriangleIndex> index_triangles;
    AABB aabb;

    std::vector<Vec3> coords;
    std::vector<Vec3> normals;
    std::vector<std::vector<VertexIndex>> faces;
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

    AABB bounding_box() const override {
        AABB aabb = hittable->bounding_box();
        aabb.x.move(offset.x);
        aabb.y.move(offset.y);
        aabb.z.move(offset.z);
        return aabb;
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

    AABB bounding_box() const override {
        AABB aabb = hittable->bounding_box();
        return aabb;
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

    AABB bounding_box() const override {
        AABB aabb = hittable->bounding_box();
        aabb.x.min *= factor;
        aabb.x.max *= factor;
        aabb.y.min *= factor;
        aabb.y.max *= factor;
        aabb.z.min *= factor;
        aabb.z.max *= factor;
        return aabb;
    }

  private:
    std::shared_ptr<Hittable> hittable;
    Vec3 factor;
};