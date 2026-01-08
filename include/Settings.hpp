#pragma once

#include "Math.hpp"
#include "SceneCreator.hpp"
#include "Types.hpp"

namespace Settings {
const std::string APP_NAME = "Graphics Engine";
const int FRAME_LIMIT = 60;

const Real ASPECT = 16.0f / 9.0f;
const int WIDTH = 600;
const int HEIGHT = WIDTH / ASPECT;

const Real FOV_Y = degrees2radians(55.0f);

const Real NEAR = 1;
const Real FAR = 1000;

const Real MAX_ZENITH_RADIANS = degrees2radians(75.0f);

const Real CAMERA_MOVEMENT_SPEED = 0.1;
const Real CAMERA_ROTATION_SPEED = 0.15;

const int max_ray_bounces = 3;

const auto scene_creator = std::make_unique<UtahTeapotInCornellBox>(3, 3, 1.5, 2);
// const auto scene_creator = std::make_unique<UtahTeapot>();
// const auto scene_creator = std::make_unique<SimpleSphere>();
}  // namespace Settings