#include "Interval.hpp"

const Interval Interval::empty = Interval(+infinity, -infinity);

const Interval Interval::universe = Interval(-infinity, +infinity);