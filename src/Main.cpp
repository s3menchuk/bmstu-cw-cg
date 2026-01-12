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

int main() {
    sf::RenderWindow window(sf::VideoMode({Settings::WIDTH, Settings::HEIGHT}), Settings::APP_NAME);
    // window.setFramerateLimit(Settings::FRAME_LIMIT);
    if (!ImGui::SFML::Init(window))
        return -1;

    std::shared_ptr<SFML_Canvas> canvas = std::make_unique<SFML_Canvas>(Settings::WIDTH, Settings::HEIGHT);  // std::unique_ptr<Canvas>

    Scene scene = Settings::scene_creator->create_scene();
    SceneView view = Settings::scene_creator->get_view();
    Camera camera(view.pos, view.dir, scene.world_up, Settings::FOV_Y, Settings::ASPECT, Settings::NEAR, Settings::FAR);

    std::shared_ptr<Renderer> renderer = std::make_unique<RayTracingRenderer>();

    RenderSettings render_settings = {Settings::max_ray_bounces, omp_get_max_threads(), Settings::samples_per_pixel};
    CameraSettings camera_settings = {Settings::CAMERA_MOVEMENT_SPEED, Settings::CAMERA_ROTATION_SPEED, Settings::MAX_ZENITH_RADIANS};
    AppContext app = {*canvas, scene, camera, *renderer, render_settings, camera_settings, true};

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
            process_key_input(app);
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        draw_settings_iu(app);

        window.clear();
        window.draw(canvas->pixels);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
