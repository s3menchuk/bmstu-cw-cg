#pragma once

#include "Types.hpp"

#include <array>
#include <cmath>
#include <iostream>

template <typename T>
struct Vector2 {
    T x, y;
};

template <typename T>
class Vector3 {
  public:
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    Vector3() : Vector3(0, 0, 0) {}
    Vector3(const std::array<T, 3> &a) : Vector3(a[0], a[1], a[2]) {}
    Vector3(const T a[3]) : Vector3(a[0], a[1], a[2]) {}

    T x, y, z;

    T length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        auto len = length();
        // if (len == 0)
        //     throw std::runtime_error("Cannot normalize a zero-length vector");
        x /= len;
        y /= len;
        z /= len;
    }

    Vector3 normalized() const {
        Vector3 copy = *this;
        copy.normalize();
        return copy;
    }

    T dot(const Vector3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross(const Vector3 &other) const {
        return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
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

    void rotate(const Vector3 &axis, T angle) {
        *this = *this * std::cos(angle) + axis.cross(*this) * std::sin(angle) + axis * axis.dot(*this) * (1 - std::cos(angle));
    }

    Vector3 operator*(T scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(T scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    Vector3 operator+(const Vector3 &other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3 &other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 &operator+=(const Vector3 &other) {
        *this = *this + other;
        return *this;
    }

    Vector3 &operator-=(const Vector3 &other) {
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

    bool operator==(const Vector3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    operator bool() const {
        return length() == 0;
    }

    operator std::array<T, 3>() const {
        return {x, y, z};
    }
};

template <typename T>
Vector3<T> operator*(T scalar, const Vector3<T> &vector) {
    return vector * scalar;
}

template <typename T>
Vector3<T> operator*(const Vector3<T> &a, const Vector3<T> &b) {
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

template <typename T>
Vector3<T> operator/(const Vector3<T> &a, const Vector3<T> &b) {
    return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}

template <typename T>
void operator>>(std::istream &in, Vector3<T> &vector) {
    return in >> vector.x >> vector.y >> vector.z;
}

template <typename T>
std::ostream &operator<<(std::ostream &out, const Vector3<T> &vector) {
    out << vector.x << " " << vector.y << " " << vector.z;
    return out;
}

// --- TYPES ---

// using Vec3f = Vector3<float>;
// using Vec3d = Vector3<double>;
// using Vec3i = Vector3<int>;

using Vec3 = Vector3<Real>;
using Point3 = Vec3;

// using Vec2f = Vector2<float>;
using Vec2 = Vector2<Real>;