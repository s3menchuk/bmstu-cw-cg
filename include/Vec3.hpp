#pragma once

#include <cmath>
#include <iostream>
#include <array>

class Vec3
{
public:
	Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
	Vec3(const std::array<float, 3>& a) : Vec3(a[0], a[1], a[2]) {}

	float x, y, z;

	float length() const {
		return sqrt(x * x + y * y + z * z);
	}

	void normalize() {
		float len = length();
		if (!len)
			throw std::runtime_error("Cannot normalize a zero-length vector");
		x /= len;
		y /= len;
		z /= len;
	}

	float dot(const Vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vec3 cross(const Vec3& other) const {
		return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	float sqr() const {
		return dot(*this);
	}

	void rotate_x(float angle) {
		float new_y = y * std::cos(angle) - z * std::sin(angle);
		float new_z = y * std::sin(angle) + z * std::cos(angle);
		y = new_y;
		z = new_z;
	}

	void rotate_y(float angle) {
		float new_x = x * std::cos(angle) + z * std::sin(angle);
		float new_z = -x * std::sin(angle) + z * std::cosf(angle);
		x = new_x;
		z = new_z;
	}

	void rotate_z(float angle) {
		float new_x = x * std::cos(angle) - y * std::sin(angle);
		float new_y = x * std::sin(angle) + y * std::cos(angle);
		x = new_x;
		y = new_y;
	}

	void rotate(const Vec3& axis, float angle) {
		//float c = std::cos(angle);
		//float s = std::sin(angle);

		//float m00 = c + (1 - c) * axis.x * axis.x;
		//float m01 = (1 - c) * axis.x * axis.y - s * axis.z;
		//float m02 = (1 - c) * axis.x * axis.z + s * axis.y;

		//float m10 = (1 - c) * axis.y * axis.x + s * axis.z;
		//float m11 = c + (1 - c) * axis.y * axis.y;
		//float m12 = (1 - c) * axis.y * axis.z - s * axis.x;

		//float m20 = (1 - c) * axis.z * axis.x - s * axis.y;
		//float m21 = (1 - c) * axis.z * axis.y + s * axis.x;
		//float m22 = c + (1 - c) * axis.z * axis.z;

		//
		//float new_x = m00 * x + m01 * y + m02 * z;
		//float new_y = m10 * x + m11 * y + m12 * z;
		//float new_z = m20 * x + m21 * y + m22 * z;

		//x = new_x;
		//y = new_y;
		//z = new_z;
		
		*this = *this * std::cos(angle) + axis.cross(*this) * std::sin(angle) + axis * axis.dot(*this) * (1 - std::cos(angle));
	}

	Vec3 operator*(float scalar) const {
		return Vec3(x * scalar, y * scalar, z * scalar);
	}
	
	Vec3 operator/(float scalar) const {
		return Vec3(x / scalar, y / scalar, z / scalar);
	}

	Vec3 operator+(const Vec3& other) const {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	Vec3 operator-(const Vec3& other) const {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}

	Vec3& operator+=(const Vec3& other) {
		*this = *this + other;
		return *this;
	}

	Vec3& operator-=(const Vec3& other) {
		*this = *this - other;
		return *this;
	}

	float operator[](size_t index) const {
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Vector index out of bounds!");
	}

	float& operator[](size_t index) {
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Vector index out of bounds!");
	}

	bool operator==(const Vec3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	operator std::array<float, 3>() const {
		return { x, y, z };
	}
};

using Point3 = Vec3;

Vec3 operator*(float scalar, const Vec3& vector);

std::ostream& operator<<(std::ostream& out, const Vec3& vector);