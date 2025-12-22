#include "Renderer.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "Ray3.hpp"
#include "Vec3.hpp"

#include <iostream>

bool RayTracingRenderer::is_in_shadow(const Scene &scene, const Point3 &point, const Light &light) const {
    Ray3 ray(point, -light.get_direction(point));
    ray.origin = ray.at(EPSILON);
    const auto dist = light.get_distance(point);
    HitRecord hit_record;
    for (const auto &obj : scene.get_visible_objects())
        if ((*obj)->hit(ray, hit_record) && hit_record.dist <= dist)
            return true;
    return false;
}

std::shared_ptr<Object> RayTracingRenderer::find_closest_obj(const Scene &scene, const Ray3 &ray) const {
    std::shared_ptr<Object> closest_obj = nullptr;
    float closest_dist = FLT_MAX;
    HitRecord hit_record;
    for (const auto &obj : scene.get_visible_objects()) {
        if ((*obj)->hit(ray, hit_record) && hit_record.dist < closest_dist) {
            closest_dist = hit_record.dist;
            closest_obj = obj;
        }
    }
    return closest_obj;
}

Color RayTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, size_t depth) const {
    if (depth == 0)
        return Color();

    auto closest_obj = find_closest_obj(scene, ray);
    if (!closest_obj) {
        return scene.get_background_color(ray);
    }

    HitRecord hit_record;
    (*closest_obj)->hit(ray, hit_record);

    const Vec3 N = hit_record.normal;
    const Vec3 V = -ray.direction;

    Color diffuse_intensity;
    Color specular_intensity;

    for (const auto &light : scene.lights) {
        if (!is_in_shadow(scene, hit_record.point, *light)) {
            Vec3 L = -light->get_direction(hit_record.point);
            diffuse_intensity += std::max(N.dot(L), 0.0f) * light->get_color() * light->get_intensity(hit_record.point);

            Vec3 R = 2 * N.dot(L) * N - L;
            // Vec3 H = (L + V).normalized();
            specular_intensity += std::pow(std::max(R.dot(V), 0.0f), 200) * light->get_color() * light->get_intensity(hit_record.point);
        }
    }

    Color diffuse_total = diffuse_intensity * (1 - closest_obj->material.reflectance) * closest_obj->material.color;
    Color specular_total = specular_intensity * closest_obj->material.reflectance;

    Vec3 reflected_dir = ray.direction - 2 * N.dot(ray.direction) * N;
    Ray3 reflected_ray(hit_record.point + reflected_dir * EPSILON, reflected_dir);
    Color reflective_color = trace_ray(scene, reflected_ray, depth - 1) * closest_obj->material.reflectance;

    Color final_color = diffuse_total + specular_total + reflective_color;
    return final_color;
}

void RayTracingRenderer::render(Canvas &canvas, const Scene &scene, const Camera &camera, size_t depth) {
    const float view_height = 2 * std::tan(camera.fov_y / 2) * camera.near;
    const float view_width = view_height * camera.aspect;

    const size_t width = canvas.get_width();
    const size_t height = canvas.get_height();
#pragma omp parallel for schedule(dynamic)
    for (size_t row = 0; row < height; ++row) {
        float ndc_y = 1 - (row + 0.5f) / height * 2;
        float dy = ndc_y * view_height / 2;
        for (size_t col = 0; col < width; ++col) {
            float ndc_x = (col + 0.5f) / width * 2 - 1;
            float dx = ndc_x * view_width / 2;
            Vec3 ray_dir = (dx * camera.right + dy * camera.up + camera.dir).normalized();
            Ray3 ray(camera.pos, ray_dir);
            Color color = trace_ray(scene, ray, depth);
            canvas.set_pixel(row, col, color.as_srgb());
        }
    }
}
