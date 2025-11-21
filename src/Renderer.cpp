#include "Renderer.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "Ray.hpp"
#include "Vec3.hpp"

#include <iostream>

bool RayTracingRenderer::is_in_shadow(const Scene &scene, const std::shared_ptr<Object> &object, const Vec3 &point,
									  const Light &light) const {
	Ray3 ray(point, -light.get_direction(point));
	HitRecord hit_record;
	for (const auto &obj : scene.get_visible_objects())
		if (obj != object && (*obj)->hit(ray, hit_record))
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

Color RayTracingRenderer::background_color(const Ray3 &ray) const {
	float k = map(ray.direction.x, -1, 1, 0, 1);
	return SKY_BLUE * (1.0f - k) + WHITE * k;
}

Color RayTracingRenderer::trace_ray(const Scene &scene, const Ray3 &ray, size_t depth) {
	auto closest_obj = find_closest_obj(scene, ray);
	if (!closest_obj) {
		return background_color(ray);
	}

	HitRecord hit_record;
	(*closest_obj)->hit(ray, hit_record);

	/*
	uint8_t r = map(hit_record.normal.x, -1, 1, 0, 255);
	uint8_t g = map(hit_record.normal.y, -1, 1, 0, 255);
	uint8_t b = map(hit_record.normal.z, -1, 1, 0, 255);
	return Color(r, g, b);
	*/

	Color ambient(closest_obj->material.ambient * scene.ambient_color * scene.ambient_intensity);
	Color diffuse;
	Color specular;
	for (const auto &light : scene.lights) {
		if (hit_record.normal.dot(light->get_direction(hit_record.point)) < 0 &&
			!is_in_shadow(scene, closest_obj, hit_record.point, *light)) {
			Vec3 L = -light->get_direction(hit_record.point);
			Vec3 H = 2 * hit_record.normal.dot(L) * hit_record.normal - L;
			diffuse += closest_obj->material.diffuse * light->color * std::max(hit_record.normal.dot(L), 0.0f) *
					   light->get_intensity(hit_record.point);
			specular += closest_obj->material.specular * light->color *
						std::pow(std::max(hit_record.normal.dot(H), 0.0f), closest_obj->material.shininess) *
						light->get_intensity(hit_record.point);
		}
	}
	Color phong_color = ambient + diffuse + specular;
	Color final_color = phong_color * (1 - closest_obj->material.reflective);

	if (depth > 1 && closest_obj->material.reflective > 0) {
		Vec3 reflected_dir = ray.direction - 2 * hit_record.normal.dot(ray.direction) * hit_record.normal;
		Ray3 reflected_ray = Ray3(hit_record.point + reflected_dir * EPSILON, reflected_dir);
		Color reflective_color = trace_ray(scene, reflected_ray, depth - 1);
		final_color += reflective_color * closest_obj->material.reflective;
	}
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