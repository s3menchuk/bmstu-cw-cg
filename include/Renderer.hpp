#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "Types.hpp"

#include <cfloat>

std::shared_ptr<Object> find_closest_obj(HitRecord &closest_hit, const Scene &scene, const Ray3 &ray,
                                         Real max_dist = std::numeric_limits<Real>::infinity());

class Renderer {
  public:
    virtual void render(Canvas &canvas, const Scene &scene, const Camera &camera) = 0;  // Strategy

    virtual ~Renderer() {};
};

class TracingRenderer : public Renderer {
  public:
    void render(Canvas &canvas, const Scene &scene, const Camera &camera) override;

    int max_ray_bounces = 1;
    int count_threads = 1;
    int samples_per_pixel = 1;

  private:
    virtual Color trace_ray(const Scene &scene, const Ray3 &ray, int max_ray_bounces) const = 0;  // Template method
};

class RayTracingRenderer : public TracingRenderer {
  public:
    Color background_color = Color(0, 0, 0);

  private:
    const Real EPSILON = 1e-5;

    Color trace_ray(const Scene &scene, const Ray3 &ray, int max_ray_bounces) const override;
};

class PathTracingRenderer : public TracingRenderer {

  private:
    Color trace_ray(const Scene &scene, const Ray3 &ray, int max_ray_bounces) const override;
    const Real EPSILON = 1e-5;
};