#pragma once

#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "Object.hpp"

enum class Primitive { Sphere, Plane, Triangle, Quad, Box, RightPrism, RightPyramid, Mesh, Model, Translate, Rotate, Scale };

const std::unordered_map<std::type_index, Primitive> PRIMITIVE_TYPES = {
    {typeid(Sphere), Primitive::Sphere},
    {typeid(Plane), Primitive::Plane},
    {typeid(Triangle), Primitive::Triangle},
    {typeid(Quad), Primitive::Quad},
    {typeid(Box), Primitive::Box},
    {typeid(RightPrism), Primitive::RightPrism},
    {typeid(RightPyramid), Primitive::RightPyramid},
    {typeid(Mesh), Primitive::Mesh},
    {typeid(Model), Primitive::Model},
    {typeid(Translate), Primitive::Translate},
    {typeid(Rotate), Primitive::Rotate},
    {typeid(Scale), Primitive::Scale},
};

const std::unordered_map<Primitive, std::string> PRIMITIVE_NAMES = {{Primitive::Sphere, "Sphere"},
                                                                    {Primitive::Plane, "Plane"},
                                                                    {Primitive::Triangle, "Triangle"},
                                                                    {Primitive::Quad, "Quad"},
                                                                    {Primitive::Box, "Box"},
                                                                    {Primitive::RightPrism, "RightPrism"},
                                                                    {Primitive::RightPyramid, "RightPyramid"},
                                                                    {Primitive::Mesh, "Mesh"},
                                                                    {Primitive::Model, "Model"},
                                                                    {Primitive::Translate, "Translate"},
                                                                    {Primitive::Rotate, "Rotate"},
                                                                    {Primitive::Scale, "Scale"}};