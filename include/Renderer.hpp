#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

#include <cfloat>

struct RenderSettings {
    int max_ray_bounces;
    int count_threads;
    int samples_per_pixel;
};

class Renderer {
  public:
    virtual void render(Canvas &canvas, const Scene &scene, const Camera &camera, const RenderSettings &settings) = 0;

    virtual ~Renderer() {};
};

class RayTracingRenderer : public Renderer {
  public:
    void render(Canvas &canvas, const Scene &scene, const Camera &camera, const RenderSettings &settings) override;

  private:
    const Real EPSILON = 1e-5;

    Color trace_ray(const Scene &scene, Ray3 ray, int depth) const;

    std::shared_ptr<Object> find_closest_obj(HitRecord &closest_hit, const Scene &scene, Ray3 ray) const;
};
