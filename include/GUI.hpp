#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"

struct CameraSettings {
    float camera_movement_speed;
    float camera_rotation_speed;
    float max_zenith_radians;
};

struct RenderSettings {
    size_t ray_tracing_depth;
};

struct KeysState {
    bool is_key_pressed;
};

struct AppContext {
    SFML_Canvas &canvas;  // TODO: Not SFML_Canvas, just Canvas!
    Scene &scene;
    Camera &camera;
    Renderer &renderer;
    RenderSettings render_settings;
    CameraSettings camera_settings;
    KeysState keys_state;
};

void draw_settings_iu(AppContext &app);

void render_frame(const AppContext &app);

void draw_objects_ui(Scene &scene);
void draw_camera_ui(Camera &camera, CameraSettings &settings);
void draw_lights_ui();

void draw_render_ui(const AppContext &app);
void process_key_input(AppContext &app);
