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

void TracingRenderer::render(Canvas &canvas, const Scene &scene, const Camera &camera) {
    Real view_height = 2 * std::tan(camera.fov_y / 2) * camera.near;
    Real view_width = view_height * camera.aspect;

    int width = canvas.get_width();
    int height = canvas.get_height();

    omp_set_num_threads(count_threads);

#pragma omp parallel for schedule(dynamic)
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            // thread_local std::mt19937 gen;
            // std::uniform_real_distribution<Real> dist(0, 1);
            Real ndc_x = (col + 0.5) / width * 2 - 1;
            Real dx = ndc_x * view_width / 2;
            Real ndc_y = 1 - (row + 0.5) / height * 2;
            Real dy = ndc_y * view_height / 2;
            Vec3 ray_dir = (dx * camera.right + dy * camera.up + camera.dir).normalized();
            Ray3 ray(camera.pos, ray_dir);

            Color color(0, 0, 0);
            for (int i = 0; i < samples_per_pixel; ++i) {
                color += trace_ray(scene, ray, max_ray_bounces);
            }
            color *= 1.0 / samples_per_pixel;

            canvas.set_pixel(row, col, color.as_srgb());
        }
    }
}

Color RayTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, int max_ray_bounces) const {
    if (max_ray_bounces < 0) {
        return Color(0, 0, 0);
    }

    HitRecord closest_hit;
    auto closest_obj = find_closest_obj(closest_hit, scene, ray);
    if (!closest_obj) {
        return background_color;
    }
    // return closest_obj->material.color;

    Vec3 N = closest_hit.normal;
    Vec3 V = -ray.direction;

    Color diffuse_intensity;
    Color specular_intensity;

    for (const auto &light : scene.lights) {
        Real dist_to_light = light->get_distance(closest_hit.point);
        HitRecord temp_hit;
        Ray3 shadow_ray(closest_hit.point + closest_hit.normal * EPSILON, -light->get_direction(closest_hit.point));
        if (!find_closest_obj(temp_hit, scene, shadow_ray, dist_to_light)) {
            Vec3 L = -light->get_direction(closest_hit.point);
            diffuse_intensity += std::max(N.dot(L), 0.0f) * light->get_color() * light->get_intensity(closest_hit.point);

            // Phong
            const Vec3 R = 2 * N.dot(L) * N - L;
            specular_intensity +=
                std::pow(std::max(R.dot(V), 0.0f), closest_obj->material.shininess) * light->get_color() * light->get_intensity(closest_hit.point);

            // Blinn-Phong
            // Vec3 H = (L + V).normalized();
            // specular_intensity +=
            //     std::pow(std::max(N.dot(H), 0.0f), closest_obj->material.shininess) * light->get_color() * light->get_intensity(closest_hit.point);
        }
    }

    Color diffuse_total = diffuse_intensity * (1 - closest_obj->material.reflectance) * closest_obj->material.color;
    Color specular_total = specular_intensity * closest_obj->material.reflectance;

    Vec3 reflected_dir = ray.direction - 2 * N.dot(ray.direction) * N;
    Ray3 reflected_ray(closest_hit.point + reflected_dir * EPSILON, reflected_dir);
    Color reflective_color = trace_ray(scene, reflected_ray, max_ray_bounces - 1) * closest_obj->material.reflectance;

    Color final_color = diffuse_total + specular_total + reflective_color;
    return final_color;
}

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

// Iteration path tracing
// Color PathTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, int max_ray_bounces) const {
//     Ray3 r(ray);

//     Color final_color(0, 0, 0);
//     Color ray_color(1, 1, 1);
//     for (int i = 0; i < max_ray_bounces + 1; ++i) {
//         HitRecord closest_hit;
//         auto closest_obj = find_closest_obj(closest_hit, scene, r);
//         if (closest_obj) {
//             r.direction = random_hemisphere_vec(closest_hit.normal);
//             r.origin = closest_hit.point + r.direction * EPSILON;
//             Color emitted_light = closest_obj->material.emission_color * closest_obj->material.emission_strength;
//             final_color += emitted_light * ray_color;
//             ray_color *= closest_obj->material.color;
//         } else {
//             break;
//         }
//     }
//     return final_color;
// }

// Recursion path tracing
Color PathTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, int max_ray_bounces) const {
    if (max_ray_bounces < 0) {
        return Color(0, 0, 0);
    }

    HitRecord closest_hit;
    auto closest_obj = find_closest_obj(closest_hit, scene, ray);
    if (!closest_obj) {
        return Color(0, 0, 0);
    }
    Vec3 random_dir = random_hemisphere_vec(closest_hit.normal);
    Ray3 random_ray(closest_hit.point + random_dir * EPSILON, random_dir);
    Color diffuse_light = closest_obj->material.color * trace_ray(scene, random_ray, max_ray_bounces - 1) * (1 - closest_obj->material.reflectance);

    Color reflected_light(0, 0, 0);

    Vec3 V = ray.direction;
    Vec3 N = closest_hit.normal;
    Vec3 reflected_dir = V - 2 * N.dot(V) * N;
    Ray3 reflected_ray(closest_hit.point + reflected_dir * EPSILON, reflected_dir);
    reflected_light = trace_ray(scene, reflected_ray, max_ray_bounces - 1) * closest_obj->material.reflectance;

    Color emitted_light = closest_obj->material.emission_color * closest_obj->material.emission_strength;

    return diffuse_light + reflected_light + emitted_light;
}

std::shared_ptr<Object> find_closest_obj(HitRecord &closest_hit, const Scene &scene, const Ray3 &ray, Real max_dist) {
    HitRecord closest, current;
    closest.dist = max_dist;
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