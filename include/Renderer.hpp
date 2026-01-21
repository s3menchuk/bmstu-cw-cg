#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "Types.hpp"

#include <cfloat>

struct RenderSettings {
    int max_ray_bounces;
    int count_threads;
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

    Color trace_ray(const Scene &scene, const Ray3 &ray, size_t depth) const;

    // std::shared_ptr<Object> find_closest_obj(HitRecord &closest_hit, const Scene &scene, const Ray3 &ray) const;

    std::shared_ptr<Object> find_closest_obj(HitRecord &closest_hit, const Scene &scene, const Ray3 &ray,
                                             Real max_dist = std::numeric_limits<Real>::infinity()) const;

    bool is_in_shadow(const Scene &scene, const HitRecord &hit, const Light &light) const;

    Color calc_diffuse(const Scene &scene, const Point3 &origin, const Point3 &point, const Vec3 &normal) const;
};
