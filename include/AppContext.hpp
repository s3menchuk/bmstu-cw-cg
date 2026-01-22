#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Renderer.hpp"
#include "Types.hpp"

struct CameraSettings {
    Real camera_movement_speed;
    Real camera_rotation_speed;
    Real max_zenith_radians;
};

struct RenderSettings {
    int max_ray_bounces;
    int count_threads;
    int samples_per_pixel;
};

struct State {
    bool need_render;
    int frame_num;
};

struct AppContext {
    SFML_Canvas &canvas;  // TODO: Not SFML_Canvas, just Canvas!
    Scene &scene;
    Camera &camera;
    Renderer &renderer;
    CameraSettings camera_settings;
    RenderSettings render_settings;
    State state;
};