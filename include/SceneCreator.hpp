#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "Vec3.hpp"

struct SceneView {
    Vec3 pos;
    Vec3 dir;
    Vec3 up;
};

class SceneCreator {
  public:
    virtual Scene create_scene() const = 0;
    virtual SceneView get_view() const = 0;
};

class TestScene : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        scene.background_color = sRGB::SKY_BLUE;

        // scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, -1), Color(sRGB::WHITE), 1));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, 0), Color(sRGB::RED), 1));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, 0), Color(sRGB::BLUE), 1));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 0, 0), Color(sRGB::GREEN), 1));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(-1, 0, 0), Color(sRGB::WHITE), 0.5));

        // scene.add_object(std::make_shared<Object>(std::make_shared<Plane>(Vec3(-1, 0, 0), 0), Material(Color(sRGB::GRAY), 1)));
        scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-1, 0, 0), 1), Material(Color(sRGB::WHITE), 0)));
        scene.add_object(std::make_shared<Object>(std::make_shared<Box>(Vec3(0, -1, 1.5), Vec3(-1, -2, 2.5)), Material(Color(sRGB::GREEN), 0.5)));

        // std::unique_ptr<ModelLoader> loader =
        // std::make_unique<ObjLoader>();
        // scene.add_object(std::make_shared<Object>(loader->load("D:\\Personal\\BMSTU\\5sem_2025\\bmstu-cw-cg\\models\\tree.obj"),
        // Glass(ORANGE)));
        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {-1.5, 0, 5};
        view.dir = {0, 0, -1};
        view.up = {-1, 0, 0};
        return view;
    }
};

class CornellBox : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        const float width = 4;
        
        return scene;
    };
    SceneView get_view() const override {
        SceneView view;
        view.pos = {0, 0, 0};
        view.dir = {0, 0, -1};
        view.up = {-1, 0, 0};
        return view;
    };
};
