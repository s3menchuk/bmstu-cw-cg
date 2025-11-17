#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <memory>
#include <numbers>
#include <array>

#include "Renderer.hpp"
#include "Canvas.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "math.hpp"

/*
TODO:
    - GUI
    - Добавить реализацию оставшихся объектов | Ok
    - Emission materials
    - Object rotation
    - Ambient light source
    - AABB
    - Performce
    - Reorganization object class hierarchy (Object, GeometricPrimitive, ...)

    - Antialiasing (several rays per pixel)
    - Real diffuse (cancel - too much expensive)
*/

int main()
{
    const unsigned int WIDTH = 400;
    const float ASPECT = 1; // 16.0f / 9.0f;
    const unsigned int HEIGHT = WIDTH / ASPECT;
    const float FOV = std::numbers::pi / 2.0f; // std::numbers::pi * 2.0f / 3.0f;
    const float NEAR = 1;
    const float FAR = 1000;
    
    const float MAX_ZENITH_DEGREES = 75;
    const float MAX_ZENITH_RADIANS = std::numbers::pi / 2 * MAX_ZENITH_DEGREES / 90;

    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Graphics Engine");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    auto canvas = std::make_unique<SFML_Canvas>(WIDTH, HEIGHT); // std::unique_ptr<Canvas>
    Camera camera(Vec3(-7, 5, 13), Vec3(0, 0, -1), FOV, ASPECT, NEAR, FAR);
    
    Scene scene;
    scene.ambient_color = WHITE;
    scene.ambient_intensity = 0.2;
    
    scene.add_object(std::make_shared<Object>(std::make_shared<Plane>(Vec3(-1, 0, 0), 0), GlossyPlastic(GRAY)));
    scene.add_object(std::make_shared<Object>(std::make_shared<Box>(Vec3(-5, 0, 0), Vec3(5, 5, 5)), GlossyPlastic(RED)));
    scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-5, 10, 0), 5), Metal(BLUE)));
    scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-10, 7, 8), 2), Metal(PURPLE)));
    scene.add_object(std::make_shared<Object>(std::make_shared<RightPrism>(Vec3(-8, 0, 0), 5, 6, 16), Glass(GREEN)));
    scene.add_object(std::make_shared<Object>(std::make_shared<RightPyramid>(Vec3(0, 10, 8), 5, 8, 3), MattePlastic(ORANGE)));

    scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, 1), WHITE, 0.5));

    auto renderer = std::make_unique<RayTracingRenderer>();

    const float CAMERA_MOVEMENT_SPEED = 0.5;
    const float CAMERA_ROTATION_SPEED = 0.15;
    
    float azimuth = radians2degrees(camera.get_azimuth());
    float zenith = radians2degrees(camera.get_zenith());

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        bool is_key_pressed = false;
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();

            // Movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { camera.pos += camera.dir * CAMERA_MOVEMENT_SPEED; is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { camera.pos -= camera.right * CAMERA_MOVEMENT_SPEED; is_key_pressed = true;  }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { camera.pos -= camera.dir * CAMERA_MOVEMENT_SPEED; is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { camera.pos += camera.right * CAMERA_MOVEMENT_SPEED; is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::PageUp)) { camera.pos -= camera.up * CAMERA_MOVEMENT_SPEED; is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::PageDown)) { camera.pos += camera.up * CAMERA_MOVEMENT_SPEED; is_key_pressed = true; }

            // Rotation
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && camera.get_zenith() <= MAX_ZENITH_RADIANS) { camera.rotate_vertically(-CAMERA_ROTATION_SPEED); is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && camera.get_zenith() >= -MAX_ZENITH_RADIANS) { camera.rotate_vertically(CAMERA_ROTATION_SPEED); is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) { camera.rotate_horizontally(CAMERA_ROTATION_SPEED); is_key_pressed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) { camera.rotate_horizontally(-CAMERA_ROTATION_SPEED); is_key_pressed = true; }
            
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Settings");

        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::SliderFloat("x", &camera.pos.x, -25, 25);
            ImGui::SliderFloat("y", &camera.pos.y, -25, 25);
            ImGui::SliderFloat("z", &camera.pos.z, -25, 25);
            if (ImGui::SliderFloat("azimuth", &azimuth, -180, 180))
                camera.set_azimuth(degrees2radians(azimuth));
            if (ImGui::SliderFloat("zenith", &zenith, -MAX_ZENITH_DEGREES, MAX_ZENITH_DEGREES))
                camera.set_zenith(degrees2radians(zenith));
        }
        
        if (ImGui::CollapsingHeader("Objects")) {
            for (size_t i = 0; i < scene.objects.size();) {
                auto& obj = scene.objects[i];
                ImGui::PushID(&obj);
                
                if (ImGui::TreeNode(typeid(*(obj->operator->())).name() + 6)) {
                    // Position
                    std::array<float, 3> pos((*obj)->get_pos());
                    if (ImGui::DragFloat3("", &pos[0]))
                        (*obj)->set_pos(pos);

                    // Rotation
                    
                    // Diffuse color
                    std::array<float, 3> float_rgb = obj->material.diffuse.as_float_rgb();
                    ImGui::ColorEdit3("", &float_rgb[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                    Color new_color(float_rgb);
                    obj->material.diffuse = new_color;
                    ImGui::SameLine();

                    // Delete button
                    if (ImGui::Button("Delete"))
                        scene.objects.erase(scene.objects.begin() + i);
                    else
                        ++i;

                    ImGui::TreePop();
                }
                else
                    ++i;
                ImGui::PopID();
            }
            ImGui::Button("Add object");
        }

        if (ImGui::CollapsingHeader("Lights")) {}

        if (ImGui::Button("Render") || is_key_pressed)
            renderer->render(*canvas, scene, camera, 2);

        ImGui::End();

        ImGui::ShowDemoWindow();
        
        window.clear();
        window.draw(canvas->pixels);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}