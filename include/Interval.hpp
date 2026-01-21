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

    void stretch(Interval range) {
        this->stretch(range.min);
        this->stretch(range.max);
    }

    void move(Real offset) {
        min += offset;
        max += offset;
    }

    static const Interval empty, universe;
};

inline const Interval Interval::empty = Interval(+infinity, -infinity);
inline const Interval Interval::universe = Interval(-infinity, +infinity);