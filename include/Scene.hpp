#pragma once

#include "Color.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Vec.hpp"

#include <memory>
#include <vector>

class Scene {
  public:
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    Color background_color = sRGB::SKY_BLUE;
    Vec3 world_up = {0, 1, 0};

    void add_object(const std::shared_ptr<Object> &object) {
        objects.push_back(object);
    }
    void add_light(const std::shared_ptr<Light> &light) {
        lights.push_back(light);
    }

    // TODO: VIA ITERATORS
    // ??? get_visible_objects() const {}

    Color get_background_color(const Ray3 &ray) const {
        Real k = map(ray.direction.x, (Real)-1, (Real)1, (Real)0, (Real)1);
        return 1.75 * background_color * (1 - k);
    }
};
