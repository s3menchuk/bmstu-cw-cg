#pragma once

#include "Vec3.h"
#include "types.h"

class Ray3
{
public:
	Ray3(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction) {};
	Point3 origin;
	Vec3 direction;

	Vec3 at(T dist) const { return origin + direction * dist; }
};