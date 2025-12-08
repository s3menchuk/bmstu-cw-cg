#pragma once

#include "Color.hpp"
#include "Light.hpp"
#include "Object.hpp"
#include "Ray.hpp"

#include <memory>
#include <vector>

class Scene {
  public:
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    Color background_color;
    Color ambient_color;
    float ambient_intensity;

    void add_object(const std::shared_ptr<Object> &object) {
        objects.push_back(object);
    }
    void add_light(const std::shared_ptr<Light> &light) {
        lights.push_back(light);
    }

    std::vector<std::shared_ptr<Object>> get_visible_objects() const {
        std::vector<std::shared_ptr<Object>> visible_objects;
        for (const auto &obj : objects)
            if (obj->visible)
                visible_objects.push_back(obj);
        return visible_objects;
    }
};
