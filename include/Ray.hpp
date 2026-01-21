#pragma once

#include "Types.hpp"
#include "Vec.hpp"

template <typename T>
class Ray {
  public:
    Ray(Vector3<T> origin, Vector3<T> direction) : origin(origin), direction(direction) {};

    Vector3<T> origin;
    Vector3<T> direction;

    Vector3<T> at(T dist) const {
        return origin + direction * dist;
    }
};

using Ray3 = Ray<Real>;
