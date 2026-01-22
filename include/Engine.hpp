#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"


#include "memory"

class Engine {
  public:
    Engine(std::shared_ptr<Renderer> renderer, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, std::shared_ptr<Canvas> canvas)
        : renderer(renderer), scene(scene), camera(camera), canvas(canvas) {}

    void render_frame() const {
        renderer->render(*canvas, *scene, *camera);
    }

    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Canvas> canvas;
};