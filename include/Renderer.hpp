#pragma once

#include "Camera.hpp"
#include "Canvas.hpp"
#include "Object.hpp"
#include "Ray3.hpp"
#include "Scene.hpp"

#include <cfloat>

const float EPSILON = 1e-4;

class Renderer {
  public:
    virtual void render(Canvas &canvas, const Scene &scene, const Camera &camera, size_t depth) = 0;

    virtual ~Renderer() {};
};

class RayTracingRenderer : public Renderer {
  public:
    void render(Canvas &canvas, const Scene &scene, const Camera &camera, size_t depth) override;

  private:
    Color trace_ray(const Scene &scene, const Ray3 &ray, size_t depth) const;

    std::shared_ptr<Object> find_closest_obj(const Scene &scene, const Ray3 &ray) const;

    bool is_in_shadow(const Scene &scene, const Point3 &point, const Light &light) const;
};
