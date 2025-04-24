#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <memory>
#include <numbers>

#include "Renderer.h"
#include "Canvas.h"
#include "Camera.h"
#include "Scene.h"

/*
TODO:
    - GUI
    - Добавить реализацию оставшихся объектов
    - Emission materials
    - Object rotation
    - Ambient light source
    - AABB
    - Performce
    - Reorganization object class hierarchy (Object, GeometricPrimitive, ...)

    - Antialiasing (several rays per pixel)
    - Real diffuse
*/

int main()
{
    const unsigned int WIDTH = 400;
    const float ASPECT = 1; // 16.0f / 9.0f;
    const unsigned int HEIGHT = WIDTH / ASPECT;
    const float FOV = std::numbers::pi / 2.0f; // std::numbers::pi * 2.0f / 3.0f;
    const float NEAR = 1;
    const float FAR = 1000;
    
    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Graphics Engine");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    auto canvas = std::make_unique<SFML_Canvas>(WIDTH, HEIGHT); // std::unique_ptr<Canvas>
    Camera camera(Vector(-4, 7, 9), Vector(0, 0, -1), FOV, ASPECT, NEAR, FAR);
    
    Scene scene;
    scene.ambient_color = WHITE;
    scene.ambient_intensity = 0.2;
    
    scene.add_object(std::make_shared<Plane>(Vector(-1, 0, 0), 0, GlossyPlastic(GRAY)));
    scene.add_object(std::make_shared<Box>(Vector(-5, 0, 0), Vector(5, 5, 5), Glass(RED)));
    scene.add_object(std::make_shared<Sphere>(Vector(-5, 10, 0), 5, Metal(BLUE)));

    /*scene.add_light(std::make_shared<DirectionLight>(Vector(1, -1, -1), GREEN, 0.2));
    scene.add_light(std::make_shared<DirectionLight>(Vector(1, 1, -1), YELLOW, 0.2));*/

    auto renderer = std::make_unique<RayTracingRenderer>();

    const float CAMERA_MOVEMENT_SPEED = 0.5;
    const float CAMERA_ROTATION_SPEED = 0.15;

    bool changed = true;


    sf::Clock deltaClock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { camera.pos += camera.dir * CAMERA_MOVEMENT_SPEED; changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { camera.pos -= camera.right * CAMERA_MOVEMENT_SPEED; changed = true;  }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { camera.pos -= camera.dir * CAMERA_MOVEMENT_SPEED; changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { camera.pos += camera.right * CAMERA_MOVEMENT_SPEED; changed = true; }

            // Rotation
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) { camera.rotate_vertically(-CAMERA_ROTATION_SPEED); changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) { camera.rotate_vertically(CAMERA_ROTATION_SPEED); changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) { camera.rotate_horizontally(CAMERA_ROTATION_SPEED); changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) { camera.rotate_horizontally(-CAMERA_ROTATION_SPEED); changed = true; }
        }

        if (changed) {
            ImGui::SFML::Update(window, deltaClock.restart());

            ImGui::Text("Hello, world %d", 123);
            /*if (ImGui::Button("Save"))
                MySaveFunction();
            ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);*/

            renderer->render(*canvas, scene, camera, 2);
            window.clear();
            window.draw(canvas->pixels);
            ImGui::SFML::Render(window);
            window.display();
            changed = false;
        }
    }

    ImGui::SFML::Shutdown();
}