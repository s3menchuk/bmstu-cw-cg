#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "MeshLight.hpp"
#include "ModelLoader.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "Vec3.hpp"

struct SceneView {
    Vec3 pos = {0, 0, 0};
    Vec3 dir = {0, 0, -1};
};

class SceneCreator {
  public:
    virtual Scene create_scene() const = 0;
    virtual SceneView get_view() const = 0;

    virtual ~SceneCreator() = default;
};

class SimpleSphere : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        scene.background_color = sRGB::SKY_BLUE;
        scene.world_up = {0, 1, 0};
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, -1), Color(sRGB::WHITE), 1));
        scene.add_object(std::make_shared<Object>(std::make_shared<Sphere>(Vec3(0, 0, 0), 1), Material(Color(sRGB::RED), 0.1)));
        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {0, 0, 3};
        view.dir = {0, 0, -1};
        return view;
    }
};

class SpectreSphere : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        scene.world_up = {0, 1, 0};
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
        return view;
    }
};

class CornellBox : public SceneCreator {
  public:
    static constexpr float width = 4;
    static constexpr float length = 4;
    static constexpr float height = 3;
    static constexpr float radius = 0.5;

    Scene create_scene() const override {
        Scene scene;
        scene.world_up = {0, 1, 0};
        scene.background_color = sRGB::SKY_BLUE;

        float d = 0.1;
        scene.add_light(std::make_shared<PointLight>(Vec3(d, d, -length + d), sRGB::WHITE, 5));
        scene.add_light(std::make_shared<PointLight>(Vec3(width - d, d, -length + d), sRGB::WHITE, 5));

        /*
        L - left, R - right
        B - bottom, T - top
        N - near, F - far
        */
        Vec3 LBN(0, 0, 0);
        Vec3 LTN(0, height, 0);
        Vec3 LBF(0, 0, -length);
        Vec3 LTF(0, height, -length);

        Vec3 RBN(width, 0, 0);
        Vec3 RTN(width, height, 0);
        Vec3 RBF(width, 0, -length);
        Vec3 RTF(width, height, -length);

        // Red Left Wall
        auto left_wall = std::make_shared<Quad>(LBN, LTN - LBN, LBF - LBN);
        scene.add_object(std::make_shared<Object>(left_wall, Material(sRGB::RED, 0.25)));

        // Green Right Wall
        auto right_wall = std::make_shared<Quad>(RBN, RTN - RBN, RBF - RBN);
        scene.add_object(std::make_shared<Object>(right_wall, Material(sRGB::BLUE, 0.25)));

        // White Back Wall
        auto back_wall = std::make_shared<Quad>(LBF, LTF - LBF, RBF - LBF);
        scene.add_object(std::make_shared<Object>(back_wall, Material(sRGB::WHITE, 0.5)));

        // White Floor
        auto floor = std::make_shared<Quad>(LBN, LBF - LBN, RBN - LBN);
        scene.add_object(std::make_shared<Object>(floor, Material(sRGB::GREEN, 0)));

        // White Ceiling
        auto ceiling = std::make_shared<Quad>(LTN, LTF - LTN, RTN - LTN);
        scene.add_object(std::make_shared<Object>(ceiling, Material(sRGB::WHITE, 0)));

        // Front Wall
        auto front_wall = std::make_shared<Quad>(LBN, LTN - LBN, RBN - LBN);
        scene.add_object(std::make_shared<Object>(front_wall, Material(sRGB::WHITE, 0.5)));

        // Sphere
        auto sphere = std::make_shared<Sphere>(Vec3(width / 2, height * 0.4, -length / 2), radius);
        scene.add_object(std::make_shared<Object>(sphere, Material(sRGB::WHITE, 0.5)));

        // auto light_sphere = std::make_shared<Sphere>(Vec3(width / 2, radius, -length + radius), radius + 0.1);
        // scene.add_light(std::make_shared<SphereLight>(*light_sphere, sRGB::WHITE, 0.35));

        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {width * 0.75, height / 2, -0.1};
        view.dir = {-0.25, 0, -1};
        return view;
    }
};

class Tree : public SceneCreator {
  public:
    Scene create_scene() const {
        Scene scene;
        scene.world_up = {-1, 0, 0};
        auto loader = std::make_shared<ObjLoader>();
        auto model = loader->load("models/tree.obj");
        scene.add_object(std::make_shared<Object>(model, Material(sRGB::GREEN, 0.2)));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, 1), Color(sRGB::WHITE), 1));
        return scene;
    }

    SceneView get_view() const {
        SceneView view;
        view.pos = {-0.5, -0.5, 1};
        view.dir = {0, 0, -1};
        return view;
    }
};

class UtahTeapot : public SceneCreator {
  public:
    Scene create_scene() const {
        Scene scene;
        scene.world_up = {0, -1, 0};
        auto loader = std::make_shared<ObjLoader>();
        auto model = loader->load("models/utah_teapot.obj");
        scene.add_object(std::make_shared<Object>(model, Material(sRGB::GRAY, 0.2)));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, 1), Color(sRGB::WHITE), 1));
        return scene;
    }

    SceneView get_view() const {
        SceneView view;
        view.pos = {-1.5, -0.3, -0.5};
        view.dir = {1, 0, 0};
        return view;
    }
};