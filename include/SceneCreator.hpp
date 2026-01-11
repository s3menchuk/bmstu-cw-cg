#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "MeshLight.hpp"
#include "ModelLoader.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "Vec.hpp"

#include <format>

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
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, -1), Color(sRGB::WHITE), 10));
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

class SimplePrism : public SceneCreator {
  public:
    Scene create_scene() const override {
        Scene scene;
        scene.background_color = sRGB::SKY_BLUE;
        scene.world_up = {0, 1, 0};
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, 1, -1), Color(sRGB::WHITE), 10));
        scene.add_object(std::make_shared<Object>(std::make_shared<RightPrism>(Vec3(0, 0, 0), 2, 5, 6), Material(Color(sRGB::RED), 0.1)));
        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {0, 0, 6};
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
    CornellBox(float width, float length, float height) : width(width), length(length), height(height) {}

    // Box sizes
    float width;
    float length;
    float height;

    Scene create_scene() const override {
        Scene scene;
        scene.world_up = {0, 1, 0};

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

        // Box sides
        std::shared_ptr<Object> left_wall, right_wall, back_wall, floor, ceiling, front_wall;

        // Left Wall
        auto left_quad = std::make_shared<Quad>(LBN, LTN - LBN, LBF - LBN);
        left_wall = std::make_shared<Object>(left_quad, Material(sRGB::RED, 0));
        scene.add_object(left_wall);

        // Right Wall
        auto right_quad = std::make_shared<Quad>(RBN, RTN - RBN, RBF - RBN);
        right_wall = std::make_shared<Object>(right_quad, Material(sRGB::GREEN, 0));
        scene.add_object(right_wall);

        // Back Wall
        auto back_quad = std::make_shared<Quad>(LBF, LTF - LBF, RBF - LBF);
        back_wall = std::make_shared<Object>(back_quad, Material(sRGB::WHITE, 0));
        scene.add_object(back_wall);

        // Floor
        auto floor_quad = std::make_shared<Quad>(LBN, LBF - LBN, RBN - LBN);
        floor = std::make_shared<Object>(floor_quad, Material(sRGB::WHITE, 0));
        scene.add_object(floor);

        // Ceiling
        auto ceiling_quad = std::make_shared<Quad>(LTN, LTF - LTN, RTN - LTN);
        ceiling = std::make_shared<Object>(ceiling_quad, Material(sRGB::WHITE, 0));
        scene.add_object(ceiling);

        // Front Wall
        auto front_quad = std::make_shared<Quad>(LBN, LTN - LBN, RBN - LBN);
        front_wall = std::make_shared<Object>(front_quad, Material(sRGB::WHITE, 0));
        // scene.add_object(front_wall);

        auto quad = std::make_shared<Quad>(LTN * 0.98f + (RTF - LTN) * 0.4f, (LTF - LTN) * 0.2f, (RTN - LTN) * 0.2f);
        // scene.add_object(std::make_shared<Object>(quad, Material(sRGB::WHITE, 0.1)));
        scene.add_light(std::make_shared<QuadLight>(*quad, Color(1, 1, 1), 0.5));

        // Light source
        // auto light_quad = std::make_shared<Quad>(LTN * 0.99f + (RTF - LTN) * 0.4f, (LTF - LTN) * 0.2f, (RTN - LTN) * 0.2f);
        // scene.add_light(std::make_shared<QuadLight>(*light_quad, sRGB::WHITE, 0.25));
        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {width * 0.5f, height * 0.5f, 0.5};
        view.dir = {0, 0, -1};
        return view;
    }
};

class MirrorCornellBox : public SceneCreator {
  public:
    static constexpr float width = 4;
    static constexpr float length = 4;
    static constexpr float height = 3;
    static constexpr float radius = 0.5;

    Scene create_scene() const override {
        Scene scene;
        scene.world_up = {0, 1, 0};

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

        // Left Wall
        auto left_wall = std::make_shared<Quad>(LBN, LTN - LBN, LBF - LBN);
        scene.add_object(std::make_shared<Object>(left_wall, Material(sRGB::RED, 0.5)));

        // Right Wall
        auto right_wall = std::make_shared<Quad>(RBN, RTN - RBN, RBF - RBN);
        scene.add_object(std::make_shared<Object>(right_wall, Material(sRGB::BLUE, 0.5)));

        // Back Wall
        auto back_wall = std::make_shared<Quad>(LBF, LTF - LBF, RBF - LBF);
        scene.add_object(std::make_shared<Object>(back_wall, Material(sRGB::WHITE, 1)));

        // Floor
        auto floor = std::make_shared<Quad>(LBN, LBF - LBN, RBN - LBN);
        scene.add_object(std::make_shared<Object>(floor, Material(sRGB::GREEN, 0.5)));

        // Ceiling
        auto ceiling = std::make_shared<Quad>(LTN, LTF - LTN, RTN - LTN);
        scene.add_object(std::make_shared<Object>(ceiling, Material(sRGB::WHITE, 1)));

        // Front Wall
        auto front_wall = std::make_shared<Quad>(LBN, LTN - LBN, RBN - LBN);
        scene.add_object(std::make_shared<Object>(front_wall, Material(sRGB::WHITE, 1)));

        // Sphere
        auto sphere = std::make_shared<Sphere>(Vec3(width / 2, height / 2, -length / 2), radius);
        scene.add_object(std::make_shared<Object>(sphere, Material(sRGB::WHITE, 0.75)));

        auto light_sphere = std::make_shared<Sphere>(sphere->center, sphere->get_radius() + 0.1);
        scene.add_light(std::make_shared<SphereLight>(*light_sphere, sRGB::WHITE, 0.4));

        return scene;
    }

    SceneView get_view() const override {
        SceneView view;
        view.pos = {width * 0.8, height * 0.5, -0.1};
        view.dir = {-0.35, 0, -1};
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
        scene.world_up = {0, 1, 0};
        auto loader = std::make_shared<ObjLoader>();
        auto model = loader->load("assets/models/utah_teapot-res2.obj");
        scene.add_object(std::make_shared<Object>(model, Material(sRGB::ORANGE, 0)));
        scene.add_light(std::make_shared<DirectionLight>(Vec3(1, -1, -1), Color(sRGB::WHITE), 1));
        return scene;
    }

    SceneView get_view() const {
        SceneView view;
        view.pos = {0, 1.5, 5};
        view.dir = {0, 0, -1};
        return view;
    }
};

class UtahTeapotInCornellBox : public SceneCreator {
  public:
    UtahTeapotInCornellBox(float width, float length, float height, size_t utah_res)
        : width(width), length(length), height(height), scene_creator(width, length, height), utah_res(utah_res) {}

    Scene create_scene() const {
        Scene scene = scene_creator.create_scene();
        scene.background_color = sRGB::BLACK;

        auto loader = std::make_shared<ObjLoader>();

        auto model = loader->load(std::format("assets/models/utah_teapot-res{}.obj", utah_res));
        model = std::make_shared<Rotate>(model, scene.world_up, std::numbers::pi);
        model = std::make_shared<Scale>(model, Vec3(0.35, 0.35, 0.35));
        model = std::make_shared<Translate>(model, Vec3(width / 2, 0, -length * 0.7));
        scene.add_object(std::make_shared<Object>(model, Material(sRGB::BLUE, 0.25)));
        return scene;
    }

    SceneView get_view() const {
        return scene_creator.get_view();
    }

  private:
    CornellBox scene_creator;
    float width, length, height;
    size_t utah_res;
};