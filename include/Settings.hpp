#pragma once

#include "Math.hpp"
#include "SceneCreator.hpp"
#include "Types.hpp"

namespace DefaultSettings {
// const std::string APP_NAME = "Graphics Engine";
const std::string APP_NAME = "CG Coursework by Semenchuk";
const int FRAME_LIMIT = 60;

const Real ASPECT = 16.0 / 9.0;
const int WIDTH = 1920;
const int HEIGHT = WIDTH / ASPECT;

const Real FOV_Y = degrees2radians(55.0);

const Real NEAR = 1;
const Real FAR = 1000;

const Real MAX_ZENITH_RADIANS = degrees2radians(75.0);

const Real CAMERA_MOVEMENT_SPEED = 0.1;
const Real CAMERA_ROTATION_SPEED = 0.15;

const int MAX_RAY_BOUNCES = 2;
const int SAMPLES_PER_PIXEL = 1;

// const auto SCENE_CREATOR = std::make_unique<UtahTeapotInCornellBox>(3, 3, 1.5, 2);
// const auto SCENE_CREATOR = std::make_unique<UtahTeapot>();
const auto SCENE_CREATOR = std::make_unique<SimpleSphere>();
}  // namespace DefaultSettings