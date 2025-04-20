#include "Vector.h"

Vector operator*(float scalar, const Vector& vector) {
	return vector * scalar;
}

std::ostream& operator<<(std::ostream& out, const Vector& vector) {
	out << vector.x << " " << vector.y << " " << vector.z;
	return out;
}