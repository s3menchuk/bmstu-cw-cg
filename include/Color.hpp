#pragma once

#include "Interval.hpp"
#include "Math.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>

struct sRGB {
    uint8_t r, g, b;

    static const sRGB BLACK;
    static const sRGB GRAY;
    static const sRGB WHITE;

    static const sRGB RED;
    static const sRGB ORANGE;
    static const sRGB YELLOW;
    static const sRGB GREEN;
    static const sRGB CYAN;
    static const sRGB BLUE;
    static const sRGB VIOLET;

    static const sRGB SKY_BLUE;
    static const sRGB TEAL;
};

class Color {
  public:
    constexpr Color() : Color(0.f) {}
    constexpr Color(float c) : Color(c, c, c) {}
    constexpr Color(float R, float G, float B) : r(R), g(G), b(B) {}
    constexpr Color(float rgb[3]) : r(rgb[0]), g(rgb[1]), b(rgb[2]) {}
    constexpr Color(const sRGB &srgb) {
        r = srgb_to_linear(srgb.r);
        g = srgb_to_linear(srgb.g);
        b = srgb_to_linear(srgb.b);
    }

    float r, g, b;

    std::array<float, 3> as_linear_array() const {
        return {r, g, b};
    }

    sRGB as_srgb() const {
        uint8_t R = linear_to_srgb(r);
        uint8_t G = linear_to_srgb(g);
        uint8_t B = linear_to_srgb(b);
        return {R, G, B};
    }

    Color operator*(float k) const {
        return {r * k, g * k, b * k};
    }

    Color operator/(float k) const {
        return {r / k, g / k, b / k};
    }

    Color operator*(const Color &other) const {
        return {r * other.r, g * other.g, b * other.b};
    }

    Color operator+(const Color &other) const {
        return {r + other.r, g + other.g, b + other.b};
    }

    Color &operator+=(const Color &other) {
        *this = *this + other;
        return *this;
    }

    Color &operator*=(float scalar) {
        *this = *this * scalar;
        return *this;
    }

    Color &operator*=(const Color &other) {
        *this = *this * other;
        return *this;
    }

    static Color lerp(const Color &a, const Color &b, float t) {
        return a * (1 - t) + b * t;
    }

  private:
    static uint8_t linear_to_srgb(float c) {
        // Reinhard tone mapping
        c /= 1 + c;

        // Gamma correction
        if (c <= 0.0031308)
            c = 12.92 * c;
        else
            c = 1.055 * std::pow(c, 1 / 2.4) - 0.055;

        // Clamping
        c = Interval(0, 1).clamp(c);
        return c * 255;
    };

    static constexpr float srgb_to_linear(uint8_t srgb) {
        float c = srgb / 255.f;
        if (c <= 0.04045f)
            return c / 12.92f;
        else
            return std::pow((c + 0.055f) / 1.055f, 2.4f);
    }
};

Color inline operator*(float k, const Color &color) {
    return color * k;
}
