#pragma once

#include <array>
#include <cmath>
#include <iostream>

#include "Types.hpp"

class Vec3 {
  public:
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    Vec3() : Vec3(0, 0, 0) {}
    Vec3(const std::array<T, 3> &a) : Vec3(a[0], a[1], a[2]) {}
    Vec3(const T a[3]) : Vec3(a[0], a[1], a[2]) {}

    T x, y, z;

    T length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        // TODO: FIX!

        // auto len = length();
        // if (len == 0)
        //     throw std::runtime_error("Cannot normalize a zero-length vector");
        // x /= len;
        // y /= len;
        // z /= len;
        auto len = length();
        if (len != 0) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    Vec3 normalized() const {
        Vec3 copy = *this;
        copy.normalize();
        return copy;
    }

    T dot(const Vec3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 cross(const Vec3 &other) const {
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

    void rotate(const Vec3 &axis, T angle) {
        *this = *this * std::cos(angle) + axis.cross(*this) * std::sin(angle) + axis * axis.dot(*this) * (1 - std::cos(angle));
    }

    Vec3 operator*(T scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 operator/(T scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3 operator+(const Vec3 &other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3 &other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 &operator+=(const Vec3 &other) {
        *this = *this + other;
        return *this;
    }

    Vec3 &operator-=(const Vec3 &other) {
        *this = *this - other;
        return *this;
    }

    T operator[](size_t index) const {
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        if (index == 2)
            return z;
        throw std::out_of_range("Vector index out of bounds!");
    }

    T &operator[](size_t index) {
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        if (index == 2)
            return z;
        throw std::out_of_range("Vector index out of bounds!");
    }

    bool operator==(const Vec3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    operator bool() const {
        return length() == 0;
    }

    operator std::array<T, 3>() const {
        return {x, y, z};
    }
};

using Point3 = Vec3;

Vec3 inline operator*(T scalar, const Vec3 &vector) {
    return vector * scalar;
}

Vec3 inline operator*(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 inline operator/(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

std::ostream inline &operator<<(std::ostream &out, const Vec3 &vector) {
    out << vector.x << " " << vector.y << " " << vector.z;
    return out;
}
