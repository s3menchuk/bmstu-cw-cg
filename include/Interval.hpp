#pragma once

#include "Types.hpp"

#include <limits>

constexpr Real infinity = std::numeric_limits<Real>::infinity();

class Interval {
  public:
    Real min, max;

    Interval(Real min, Real max) : min(min), max(max) {}
    Interval() : min(-infinity), max(infinity) {}

    Real size() const {
        return max - min;
    }

    bool contains(Real x) const {
        return min <= x && x <= max;
    }

    bool surrounds(Real x) const {
        return min < x && x < max;
    }

    Real clamp(Real x) const {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    void stretch(Real x) {
        if (x < min)
            min = x;
        else if (x > max)
            max = x;
    }

    static const Interval empty, universe;
};

const inline Interval Interval::empty = Interval(+infinity, -infinity);
const inline Interval Interval::universe = Interval(-infinity, +infinity);
