#pragma once

#include "Vector.h"
#include <iostream>

class Camera
{
public:
	Camera(const Vector &pos, const Vector &dir, float fov, float aspect, float near, float far) :
		pos(pos), dir(dir), fov(fov), aspect(aspect), near(near), far(far)
	{
		this->dir.normalize();
		if (std::abs(dir.z) >= std::abs(dir.y))
			up = Vector(-dir.z, 0, dir.x);
		else
			up = Vector(dir.y, -dir.x, 0);
		right = dir.cross(up);
	};

	void rotate_vertically(float angle) {
		dir.rotate_y(angle);
		up.rotate_y(angle);
	}
	void rotate_horizontally(float angle) {
		dir.rotate_x(angle);
		right.rotate_x(angle);
	}

	Vector pos;
	Vector dir;
	Vector up;
	Vector right;
	float fov;
	float aspect;
	float near;
	float far;
};