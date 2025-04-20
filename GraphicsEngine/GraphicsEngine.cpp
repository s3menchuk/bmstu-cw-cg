#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <numbers>

#include "Renderer.h"
#include "Canvas.h"
#include "Camera.h"
#include "Scene.h"

int main()
{
    const unsigned int WIDTH = 400;
    const float ASPECT = 1; // 16.0f / 9.0f;
    const unsigned int HEIGHT = WIDTH / ASPECT;
    const float FOV = std::numbers::pi / 12.0f * 5.0f; // std::numbers::pi * 2.0f / 3.0f;
    const float NEAR = 1;
    const float FAR = 1000;
    
    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Graphics Engine");

    auto canvas = std::make_unique<SFML_Canvas>(WIDTH, HEIGHT); // std::unique_ptr<Canvas>
    Camera camera(Vector(-5, 0, 20), Vector(0, 0, -1), FOV, ASPECT, NEAR, FAR);
    
    Scene scene;
    scene.objects.push_back(std::make_shared<Plane>(Vector(-1, 0, 0), 0, Material(GRAY, 0.2, 0.5, 0.5, 16)));
    scene.objects.push_back(std::make_shared<Sphere>(Vector(-5, 7, 0), 5, Material(RED, 0, 0.5, 0.5, 16)));
    scene.objects.push_back(std::make_shared<Sphere>(Vector(-3.5, -7, 0), 3.5, Material(GREEN, 0.3, 0.5, 0.5, 16)));
    scene.objects.push_back(std::make_shared<Sphere>(Vector(-10, 0, -20), 10, Material(BLUE, 0.6, 0.5, 0.5, 16)));
    scene.lights.push_back(std::make_shared<DirectionLight>(Vector(1, -1, -1), WHITE, 1));
    /*scene.lights.push_back(std::make_shared<DirectionLight>(Vector(1, 1, -1), BLUE, 1));*/
    //scene.lights.push_back(std::make_shared<PointLight>(Vector(-5, 0, 0), PURPLE, 1));

    std::unique_ptr<Renderer> renderer = std::make_unique<RayTracingRenderer>();

    const float SPEED = 0.1;
    const float DELTA_ANGLE = 0.1;

    bool changed = true;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { camera.pos += camera.dir * SPEED; changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { camera.pos -= camera.right * SPEED; changed = true;  }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { camera.pos -= camera.dir * SPEED; changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { camera.pos += camera.right * SPEED; changed = true; }

            // Rotation
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) { camera.rotate_vertically(DELTA_ANGLE); changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) { camera.rotate_vertically(-DELTA_ANGLE); changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) { camera.rotate_horizontally(DELTA_ANGLE); changed = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) { camera.rotate_horizontally(-DELTA_ANGLE); changed = true; }
        }

        if (changed) {
            renderer->render(*canvas, scene, camera, 3);
            window.clear();
            window.draw(canvas->pixels);
            window.display();
            changed = false;
        }
    }
}