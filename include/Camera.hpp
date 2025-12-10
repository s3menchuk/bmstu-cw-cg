#pragma once

#include "Math.hpp"
#include "Types.hpp"
#include "Vec3.hpp"

#include <iostream>

class Camera {
  public:
    Camera(const Point3 &pos, const Vec3 &dir, const Vec3 &world_up, T fov_y, T aspect, T near, T far)
        : pos(pos), fov_y(fov_y), aspect(aspect), near(near), far(far) {
        this->dir = dir.normalized();
        this->right = this->dir.cross(world_up.normalized());
        this->up = this->right.cross(this->dir);

        this->fov_x = 2 * std::atan(std::tan(fov_y / 2) * aspect);
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

    void rotate_by(const Vec3 &axis, T radians) {
        dir.rotate(axis, radians);
        up.rotate(axis, radians);
        right.rotate(axis, radians);
    }

    T get_azimuth() const {
        return std::atan2(dir.z, dir.y) + std::numbers::pi / 2;
    }

    T get_zenith() const {
        return std::acos(dir.x) - std::numbers::pi / 2;
    }

    void set_azimuth(T radians) {
        rotate_horizontally(-get_azimuth() - radians);
    }

    void set_zenith(T radians) {
        rotate_vertically(get_zenith() - radians);
    }

    Vec3 pos;
    Vec3 dir;
    Vec3 up;
    Vec3 right;
    T fov_y;
    T fov_x;
    T aspect;
    T near;
    T far;
};
