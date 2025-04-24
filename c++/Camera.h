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

		if (this->dir == Vector(0, 1, 0)) {
			this->up = this->dir.cross(Vector(1, 0, 0));
		}
		Vector world_up = this->dir == Vector(0, 1, 0) ? Vector(0, 0, 1) : Vector(0, 1, 0);
		this->up = this->dir.cross(world_up);
		this->up.normalize();

		this->right = this->dir.cross(up);

		/*if (std::abs(dir.z) >= std::abs(dir.y))
			up = Vector(-dir.z, 0, dir.x);
		else
			up = Vector(dir.y, -dir.x, 0);
		right = dir.cross(up);*/
	};

	void rotate_vertically(float angle) {
		dir.rotate(right, angle);
		up.rotate(right, angle);
	}
	void rotate_horizontally(float angle) {
		dir.rotate_x(angle);
		up.rotate_x(angle);
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