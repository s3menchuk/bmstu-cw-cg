#pragma once

#include "Vector.h"

class Ray
{
public:
	Ray(Vector origin, Vector direction) : origin(origin), direction(direction) {};
	Vector origin;
	Vector direction;

	Vector at(float dist) const { return origin + direction * dist; }
};

