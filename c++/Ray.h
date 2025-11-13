#pragma once

#include "Vec3.h"

class Ray3
{
public:
	Ray3(Point3 origin, Vec3 direction) : origin(origin), direction(direction) {};
	Point3 origin;
	Vec3 direction;

	Vec3 at(float dist) const { return origin + direction * dist; }
};

