#include "BVH.hpp"

struct BVHNode {
    AABB aabb;
    std::unique_ptr<BVHNode> left = nullptr;
    std::unique_ptr<BVHNode> right = nullptr;
    Hittable *hittable = nullptr;
};

std::unique_ptr<BVHNode> buildBVH(std::vector<Triangle> &triangles, size_t start, size_t end) {
    AABB aabb;
    for (size_t i = start; i < end; ++i) {
        aabb.add_vertex(triangles[i].a);
        aabb.add_vertex(triangles[i].b);
        aabb.add_vertex(triangles[i].c);
    }
    size_t axis = aabb.x.size() > std::max(aabb.y.size(), aabb.z.size()) ? 0 : aabb.y.size() > aabb.z.size() ? 1 : 2;
    // Real mid = aabb.axis_interval(axis)
    return nullptr;
}

bool traverseBVH(const BVHNode *BVH, Ray3 ray, HitRecord &hit_info) {
    if (!BVH)
        return false;
    if (!(BVH->aabb.hit(ray)))
        return false;
    if (!BVH->left && !BVH->right)
        return BVH->hittable->hit(ray, hit_info);
    HitRecord hit_left;
    hit_left.dist = std::numeric_limits<Real>::infinity();
    traverseBVH(BVH->left.get(), ray, hit_left);
    HitRecord hit_right;
    hit_right.dist = std::numeric_limits<Real>::infinity();
    traverseBVH(BVH->right.get(), ray, hit_right);
    hit_info = hit_left.dist < hit_right.dist ? hit_left : hit_right;
    return true;
}
