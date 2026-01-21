#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Types.hpp"

struct CameraSettings {
    Real camera_movement_speed;
    Real camera_rotation_speed;
    Real max_zenith_radians;
};

struct AppContext {
    SFML_Canvas &canvas;  // TODO: Not SFML_Canvas, just Canvas!
    Scene &scene;
    Camera &camera;
    Renderer &renderer;
    RenderSettings render_settings;
    CameraSettings camera_settings;
    bool need_render;
};

void draw_settings_iu(AppContext &app);

void draw_objects_ui(Scene &scene);
void draw_camera_ui(Camera &camera, CameraSettings &settings);
void draw_lights_ui();

void draw_render_ui(const AppContext &app);
bool handle_keystrokes(Camera &camera, const Scene &scene, const CameraSettings &camera_settings);
