#include <vector>
#include <numbers>

#include "Object.hpp"

void move_points_by(std::vector<Point3>& points, const Vec3& offset) {
	for (Point3& p : points)
		p += offset;
}

std::vector<Point3> get_points_on_circle(const Vec3& center, float radius, size_t order) {
	std::vector<Point3> res;
	for (size_t i = 0; i < order; ++i) {
		float radians = static_cast<float>(i) / order * (2 * std::numbers::pi);
		float dx = std::cos(radians);
		float dy = std::sin(radians);
		Point3 point = center + Vec3(0, dx, dy) * radius;
		res.push_back(point);
	}
	return res;
}