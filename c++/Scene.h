#pragma once

#include "Object.h"
#include "Ray.h"
#include "Color.h"
#include "Light.h"

#include <vector>
#include <memory>

class Scene
{
public:
	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<Light>> lights;
	
	Color ambient_color;
	float ambient_intensity;

	void add_object(std::shared_ptr<Object> object) { objects.push_back(object); }
	void add_light(std::shared_ptr<Light> light) { lights.push_back(light); }
};