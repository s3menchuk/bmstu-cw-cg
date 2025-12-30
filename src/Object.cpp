#include "Object.hpp"

#include <numbers>
#include <vector>

void move_points_by(std::vector<Point3> &points, const Vec3 &offset) {
    for (Point3 &p : points)
        p += offset;
}

std::vector<Point3> get_points_on_circle(const Vec3 &center, T radius, size_t order) {
    std::vector<Point3> res;
    for (size_t i = 0; i < order; ++i) {
        auto radians = static_cast<T>(i) / order * (2 * std::numbers::pi);
        auto dx = std::cos(radians);
        auto dy = std::sin(radians);
        Point3 point = center + Vec3(0, dx, dy) * radius;
        res.push_back(point);
    }
    return res;
}

Vec3 compute_centroid(const std::vector<Vec3> &points) {
    Vec3 c(0, 0, 0);
    for (const auto &p : points)
        c += p;
    return c / static_cast<T>(points.size());
}

void sort_vertices_ccw3d(std::vector<Vec3> &vertices) {
    if (vertices.size() < 3)
        return;

    Vec3 center = compute_centroid(vertices);
    Vec3 v1 = vertices[0] - center;
    Vec3 v2 = vertices[1] - center;

    Vec3 normal = v1.cross(v2);
    normal.normalize();

    Vec3 u = v1;
    u.normalize();
    Vec3 v = normal.cross(u);
    v.normalize();

    std::sort(vertices.begin(), vertices.end(), [&](const Vec3 &a, const Vec3 &b) {
        Vec3 da = a - center;
        Vec3 db = b - center;
        double angleA = std::atan2(da.dot(v), da.dot(u));
        double angleB = std::atan2(db.dot(v), db.dot(u));
        return angleA < angleB;
    });
}

// Classic
// bool Sphere::hit(const Ray3 &ray, HitRecord &hit_record) const{
//     Vec3 co = ray.origin - center;
//     auto a = ray.direction.sqr();
//     auto b = 2 * ray.direction.dot(co);
//     auto c = co.sqr() - std::pow(radius, 2);
//     auto D = b * b - 4 * a * c;
//     if (D < 0)
//         return false;
//     auto t1 = (-b - std::sqrt(D)) / (2 * a);
//     auto t2 = (-b + std::sqrt(D)) / (2 * a);
//     if (t2 < 0)
//         return false;
//     auto t_min = t1 >= 0 ? t1 : t2;
//     hit_record.dist = t_min;
//     hit_record.point = ray.at(t_min);
//     hit_record.normal = (hit_record.point - center).normalized();
//     return true;
// }

// Optimized
bool Sphere::hit(const Ray3 &ray, HitRecord &hit_record) const {
    Vec3 oc = ray.origin - center;
    auto dot = oc.dot(ray.direction);
    auto D = dot * dot - oc.dot(oc) + radius * radius;
    if (D < 0)
        return false;
    auto t2 = -dot + std::sqrt(D);
    if (t2 < 0)
        return false;
    auto t1 = -dot - std::sqrt(D);
    auto t_min = t1 > 0 ? t1 : t2;
    hit_record.dist = t_min;
    hit_record.point = ray.at(t_min);
    hit_record.normal = (hit_record.point - center).normalized();
    return true;
}

bool Plane::hit(const Ray3 &ray, HitRecord &hit_record) const {
    float N = normal.dot(ray.origin) + offset;
    float D = normal.dot(ray.direction);
    if (D == 0)
        return false;
    T t = -N / D;
    if (t < 0)
        return false;
    hit_record.dist = t;
    hit_record.point = ray.at(t);
    hit_record.normal = normal.dot(ray.direction) < 0 ? normal : -normal;
    return true;
}

bool Quad::hit(const Ray3 &ray, HitRecord &hit_record) const {
    auto denom = normal.dot(ray.direction);

    if (std::abs(denom) < std::numeric_limits<T>::epsilon())
        return false;

    auto t = (D - normal.dot(ray.origin)) / denom;

    if (t < 0)
        return false;

    auto intersection = ray.at(t);
    Vec3 planar_hitpt_vector = intersection - Q;
    auto alpha = w.dot(planar_hitpt_vector.cross(v));
    auto beta = w.dot(u.cross(planar_hitpt_vector));

    Interval interval(0, 1);
    if (!(interval.contains(alpha) && interval.contains(beta)))
        return false;

    hit_record.dist = t;
    hit_record.point = intersection;
    hit_record.normal = normal.dot(ray.direction) < 0 ? normal : -normal;

    return true;
}

bool Triangle::hit(const Ray3 &ray, HitRecord &hit_record) const {
    constexpr T EPSILON = std::numeric_limits<T>::epsilon();
    auto edge1 = b - a;
    auto edge2 = c - a;

    auto h = ray.direction.cross(edge2);
    auto det = edge1.dot(h);

    if (std::abs(det) < EPSILON)
        return false;

    auto inv_det = 1.0 / det;
    auto s = ray.origin - a;
    auto u = inv_det * s.dot(h);
    if (u < 0.0 || u > 1.0)
        return false;

    auto q = s.cross(edge1);
    auto v = inv_det * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;

    auto t = inv_det * edge2.dot(q);
    if (t > EPSILON) {
        hit_record.dist = t;
        hit_record.point = ray.at(t);
        auto normal = edge1.cross(edge2).normalized();
        hit_record.normal = normal.dot(ray.direction) < 0 ? normal : -normal;
        return true;
    }

    return false;
}

bool Mesh::hit(const Ray3 &ray, HitRecord &hit_record) const {
    T min_dist = std::numeric_limits<T>::max();
    HitRecord temp_hit_record;
    for (const auto &t : triangles) {
        if (t.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
            min_dist = temp_hit_record.dist;
            hit_record = temp_hit_record;
        }
    }
    return min_dist != std::numeric_limits<T>::max();
}

bool Box::hit(const Ray3 &ray, HitRecord &hit_record) const {
    T min_dist = std::numeric_limits<T>::max();
    HitRecord temp_hit_record;
    for (const auto &side : sides) {
        if (side.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
            min_dist = temp_hit_record.dist;
            hit_record = temp_hit_record;
        }
    }
    return min_dist != std::numeric_limits<T>::max();
}

bool RightPrism::hit(const Ray3 &ray, HitRecord &hit_record) const {
    T min_dist = std::numeric_limits<T>::max();
    HitRecord temp_hit_record;
    for (const auto &side : side_faces) {
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
    return min_dist != std::numeric_limits<T>::max();
}

bool RightPyramid::hit(const Ray3 &ray, HitRecord &hit_record) const {
    T min_dist = std::numeric_limits<T>::max();
    HitRecord temp_hit_record;
    for (const auto &side : side_faces) {
        if (side.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
            min_dist = temp_hit_record.dist;
            hit_record = temp_hit_record;
        }
    }
    if (base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
        min_dist = temp_hit_record.dist;
        hit_record = temp_hit_record;
    }
    return min_dist != std::numeric_limits<T>::max();
}

bool Model::hit(const Ray3 &ray, HitRecord &hit_record) const {
    if (!bbox.hit(ray, Interval::universe))
        return false;
    T min_dist = std::numeric_limits<T>::max();
    HitRecord temp_hit_record;
    for (const auto &side : sides) {
        if (side->hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
            min_dist = temp_hit_record.dist;
            hit_record = temp_hit_record;
        }
    }
    return min_dist != std::numeric_limits<T>::max();
}