#include "AppContext.hpp"
#include "Benchmark.hpp"
#include "Camera.hpp"
#include "Canvas.hpp"
#include "GUI.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Settings.hpp"

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <omp.h>

/*
TODO:
    - Transforms (canonical forms of primitives (sphere at (0, 0, 0) and r=1))
    - BVH (AABB) | KD-Tree
    - Make Vec3 class template class
*/

void render_frame(AppContext &app) {
    app.renderer.max_ray_bounces = app.render_settings.max_ray_bounces;
    app.renderer.count_threads = app.render_settings.count_threads;
    app.renderer.samples_per_pixel = app.render_settings.samples_per_pixel;

    auto start = std::chrono::high_resolution_clock::now();
    app.renderer.render(app.canvas, app.scene, app.camera);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " ms\n";

    app.state.need_render = false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({DefaultSettings::WIDTH, DefaultSettings::HEIGHT}), DefaultSettings::APP_NAME);
    // window.setFramerateLimit(Settings::FRAME_LIMIT);
    if (!ImGui::SFML::Init(window))
        return -1;

    RayTracingRenderer renderer;
    renderer.background_color = sRGB::SKY_BLUE;

    Scene scene = DefaultSettings::SCENE_CREATOR->create_scene();

    SceneView view = DefaultSettings::SCENE_CREATOR->get_view();
    Camera camera(view.pos, view.dir, scene.world_up, DefaultSettings::FOV_Y, DefaultSettings::ASPECT, DefaultSettings::NEAR, DefaultSettings::FAR);

    SFML_Canvas canvas(DefaultSettings::WIDTH, DefaultSettings::HEIGHT);

    AppContext app(renderer, scene, camera, canvas);

    app.camera_settings.camera_movement_speed = DefaultSettings::CAMERA_MOVEMENT_SPEED;
    app.camera_settings.camera_rotation_speed = DefaultSettings::CAMERA_ROTATION_SPEED;
    app.camera_settings.max_zenith_radians = DefaultSettings::MAX_ZENITH_RADIANS;

    app.render_settings.max_ray_bounces = DefaultSettings::MAX_RAY_BOUNCES;
    app.render_settings.count_threads = omp_get_max_threads();
    app.render_settings.samples_per_pixel = DefaultSettings::SAMPLES_PER_PIXEL;

    app.state.need_render = true;
    app.state.frame_num = 0;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        app.state.frame_num++;

        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
            if (handle_keystrokes(camera, scene, app.camera_settings))
                app.state.need_render = true;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        draw_settings_iu(app);

        if (app.state.need_render) {
            render_frame(app);
        }

        window.draw(canvas.pixels);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}