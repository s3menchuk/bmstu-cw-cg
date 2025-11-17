#pragma once

#include <cmath>
#include <iostream>
#include <array>
#include "types.h"

class Vec3
{
public:
	constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
	constexpr Vec3() : Vec3(0, 0, 0) {}
	constexpr Vec3(const std::array<T, 3>& a) : Vec3(a[0], a[1], a[2]) {}

	T x, y, z;

	
	T length() const {
		return sqrt(x * x + y * y + z * z);
	}

	void normalize() {
		auto len = length();
		if (len == 0)
			throw std::runtime_error("Cannot normalize a zero-length vector");
		x /= len;
		y /= len;
		z /= len;
	}

	T dot(const Vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vec3 cross(const Vec3& other) const {
		return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	T sqr() const {
		return dot(*this);
	}

	void rotate_x(T angle) {
		T new_y = y * std::cos(angle) - z * std::sin(angle);
		T new_z = y * std::sin(angle) + z * std::cos(angle);
		y = new_y;
		z = new_z;
	}

	void rotate_y(T angle) {
		T new_x = x * std::cos(angle) + z * std::sin(angle);
		T new_z = -x * std::sin(angle) + z * std::cos(angle);
		x = new_x;
		z = new_z;
	}

	void rotate_z(T angle) {
		T new_x = x * std::cos(angle) - y * std::sin(angle);
		T new_y = x * std::sin(angle) + y * std::cos(angle);
		x = new_x;
		y = new_y;
	}

	void rotate(const Vec3& axis, T angle) {
		//T c = std::cos(angle);
		//T s = std::sin(angle);

		//T m00 = c + (1 - c) * axis.x * axis.x;
		//T m01 = (1 - c) * axis.x * axis.y - s * axis.z;
		//T m02 = (1 - c) * axis.x * axis.z + s * axis.y;

		//T m10 = (1 - c) * axis.y * axis.x + s * axis.z;
		//T m11 = c + (1 - c) * axis.y * axis.y;
		//T m12 = (1 - c) * axis.y * axis.z - s * axis.x;

		//T m20 = (1 - c) * axis.z * axis.x - s * axis.y;
		//T m21 = (1 - c) * axis.z * axis.y + s * axis.x;
		//T m22 = c + (1 - c) * axis.z * axis.z;

		//
		//T new_x = m00 * x + m01 * y + m02 * z;
		//T new_y = m10 * x + m11 * y + m12 * z;
		//T new_z = m20 * x + m21 * y + m22 * z;

		//x = new_x;
		//y = new_y;
		//z = new_z;
		
		*this = *this * std::cos(angle) + axis.cross(*this) * std::sin(angle) + axis * axis.dot(*this) * (1 - std::cos(angle));
	}

	Vec3 operator*(T scalar) const {
		return Vec3(x * scalar, y * scalar, z * scalar);
	}
	
	Vec3 operator/(T scalar) const {
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

	T operator[](size_t index) const {
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Vector index out of bounds!");
	}

	T& operator[](size_t index) {
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Vector index out of bounds!");
	}

	bool operator==(const Vec3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	operator bool() const { return length() == 0; }

	operator std::array<T, 3>() const {
		return { x, y, z };
	}
};

using Point3 = Vec3;

Vec3 operator*(T scalar, const Vec3& vector);

std::ostream& operator<<(std::ostream& out, const Vec3& vector);