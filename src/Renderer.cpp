#include "Renderer.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "Ray.hpp"
#include "Vec3.hpp"

#include <iostream>

bool RayTracingRenderer::is_in_shadow(const Scene &scene, const Point3 &point, const Light &light) const {
    Ray3 ray(point, -light.get_direction(point));
    ray.origin = ray.at(EPSILON);
    HitRecord hit_record;
    for (const auto &obj : scene.get_visible_objects())
        if ((*obj)->hit(ray, hit_record))
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

Color RayTracingRenderer::background_color(const Scene &scene, const Ray3 &ray) const {
    // return scene.background_color;
    float k = map(ray.direction.x, -1, 1, 0, 1);
    return 1.75 * scene.background_color * (1.0f - k);
}

Color RayTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, size_t depth) {
    if (depth == 0)
        return Color();

    auto closest_obj = find_closest_obj(scene, ray);
    if (!closest_obj) {
        return background_color(scene, ray);
    }

    HitRecord hit_record;
    (*closest_obj)->hit(ray, hit_record);

    // float r = map(hit_record.normal.x, -1, 1, 0, 1);
    // float g = map(hit_record.normal.y, -1, 1, 0, 1);
    // float b = map(hit_record.normal.z, -1, 1, 0, 1);
    // return Color(r, g, b);

    // Color ambient = scene.ambient_color * scene.ambient_intensity;

    const Vec3 N = hit_record.normal;

    Color diffuse_intensity;
    for (const auto &light : scene.lights) {
        if (!is_in_shadow(scene, hit_record.point, *light)) {
            Vec3 L = -light->get_direction(hit_record.point);
            diffuse_intensity += std::max(N.dot(L), 0.0f) * light->color * light->get_intensity(hit_record.point);
        }
    }
    Color diffuse_color = (1 - closest_obj->material.metallic) * closest_obj->material.albedo;
    Color diffuse_total = diffuse_intensity * diffuse_color;

    const Vec3 V = -ray.direction;
    Color specular_intensity;
    for (const auto &light : scene.lights) {
        if (!is_in_shadow(scene, hit_record.point, *light)) {
            Vec3 L = -light->get_direction(hit_record.point);
            // Vec3 R = 2 * N.dot(L) * N - L;
            Vec3 H = L + V;
            H.normalize();
            specular_intensity += std::pow(std::max(N.dot(H), 0.0f), 200) * light->color * light->get_intensity(hit_record.point);
        }
    }
    Color F0 = Color::lerp(Color(0.04), closest_obj->material.albedo, closest_obj->material.metallic);
    Color specular_color = closest_obj->material.metallic * F0;
    Color specular_total = specular_intensity * closest_obj->material.metallic;

    Color reflective_color;
    Vec3 reflected_dir = ray.direction - 2 * N.dot(ray.direction) * N;
    Ray3 reflected_ray(hit_record.point + reflected_dir * EPSILON, reflected_dir);
    reflective_color = trace_ray(scene, reflected_ray, depth - 1);

    Color final_color = diffuse_total + specular_total + reflective_color * closest_obj->material.metallic;
    return final_color;
}

void RayTracingRenderer::render(Canvas &canvas, const Scene &scene, const Camera &camera, size_t depth) {
    Vec3 view_center = camera.pos + camera.dir * camera.near;

    float view_height = 2 * std::tan(camera.fov / 2) * camera.near;
    float view_width = view_height * camera.aspect;

    Vec3 left_bottom_corner_view = view_center - camera.right * (view_width / 2) - camera.up * (view_height / 2);

    const size_t width = canvas.get_width();
    const size_t height = canvas.get_height();
#pragma omp parallel for schedule(dynamic)
    for (int row = 0; row < height; ++row) {
        float dy = map(row, 0, height, 0, view_height);
        for (int col = 0; col < width; ++col) {
            float dx = map(col, 0, width, 0, view_width);
            Vec3 point = left_bottom_corner_view + camera.right * dx + camera.up * dy;
            Vec3 direction = point - camera.pos;
            direction.normalize();
            Ray3 ray(camera.pos, direction);
            Color color = trace_ray(scene, ray, depth);
            canvas.set_pixel(row, col, color);
        }
    }
}
