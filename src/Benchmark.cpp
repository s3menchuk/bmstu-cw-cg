#include "Benchmark.hpp"

#include "Camera.hpp"
#include "Canvas.hpp"
#include "CanvasExporter.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "SceneCreator.hpp"
#include "Settings.hpp"

#include <cstddef>
#include <vector>

void benchmark() {
    auto renderer = std::make_unique<RayTracingRenderer>();
    auto canvas = std::make_shared<SimpleCanvas>(DefaultSettings::WIDTH, DefaultSettings::HEIGHT);

    renderer->max_ray_bounces = 3;

    auto exporter = std::make_shared<CanvasPPMBinaryExporter>();

    for (auto count_threads : {1, 2, 4, 8, 12, 16, 20, 24}) {
        renderer->count_threads = count_threads;
        for (auto utah_res : {1, 2, 3, 4}) {
            std::shared_ptr<SceneCreator> scene_creator = std::make_shared<UtahTeapotInCornellBox>(3, 3, 1.5, utah_res);
            Scene scene = scene_creator->create_scene();
            SceneView view = scene_creator->get_view();
            Camera camera(view.pos, view.dir, scene.world_up, DefaultSettings::FOV_Y, DefaultSettings::ASPECT, DefaultSettings::NEAR,
                          DefaultSettings::FAR);

            auto start = std::chrono::high_resolution_clock::now();

            renderer->render(*canvas, scene, camera);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cout << "count_threads = " << count_threads << ", utah_res = " << utah_res << " | time = " << elapsed.count() << " ms\n";

            // exporter->save(*canvas, std::format("T{}-R{}.ppm", count_threads, utah_res));
        }
    }
}