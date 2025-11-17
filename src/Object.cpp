#include "Object.hpp"

#include <numbers>
#include <vector>

void move_points_by(std::vector<Point3> &points, const Vec3 &offset) {
	for (Point3 &p : points)
		p += offset;
}

std::vector<Point3> get_points_on_circle(const Vec3 &center, T radius, size_t order) {
	std::vector<Point3> res;
	for (size_t i = 0; i < order; ++i) {
		auto radians = static_cast<T>(i) / order * (2 * std::numbers::pi);
		auto dx = std::cos(radians);
		auto dy = std::sin(radians);
		Point3 point = center + Vec3(0, dx, dy) * radius;
		res.push_back(point);
	}
	return res;
}

// ����� �������
Vec3 compute_centroid(const std::vector<Vec3> &points) {
	Vec3 c(0, 0, 0);
	for (const auto &p : points)
		c += p;
	return c / static_cast<T>(points.size());
}

// ���������� ������ �� ����� � ���������
void sort_vertices_ccw3d(std::vector<Vec3> &vertices) {
	if (vertices.size() < 3)
		return;

	Vec3 center = compute_centroid(vertices);
	Vec3 v1 = vertices[0] - center;
	Vec3 v2 = vertices[1] - center;

	// ������� � ��������� ����� ��� ������ �������
	Vec3 normal = v1.cross(v2);
	normal.normalize();

	// ������ ����� � ���������: u � v � ��� �������������� ���
	Vec3 u = v1; // ������ ��� ����� v1
	u.normalize();
	Vec3 v = normal.cross(u); // ������ ��� ������������ u � �������
	v.normalize();

	// ���������� �� ���� atan2 ����� u � v
	std::sort(vertices.begin(), vertices.end(), [&](const Vec3 &a, const Vec3 &b) {
		Vec3 da = a - center;
		Vec3 db = b - center;
		double angleA = std::atan2(da.dot(v), da.dot(u));
		double angleB = std::atan2(db.dot(v), db.dot(u));
		return angleA < angleB; // CCW
	});
}

const std::string Sphere::type = "Sphere";
const std::string Plane::type = "Plane";
const std::string Quad::type = "Quad";
const std::string Triangle::type = "Triangle";
const std::string Mesh::type = "Mesh";
const std::string Box::type = "Box";
const std::string RightPrism::type = "Right Prism";
const std::string RightPyramid::type = "Right Pyramid";
const std::string Model::type = "Model";
