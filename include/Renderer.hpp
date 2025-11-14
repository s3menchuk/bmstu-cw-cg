#pragma once

#include "Canvas.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Ray.hpp"
#include "Object.hpp"

#include <cfloat>

const float EPSILON = 1e-4;

class Renderer
{
public:
	virtual void render(Canvas& canvas, const Scene& scene, const Camera& camera, size_t depth = 3) = 0;

	virtual ~Renderer() {};
};

class RayTracingRenderer : public Renderer
{
public:
	void render(Canvas& canvas, const Scene& scene, const Camera& camera, size_t depth = 3) override;

	Color trace_ray(const Scene &scene, const Ray3 &ray, size_t depth = 3);

	std::shared_ptr<Object> find_closest_obj(const Scene& scene, const Ray3& ray) const;

	bool is_in_shadow(const Scene& scene, const std::shared_ptr<Object>& object, const Vec3& point, const Light& light) const;

	Color background_color(const Ray3& ray) const;
};