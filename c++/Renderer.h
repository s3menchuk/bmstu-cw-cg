#pragma once

#include "Canvas.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"
#include "Object.h"

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

	Color trace_ray(const Scene &scene, const Ray &ray, size_t depth = 3);

	std::shared_ptr<Object> find_closest_obj(const Scene& scene, const Ray& ray) const;

	bool is_in_shadow(const Scene& scene, std::shared_ptr<Object> object, const Vector& point, const Light& light) const;
};