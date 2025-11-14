#include "Vec3.hpp"

Vec3 operator*(float scalar, const Vec3& vector) {
	return vector * scalar;
}

std::ostream& operator<<(std::ostream& out, const Vec3& vector) {
	out << vector.x << " " << vector.y << " " << vector.z;
	return out;
}