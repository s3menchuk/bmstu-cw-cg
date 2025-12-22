#pragma once

#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "Object.hpp"

const std::unordered_map<std::type_index, std::string_view> OBJECT_TYPES = {
    {typeid(Sphere), "Sphere"}, {typeid(Plane), "Plane"},           {typeid(Triangle), "Triangle"},         {typeid(Quad), "Quad"},
    {typeid(Box), "Box"},       {typeid(RightPrism), "RightPrism"}, {typeid(RightPyramid), "RightPyramid"}, {typeid(Mesh), "Mesh"},
    {typeid(Model), "Model"}};