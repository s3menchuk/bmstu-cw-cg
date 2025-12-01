#pragma once

#include "Types.hpp"

#include <limits>

constexpr T infinity = std::numeric_limits<T>::infinity();

class Interval {
  public:
	T min, max;

	constexpr Interval(T min, T max) : min(min), max(max) {}
	constexpr Interval() : min(-infinity), max(infinity) {}

	T size() const { return max - min; }

	bool contains(T x) const { return min <= x && x <= max; }

	bool surrounds(T x) const { return min < x && x < max; }

	T clamp(T x) const {
		if (x < min)
			return min;
		if (x > max)
			return max;
		return x;
	}

	void stretch(T x) {
		if (x < min)
			min = x;
		else if (x > max)
			max = x;
	}

	static const Interval empty, universe;
};

const inline Interval Interval::empty = Interval(+infinity, -infinity);
const inline Interval Interval::universe = Interval(-infinity, +infinity);