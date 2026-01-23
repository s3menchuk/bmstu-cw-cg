#pragma once

#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "Object.hpp"

enum class ePrimitive { Sphere, Plane, Triangle, Quad, Box, RightPrism, RightPyramid, Mesh, Model, Translate, Rotate, Scale };

const std::unordered_map<std::type_index, ePrimitive> PRIMITIVE_TYPES = {
    {typeid(Sphere), ePrimitive::Sphere},
    {typeid(Plane), ePrimitive::Plane},
    {typeid(Triangle), ePrimitive::Triangle},
    {typeid(Quad), ePrimitive::Quad},
    {typeid(Box), ePrimitive::Box},
    {typeid(RightPrism), ePrimitive::RightPrism},
    {typeid(RightPyramid), ePrimitive::RightPyramid},
    {typeid(Mesh), ePrimitive::Mesh},
    {typeid(Model), ePrimitive::Model},
    {typeid(Translate), ePrimitive::Translate},
    {typeid(Rotate), ePrimitive::Rotate},
    {typeid(Scale), ePrimitive::Scale},
};

const std::unordered_map<ePrimitive, std::string> PRIMITIVE_NAMES = {{ePrimitive::Sphere, "Sphere"},
                                                                     {ePrimitive::Plane, "Plane"},
                                                                     {ePrimitive::Triangle, "Triangle"},
                                                                     {ePrimitive::Quad, "Quad"},
                                                                     {ePrimitive::Box, "Box"},
                                                                     {ePrimitive::RightPrism, "Right Prism"},
                                                                     {ePrimitive::RightPyramid, "Right Pyramid"},
                                                                     {ePrimitive::Mesh, "Mesh"},
                                                                     {ePrimitive::Model, "Model"},
                                                                     {ePrimitive::Translate, "Translate"},
                                                                     {ePrimitive::Rotate, "Rotate"},
                                                                     {ePrimitive::Scale, "Scale"}};