#pragma once

#include "Math.hpp"
#include "Types.hpp"
#include "Vec.hpp"

#include <iostream>

class Camera {
  public:
    Camera(Point3 pos, Vec3 dir, Vec3 world_up, Real fov_y, Real aspect, Real near, Real far)
        : pos(pos), fov_y(fov_y), aspect(aspect), near(near), far(far), world_up(world_up) {
        this->dir = dir.normalized();
        this->right = this->dir.cross(world_up.normalized());
        this->up = this->right.cross(this->dir);

        this->fov_x = 2 * std::atan(std::tan(fov_y / 2) * aspect);
    };

    void rotate_vertically(Real radians) {
        dir.rotate(right, radians);
        up.rotate(right, radians);
    }
    void rotate_horizontally(Real radians) {
        dir.rotate(world_up, radians);
        up.rotate(world_up, radians);
        right.rotate(world_up, radians);
    }

    void rotate_by(Vec3 axis, Real radians) {
        dir.rotate(axis, radians);
        up.rotate(axis, radians);
        right.rotate(axis, radians);
    }

    Real get_azimuth() const {
        return std::atan2(dir.x, -dir.z);
    }

    Real get_zenith() const {
        return std::asin(dir.y);
    }

    void set_azimuth(Real radians) {
        rotate_horizontally(get_azimuth() - radians);
    }

    void set_zenith(Real radians) {
        rotate_vertically(radians - get_zenith());
    }

    Vec3 pos;

    Vec3 dir;
    Vec3 up;
    Vec3 right;

    Real fov_y;
    Real fov_x;
    Real aspect;
    Real near;
    Real far;

  private:
    Vec3 world_up;
};
