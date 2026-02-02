#pragma once

#include "Math.hpp"
#include "Types.hpp"
#include "Vec.hpp"

#include <string>

namespace DefaultSettings {
const std::string APP_NAME = "Graphics Engine";
const int FRAME_LIMIT = 60;

const Real ASPECT = 16.0f / 9.0f;
const int WIDTH = 1280;
const int HEIGHT = WIDTH / ASPECT;

const Vec3 CAMERA_POS(0, 0.35, 1);
const Vec3 CAMERA_DIR(0, 0, -1);
const Vec3 WORLD_UP(0, 1, 0);

const Real FOV_Y = degrees2radians(55.0f);

const Real CAMERA_NEAR = 1;
const Real CAMERA_FAR = 1000;

const Real MAX_ZENITH_RADIANS = degrees2radians(75.0f);

const Real CAMERA_MOVEMENT_SPEED = 0.01;
const Real CAMERA_ROTATION_SPEED = 0.1;

const Real MOUSE_SENSITIVITY = 0.025f;

const int max_ray_bounces = 2;
const int samples_per_pixel = 1;
}  // namespace DefaultSettings