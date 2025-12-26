#include "Camera.hpp"
#include "Canvas.hpp"
#include "CanvasExporter.hpp"
#include "GUI.hpp"
#include "ModelLoader.hpp"
#include "PrimitiveTypes.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "SceneCreator.hpp"
#include "Settings.hpp"

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>

/*
TODO:
    - Затенение (Shading)
    - GUI MUST BE OUTSIDE MAIN! | Ok
    - GUI | Ok
    - Добавить реализацию оставшихся объектов | Ok
    - Reorganization object class hierarchy (Object, GeometricPrimitive, ...) | Ok???
    - Object rotation
    - Emission materials
    - Performce

    - Reorganization method hit

    - Antialiasing (several rays per pixel)
    - Real diffuse (cancel - too much expensive)

    - BVH (AABB) | KD-Tree

    - Make Vec3 class template class

    - Устранить искажения на бокам изображения (вроде бы называется эффектом рыбьего глаза) | Ok
*/

int main() {
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(Settings::WIDTH), static_cast<unsigned int>(Settings::HEIGHT)}),
                            Settings::APP_NAME);
    window.setFramerateLimit(Settings::FRAME_LIMIT);
    if (!ImGui::SFML::Init(window))
        return -1;

    std::shared_ptr<SFML_Canvas> canvas = std::make_unique<SFML_Canvas>(Settings::WIDTH, Settings::HEIGHT);  // std::unique_ptr<Canvas>

    Scene scene = Settings::scene_creator->create_scene();
    SceneView view = Settings::scene_creator->get_view();
    Camera camera(view.pos, view.dir, scene.world_up, Settings::FOV_Y, Settings::ASPECT, Settings::NEAR, Settings::FAR);

    std::shared_ptr<Renderer> renderer = std::make_unique<RayTracingRenderer>();

    RenderSettings render_settings = {Settings::ray_tracing_depth};
    CameraSettings camera_settings = {Settings::CAMERA_MOVEMENT_SPEED, Settings::CAMERA_ROTATION_SPEED, Settings::MAX_ZENITH_RADIANS};
    KeysState keys_state = {false};
    AppContext app = {*canvas, scene, camera, *renderer, render_settings, camera_settings};

    render_frame(app);

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
