#include "AppContext.hpp"
#include "Camera.hpp"
#include "Canvas.hpp"
#include "GUI.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Settings.hpp"

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <omp.h>
#include <string>
#include <typeindex>
#include <typeinfo>

/*
TODO:
At first:
    - GUI
    - Transforms (canonical forms of primitives (sphere at (0, 0, 0) and r=1))
    - Shading
    - Camera limit verticle angles

    - GPU (OpenGL, Vulkan)
    - Real diffuse
    - Antialiasing (several rays per pixel)
    - Emmision materials

    - BVH (AABB) | KD-Tree

    - Make Vec3 class template class

    - Добавить реализацию оставшихся объектов | Ok
    - Устранить искажения на бокам изображения (вроде бы называется эффектом рыбьего глаза) | Ok
*/

void render_frame(const AppContext &app) {
    auto start = std::chrono::high_resolution_clock::now();
    app.renderer.render(app.canvas, app.scene, app.camera);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " ms\n";
}

int main() {
    sf::RenderWindow window(sf::VideoMode({DefaultSettings::WIDTH, DefaultSettings::HEIGHT}), DefaultSettings::APP_NAME);
    // window.setFramerateLimit(Settings::FRAME_LIMIT);
    if (!ImGui::SFML::Init(window))
        return -1;

    auto canvas = std::make_unique<SFML_Canvas>(DefaultSettings::WIDTH, DefaultSettings::HEIGHT);  // std::unique_ptr<Canvas>

    Scene scene = DefaultSettings::SCENE_CREATOR->create_scene();
    SceneView view = DefaultSettings::SCENE_CREATOR->get_view();
    Camera camera(view.pos, view.dir, scene.world_up, DefaultSettings::FOV_Y, DefaultSettings::ASPECT, DefaultSettings::NEAR, DefaultSettings::FAR);

    auto renderer = std::make_unique<RayTracingRenderer>();

    CameraSettings camera_settings = {DefaultSettings::CAMERA_MOVEMENT_SPEED, DefaultSettings::CAMERA_ROTATION_SPEED,
                                      DefaultSettings::MAX_ZENITH_RADIANS};
    AppContext app = {*canvas, scene, camera, *renderer, camera_settings, DefaultSettings::MAX_RAY_BOUNCES, true};
    app.renderer = *std::make_unique<PathTracingRenderer>();
    // omp_get_max_threads()

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
            if (handle_keystrokes(camera, scene, camera_settings))
                app.state.need_render = true;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        draw_settings_iu(app);

        if (app.state.need_render) {
            renderer->max_ray_bounces = app.render_settings.max_ray_bounces;
            renderer->count_threads = app.render_settings.count_threads;
            renderer->samples_per_pixel = app.render_settings.samples_per_pixel;
            render_frame(app);
            app.state.need_render = false;
        }

        // window.clear();
        window.draw(canvas->pixels);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

/*
auto window = std::make_unique<CreateWindow>(width, height);
if (!window)
    return -1;

while (window.is_open()) {
    handle events
    update app state
    render image
    display image
}

window.close();
*/