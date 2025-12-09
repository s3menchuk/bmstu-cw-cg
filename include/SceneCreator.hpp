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

class SimpleSphere : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        // scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-1, 0, 0), 1), Material(Color(sRGB::RED), 0)));
        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {-1.5, 0, 5};
        view.dir = {0, 0, -1};
        view.up = {1, 0, 0};
        return view;
    }
};

class SpectreSphere : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        // scene.background_color = sRGB::SKY_BLUE;

        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, 0), Color(sRGB::RED), 1));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, 0), Color(sRGB::BLUE), 1));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 0, 0), Color(sRGB::GREEN), 1));
        // scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, 0), Color(sRGB::WHITE), 0.5));

        scene.add_object(std::make_shared<Object>(std::make_shared<Plane>(Vec3(-1, 0, 0), 0), Material(Color(sRGB::WHITE), 1)));
        scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(-1, 0, 0), 1), Material(Color(sRGB::WHITE), 0)));

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
        view.up = {1, 0, 0};
        return view;
    }
};

class CornellBox : public SceneCreator {
  public:
    static constexpr float width = 4;
    static constexpr float length = 6;
    static constexpr float height = 8;

    Scene create_scene() const override {
        Scene scene;
        // scene.background_color = sRGB::SKY_BLUE;
        /*
        L - left, R - right
        B - bottom, T - top
        N - near, F - far
        */
        scene.add_light(std::make_shared<DirectionLight>(Vec3(-1, -1, -1), Color(sRGB::WHITE), 1));

        Vec3 LBN = Vec3(0, 0, 0);
        Vec3 LTN = Vec3(0, 0, height);
        Vec3 LBF = Vec3(0, length, 0);
        Vec3 LTF = Vec3(0, length, height);
        scene.add_object(std::make_shared<Object>(std::make_shared<Quad>(LBN, LTN - LBN, LBF - LBN), Material(Color(sRGB::RED), 0.2)));
        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {width / 2, 0, height / 2};
        view.dir = {0, 1, 0};
        view.up = {0, 0, 1};
        return view;
    }
};
