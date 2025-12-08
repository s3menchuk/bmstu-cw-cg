#pragma once

#include "Types.hpp"
#include "Vec3.hpp"

class Ray3 {
  public:
    Ray3(const Point3 &origin, const Vec3 &direction) : origin(origin), direction(direction) {};
    Point3 origin;
    Vec3 direction;

    Point3 at(T dist) const {
        return origin + direction * dist;
    }
};
