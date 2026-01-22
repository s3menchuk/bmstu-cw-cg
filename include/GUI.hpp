#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Types.hpp"

void draw_settings_iu(AppContext &app);

void draw_objects_ui(Scene &scene);
void draw_camera_ui(Camera &camera, CameraSettings &settings);
void draw_lights_ui();

void draw_render_ui(const AppContext &app);
bool handle_keystrokes(Camera &camera, const Scene &scene, const CameraSettings &camera_settings);
