#pragma once

#include "Interval.hpp"
#include "Ray.hpp"
#include "Types.hpp"
#include "Vec.hpp"

#include <vector>

class AABB {
  public:
    Interval x, y, z;

    AABB() : x(Interval::empty), y(Interval::empty), z(Interval::empty) {}
    AABB(Interval x, Interval y, Interval z) : x(x), y(y), z(z) {}
    AABB(Point3 a, Point3 b) {
        x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
    }
    AABB(const std::vector<Point3> &vertices) {
        x = Interval::empty;
        y = Interval::empty;
        z = Interval::empty;
        for (const auto &vertex : vertices) {
            x.stretch(vertex.x);
            y.stretch(vertex.y);
            z.stretch(vertex.z);
        }
    }

    void add_vertex(Point3 vertex) {
        x.stretch(vertex.x);
        y.stretch(vertex.y);
        z.stretch(vertex.z);
    }

    void add_aabb(AABB other) {}

    const Interval &operator[](int n) const {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    bool hit(Ray3 ray, Interval range = Interval::universe) const {
        for (int axis = 0; axis < 3; axis++) {
            const Interval &ax = (*this)[axis];
            const double adinv = 1.0 / ray.direction[axis];

            auto t0 = (ax.min - ray.origin[axis]) * adinv;
            auto t1 = (ax.max - ray.origin[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > range.min)
                    range.min = t0;
                if (t1 < range.max)
                    range.max = t1;
            } else {
                if (t1 > range.min)
                    range.min = t1;
                if (t0 < range.max)
                    range.max = t0;
            }

            if (range.max <= range.min)
                return false;
        }
        return true;
    }
};
