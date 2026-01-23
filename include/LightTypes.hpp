#pragma once

#include "Light.hpp"

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

enum class eLight { Directional, Point };

const std::unordered_map<std::type_index, eLight> LIGHT_TYPES = {{typeid(DirectionLight), eLight::Directional}, {typeid(PointLight), eLight::Point}};

const std::unordered_map<eLight, std::string> LIGHT_NAMES = {{eLight::Directional, "Directional Light"}, {eLight::Point, "Point Light"}};
