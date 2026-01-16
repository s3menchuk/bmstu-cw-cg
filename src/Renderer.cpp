#include "Renderer.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "PrimitiveTypes.hpp"
#include "Ray.hpp"
#include "Types.hpp"
#include "Vec.hpp"

#include <iostream>
#include <omp.h>
#include <random>

Vec3 random_uniform_vec() {
    thread_local std::mt19937 gen{std::random_device{}()};
    std::uniform_real_distribution<Real> dist(-1, 1);
    Vec3 dir;
    do {
        dir = {dist(gen), dist(gen), dist(gen)};
    } while (dir.dot(dir) > 1);
    return dir.normalized();
}

Vec3 random_hemisphere_vec(Vec3 normal) {
    Vec3 dir = random_uniform_vec();
    if (dir.dot(normal) < 0)
        dir = -dir;
    return dir;
}

std::shared_ptr<Object> RayTracingRenderer::find_closest_obj(HitRecord &closest_hit, const Scene &scene, Ray3 ray) const {
    HitRecord closest, current;
    closest.dist = std::numeric_limits<Real>::infinity();
    std::shared_ptr<Object> closest_obj = nullptr;
    for (const auto &obj : scene.objects) {
        if (obj->visible && obj->hit(ray, current) && current.dist < closest.dist) {
            closest = current;
            closest_obj = obj;
        }
    }
    if (closest_obj) {
        closest_hit = closest;
    }
    return closest_obj;
}

// Iteration trace ray
// Color RayTracingRenderer::trace_ray(const Scene &scene, Ray3 ray, int depth) const {
//     Color final_color(0, 0, 0);
//     Color ray_color(1, 1, 1);
//     for (int i = 0; i < depth; ++i) {
//         HitRecord closest_hit;
//         auto closest_obj = find_closest_obj(closest_hit, scene, ray);
//         if (closest_obj) {
//             ray.direction = random_hemisphere_vec(closest_hit.normal);
//             ray.origin = closest_hit.point + ray.direction * EPSILON;
//             Color emitted_light = closest_obj->material.emission_color * closest_obj->material.emission_strength;
//             final_color += emitted_light * ray_color;
//             ray_color *= closest_obj->material.base_color;
//         } else {
//             break;
//         }
//     }
//     return final_color;
// }

// Recursion trace ray
Color RayTracingRenderer::trace_ray(const Scene &scene, Ray3 ray, int depth) const {
    if (depth <= 0) {
        return Color(0, 0, 0);
    }
    HitRecord closest_hit;
    auto closest_obj = find_closest_obj(closest_hit, scene, ray);
    if (!closest_obj) {
        return Color(0, 0, 0);
    }
    Vec3 random_dir = random_hemisphere_vec(closest_hit.normal);
    Ray3 random_ray(closest_hit.point + random_dir * EPSILON, random_dir);
    Color diffuse_light = closest_obj->material.base_color * trace_ray(scene, random_ray, depth - 1) * (1 - closest_obj->material.metallic);

    Vec3 V = ray.direction;
    Vec3 N = closest_hit.normal;
    Vec3 reflected_dir = V - 2 * N.dot(V) * N;
    Ray3 reflected_ray(closest_hit.point + reflected_dir * EPSILON, reflected_dir);
    Color reflected_light = trace_ray(scene, reflected_ray, depth - 1) * closest_obj->material.metallic;

    Color emitted_light = closest_obj->material.emission_color * closest_obj->material.emission_strength;

    return diffuse_light + reflected_light + emitted_light;
}

void RayTracingRenderer::render(Canvas &canvas, const Scene &scene, const Camera &camera, const RenderSettings &settings) {
    Real view_height = 2 * std::tan(camera.fov_y / 2) * camera.near;
    Real view_width = view_height * camera.aspect;

    int width = canvas.get_width();
    int height = canvas.get_height();

    omp_set_num_threads(settings.count_threads);

#pragma omp parallel for schedule(dynamic)
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            thread_local std::mt19937 gen;
            std::uniform_real_distribution<Real> dist(0, 1);
            Real ndc_x = (col + dist(gen)) / width * 2 - 1;
            Real dx = ndc_x * view_width / 2;
            Real ndc_y = 1 - (row + dist(gen)) / height * 2;
            Real dy = ndc_y * view_height / 2;

            Vec3 ray_dir = (dx * camera.right + dy * camera.up + camera.dir).normalized();
            Ray3 ray(camera.pos, ray_dir);

            Color color(0, 0, 0);
            for (int i = 0; i < settings.samples_per_pixel; ++i) {
                color += trace_ray(scene, ray, settings.max_ray_bounces);
            }
            color *= 1.0 / settings.samples_per_pixel;

            canvas.set_pixel(row, col, color);
        }
    }
}

void accumulate_frame(Canvas &target, const Canvas &source, int frame_num) {
    int width = target.get_width();
    int height = target.get_height();

#pragma omp parallel for schedule(dynamic)
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            Color prev_color = target.get_pixel(row, col);
            Color curr_color = source.get_pixel(row, col);
            Color updt_color = (prev_color * (frame_num - 1) + curr_color) / frame_num;
            target.set_pixel(row, col, updt_color);
        }
    }
}