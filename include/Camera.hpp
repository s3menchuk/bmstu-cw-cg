#pragma once

#include "Math.hpp"
#include "Types.hpp"
#include "Vec3.hpp"

#include <iostream>

class Camera {
  public:
	Camera(const Point3 &pos, const Vec3 &dir, T fov, T aspect, T near, T far)
		: pos(pos), dir(dir), fov(fov), aspect(aspect), near(near), far(far) {
		this->dir.normalize();

		if (this->dir == Vec3(0, 1, 0)) {
			this->up = this->dir.cross(Vec3(1, 0, 0));
		}
		Vec3 world_up = this->dir == Vec3(0, 1, 0) ? Vec3(0, 0, 1) : Vec3(0, 1, 0);
		this->up = this->dir.cross(world_up);
		this->up.normalize();

		this->right = this->dir.cross(up);

		/*if (std::abs(dir.z) >= std::abs(dir.y))
			up = Vector(-dir.z, 0, dir.x);
		else
			up = Vector(dir.y, -dir.x, 0);
		right = dir.cross(up);*/
	};

	void rotate_vertically(T radians) {
		dir.rotate(right, radians);
		up.rotate(right, radians);
	}
	void rotate_horizontally(T radians) {
		dir.rotate_x(radians);
		up.rotate_x(radians);
		right.rotate_x(radians);
	}

	T get_azimuth() const { return std::atan2(dir.z, dir.y) + std::numbers::pi / 2; }

	T get_zenith() const { return std::acos(dir.x) - std::numbers::pi / 2; }

	void set_azimuth(T radians) { rotate_horizontally(-get_azimuth() - radians); }

	void set_zenith(T radians) { rotate_vertically(get_zenith() - radians); }

	Vec3 pos;
	Vec3 dir;
	Vec3 up;
	Vec3 right;
	T fov;
	T aspect;
	T near;
	T far;
};