#pragma once

#include "Color.h"

class Material {
public:
    Material(Color color, float reflective, float diffuse, float specular, float shininess) :
        color(color), reflective(reflective), diffuse(diffuse), specular(specular), shininess(shininess) {}

    Color color;
    float reflective;
    float diffuse;
    float specular;
    float shininess;
};