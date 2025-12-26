#pragma once

#include "SceneCreator.hpp"

namespace Settings {
const std::string APP_NAME = "Graphics Engine";
const size_t FRAME_LIMIT = 60;

const float ASPECT = 16.0f / 9.0f;
const size_t WIDTH = 1280;
const size_t HEIGHT = WIDTH / ASPECT;

const float FOV_Y = std::numbers::pi / 180 * 55;

const float NEAR = 1;
const float FAR = 1000;

const float MAX_ZENITH_DEGREES = 75;
const float MAX_ZENITH_RADIANS = std::numbers::pi / 2 * MAX_ZENITH_DEGREES / 90;

const float CAMERA_MOVEMENT_SPEED = 0.1;
const float CAMERA_ROTATION_SPEED = 0.15;

inline size_t max_ray_bounces = 3;

inline std::shared_ptr<SceneCreator> scene_creator = std::make_shared<UtahTeapotInCornellBox>(3, 3, 1.5, 1);
}  // namespace Settings