#pragma once

#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>

#include "Object.hpp"

enum class ePrimitive { Sphere, Plane, Triangle, Quad, Box, RightPrism, RightPyramid, Mesh, Model };

const std::unordered_map<std::type_index, ePrimitive> PRIMITIVE_TYPES = {{typeid(Sphere), ePrimitive::Sphere},
                                                                         {typeid(Plane), ePrimitive::Plane},
                                                                         {typeid(Triangle), ePrimitive::Triangle},
                                                                         {typeid(Quad), ePrimitive::Quad},
                                                                         {typeid(Box), ePrimitive::Box},
                                                                         {typeid(RightPrism), ePrimitive::RightPrism},
                                                                         {typeid(RightPyramid), ePrimitive::RightPyramid},
                                                                         {typeid(Mesh), ePrimitive::Mesh},
                                                                         {typeid(Model), ePrimitive::Model}};

const std::unordered_map<ePrimitive, std::string> PRIMITIVE_NAMES = {{ePrimitive::Sphere, "Sphere"},
                                                                     {ePrimitive::Plane, "Plane"},
                                                                     {ePrimitive::Triangle, "Triangle"},
                                                                     {ePrimitive::Quad, "Quad"},
                                                                     {ePrimitive::Box, "Box"},
                                                                     {ePrimitive::RightPrism, "Right Prism"},
                                                                     {ePrimitive::RightPyramid, "Right Pyramid"},
                                                                     {ePrimitive::Mesh, "Mesh"},
                                                                     {ePrimitive::Model, "Model"}};

const std::unordered_set<ePrimitive> SELECTED_AVAILABLE_PRIMITIVES = {ePrimitive::Sphere, ePrimitive::Plane, ePrimitive::Box, ePrimitive::RightPrism,
                                                                      ePrimitive::RightPyramid};