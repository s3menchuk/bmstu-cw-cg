#include "Renderer.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "PrimitiveTypes.hpp"
#include "Ray3.hpp"
#include "Vec3.hpp"

#include <iostream>
#include <omp.h>

bool RayTracingRenderer::is_in_shadow(const Scene &scene, const HitRecord &hit, const Light &light) const {
    Ray3 ray(hit.point, -light.get_direction(hit.point));
    // ray.origin = ray.at(EPSILON);
    ray.origin += hit.normal * EPSILON;
    const auto dist = light.get_distance(hit.point);
    HitRecord hit_record;
    for (const auto &obj : scene.objects)
        if (obj->visible && obj->hit(ray, hit_record) && hit_record.dist < dist)
            return true;
    return false;
}

std::shared_ptr<Object> RayTracingRenderer::find_closest_obj(HitRecord &closest_hit, const Scene &scene, const Ray3 &ray) const {
    HitRecord closest, current;
    closest.dist = std::numeric_limits<T>::infinity();
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

Color RayTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, size_t depth) const {
    if (depth == 0)
        return Color();

    HitRecord closest_hit;
    auto closest_obj = find_closest_obj(closest_hit, scene, ray);
    if (!closest_obj) {
        return scene.get_background_color(ray);
    }

    const Vec3 N = closest_hit.normal;
    const Vec3 V = -ray.direction;

    Color diffuse_intensity;
    Color specular_intensity;

    for (const auto &light : scene.lights) {
        if (!is_in_shadow(scene, closest_hit, *light)) {
            const Vec3 L = -light->get_direction(closest_hit.point);
            diffuse_intensity += std::max(N.dot(L), 0.0f) * light->get_color() * light->get_intensity(closest_hit.point);

            // const Vec3 R = 2 * N.dot(L) * N - L;
            Vec3 H = (L + V).normalized();
            specular_intensity += std::pow(std::max(N.dot(H), 0.0f), 100) * light->get_color() * light->get_intensity(closest_hit.point);
        }
    }

    Color diffuse_color = {1, 1, 1};

    Color diffuse_total = diffuse_intensity * (1 - closest_obj->material.reflectance) * diffuse_color * closest_obj->material.color;
    Color specular_total = specular_intensity * closest_obj->material.reflectance;

    Vec3 reflected_dir = ray.direction - 2 * N.dot(ray.direction) * N;
    Ray3 reflected_ray(closest_hit.point + reflected_dir * EPSILON, reflected_dir);
    Color reflective_color = trace_ray(scene, reflected_ray, depth - 1) * closest_obj->material.reflectance;

    Color final_color = diffuse_total + specular_total + reflective_color;
    return final_color;
}

void RayTracingRenderer::render(Canvas &canvas, const Scene &scene, const Camera &camera, const RenderSettings &settings) {
    float view_height = 2 * std::tan(camera.fov_y / 2) * camera.near;
    float view_width = view_height * camera.aspect;

    size_t width = canvas.get_width();
    size_t height = canvas.get_height();

    omp_set_num_threads(settings.count_threads);

    // #pragma omp parallel for schedule(dynamic)
    for (int row = 0; row < height; ++row) {
        float ndc_y = 1 - (row + 0.5f) / height * 2;
        float dy = ndc_y * view_height / 2;
        for (int col = 0; col < width; ++col) {
            float ndc_x = (col + 0.5f) / width * 2 - 1;
            float dx = ndc_x * view_width / 2;
            Vec3 ray_dir = (dx * camera.right + dy * camera.up + camera.dir).normalized();
            Ray3 ray(camera.pos, ray_dir);
            Color color = trace_ray(scene, ray, settings.max_ray_bounces);
            canvas.set_pixel(row, col, color.as_srgb());
        }
    }
}

Color RayTracingRenderer::calc_diffuse(const Scene &scene, const Point3 &origin, const Point3 &point, const Vec3 &normal) const {
    Ray3 ray(origin, (point - origin).normalized());
    const Vec3 N = normal;
    const Vec3 V = -ray.direction;
    HitRecord hit_record;
    hit_record.point = point;
    hit_record.normal = normal;
    hit_record.dist = (point - origin).length();
    Color diffuse_intensity;
    for (const auto &light : scene.lights) {
        if (!RayTracingRenderer::is_in_shadow(scene, hit_record, *light)) {
            const Vec3 L = -light->get_direction(hit_record.point);
            diffuse_intensity += std::max(N.dot(L), 0.0f) * light->get_color() * light->get_intensity(hit_record.point);
        }
    }
    return diffuse_intensity;
}