#include "Object.hpp"

#include <numbers>
#include <vector>

void move_points_by(std::vector<Point3> &points, Vec3 offset) {
    for (Point3 &p : points)
        p += offset;
}

std::vector<Point3> get_points_on_circle(Vec3 center, Real radius, size_t order) {
    std::vector<Point3> res;
    for (size_t i = 0; i < order; ++i) {
        auto radians = static_cast<Real>(i) / order * (2 * std::numbers::pi);
        auto dx = std::cos(radians);
        auto dz = std::sin(radians);
        Point3 point = center + Vec3(dx, 0, dz) * radius;
        res.push_back(point);
    }
    return res;
}

Vec3 compute_centroid(const std::vector<Vec3> &points) {
    Vec3 c(0, 0, 0);
    for (const auto &p : points)
        c += p;
    return c / static_cast<Real>(points.size());
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

Vec3 Triangle::barycentric(const Point3 &p) const {
    auto area = calc_area();
    Triangle t_a(p, b, c);
    Triangle t_b(p, a, c);
    Triangle t_c(p, a, b);
    auto a = t_a.calc_area() / area;
    auto b = t_b.calc_area() / area;
    auto c = t_c.calc_area() / area;
    return {a, b, c};
}

template <typename T>
concept HasHit = requires(T t, const Ray3 &ray, HitRecord &hit_record) {
    { t.hit(ray, hit_record) } -> std::same_as<bool>;
};

template <std::input_iterator It>
    requires HasHit<std::iter_value_t<It>>
It find_closest_hit(const Ray3 &ray, It begin, It end, HitRecord &hit_record) {
    Real min_dist = std::numeric_limits<Real>::max();
    HitRecord temp_hit_record;
    It closest_obj = end;
    for (auto it = begin; it != end; ++it) {
        if (it->hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
            min_dist = temp_hit_record.dist;
            hit_record = temp_hit_record;
            closest_obj = it;
        }
    }
    return closest_obj;
}

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
    Real t = -N / D;
    if (t <= 0)
        return false;
    hit_record.dist = t;
    hit_record.point = ray.at(t);
    hit_record.normal = normal.dot(ray.direction) < 0 ? normal : -normal;
    return true;
}

bool Quad::hit(const Ray3 &ray, HitRecord &hit_record) const {
    auto denom = normal.dot(ray.direction);

    if (std::abs(denom) < std::numeric_limits<Real>::epsilon())
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
    constexpr Real EPSILON = std::numeric_limits<Real>::epsilon();
    auto edge_ab = b - a;
    auto edge_ac = c - a;

    auto pvec = ray.direction.cross(edge_ac);
    auto det = edge_ab.dot(pvec);

    if (std::abs(det) < EPSILON)
        return false;

    auto inv_det = 1 / det;
    auto tvec = ray.origin - a;
    auto u = inv_det * tvec.dot(pvec);
    if (u < 0 || u > 1)
        return false;

    auto qvec = tvec.cross(edge_ab);
    auto v = inv_det * ray.direction.dot(qvec);
    if (v < 0 || u + v > 1)
        return false;

    auto t = inv_det * edge_ac.dot(qvec);
    if (t < EPSILON) {
        return false;
    }
    hit_record.dist = t;
    hit_record.point = ray.at(t);
    auto normal = edge_ab.cross(edge_ac).normalized();
    hit_record.normal = normal.dot(ray.direction) < 0 ? normal : -normal;
    return true;
}

bool Mesh::hit(const Ray3 &ray, HitRecord &hit_record) const {
    return find_closest_hit(ray, triangles.begin(), triangles.end(), hit_record) != triangles.end();
}

bool Box::hit(const Ray3 &ray, HitRecord &hit_record) const {
    return find_closest_hit(ray, faces.begin(), faces.end(), hit_record) != faces.end();
}

bool RightPrism::hit(const Ray3 &ray, HitRecord &hit_record) const {
    Real min_dist = std::numeric_limits<Real>::max();
    if (find_closest_hit(ray, side_faces.begin(), side_faces.end(), hit_record) != side_faces.end()) {
        min_dist = hit_record.dist;
    }
    HitRecord temp_hit_record;
    if (upper_base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
        min_dist = temp_hit_record.dist;
        hit_record = temp_hit_record;
    }
    if (lower_base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
        min_dist = temp_hit_record.dist;
        hit_record = temp_hit_record;
    }
    return min_dist != std::numeric_limits<Real>::max();
}

bool RightPyramid::hit(const Ray3 &ray, HitRecord &hit_record) const {
    Real min_dist = std::numeric_limits<Real>::max();
    if (find_closest_hit(ray, side_faces.begin(), side_faces.end(), hit_record) != side_faces.end()) {
        min_dist = hit_record.dist;
    }
    HitRecord temp_hit_record;
    if (base.hit(ray, temp_hit_record) && temp_hit_record.dist < min_dist) {
        min_dist = temp_hit_record.dist;
        hit_record = temp_hit_record;
    }
    return min_dist != std::numeric_limits<Real>::max();
}

bool Model::hit(const Ray3 &ray, HitRecord &hit_record) const {
    if (!aabb.hit(ray))
        return false;
    auto it = find_closest_hit(ray, triangles.begin(), triangles.end(), hit_record);
    if (it == triangles.end())
        return false;
    int triangle_index = it - triangles.begin();
    const TriangleIndex &info = index_triangles[triangle_index];
    const Vertex va = {coords[info.a.v], normals[info.a.vn]};
    const Vertex vb = {coords[info.b.v], normals[info.b.vn]};
    const Vertex vc = {coords[info.c.v], normals[info.c.vn]};
    Triangle t(va.pos, vb.pos, vc.pos);
    Vec3 coeffs = t.barycentric(hit_record.point);
    hit_record.normal = (va.norm * coeffs.x + vb.norm * coeffs.y + vc.norm * coeffs.z).normalized();
    return true;
}