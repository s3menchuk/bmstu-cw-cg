#pragma once

#include "AABB.hpp"
#include "Object.hpp"
#include "Ray.hpp"

#include <memory>

/*
BVH
AABB
Ray intersection with AABB
Traverse BVH
Build BVH
Optimizations
*/

struct BVHNode;

std::unique_ptr<BVHNode> buildBVH(std::vector<Triangle> &triangles, size_t start, size_t end);

bool traverseBVH(const BVHNode *BVH, Ray3 ray, HitRecord &hit_info);