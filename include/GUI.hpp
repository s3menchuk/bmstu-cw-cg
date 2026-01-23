#pragma once

#include "AppContext.hpp"
#include "Camera.hpp"
#include "Scene.hpp"

void draw_settings_iu(AppContext &app);

void draw_objects_ui(Scene &scene);
void draw_camera_ui(Camera &camera, CameraSettings &settings);
void draw_lights_ui(Scene &scene);

void draw_render_ui(const AppContext &app);
bool handle_keystrokes(Camera &camera, const Scene &scene, const CameraSettings &camera_settings);
