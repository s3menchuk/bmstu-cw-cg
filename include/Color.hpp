#pragma once

#include "Interval.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

class Color {
public:
  constexpr Color() : Color(0, 0, 0) {}
  constexpr Color(float c) : Color(c, c, c) {}
  constexpr Color(float R, float G, float B) : r(R), g(G), b(B) {}
  constexpr Color(const std::array<float, 3> &a) : Color(a[0], a[1], a[2]) {}

  float r, g, b;

  static const Color BLACK;
  static const Color GRAY;
  static const Color WHITE;

  static const Color RED;
  static const Color ORANGE;
  static const Color YELLOW;
  static const Color GREEN;
  static const Color BLUE;
  static const Color PURPLE;

  static const Color SKY_BLUE;
  static const Color TEAL;

  std::array<float, 3> as_hdr_array() const { return {r, g, b}; }

  std::array<uint8_t, 3> as_rgb_array() const {
    uint8_t R = process_component(r);
    uint8_t G = process_component(g);
    uint8_t B = process_component(b);
    return {R, G, B};
  }

  Color operator*(float k) const { return {r * k, g * k, b * k}; }

  Color operator/(float k) const { return {r / k, g / k, b / k}; }

  Color operator*(const Color &other) const { return {r * other.r, g * other.g, b * other.b}; }

  Color operator+(const Color &other) const { return {r + other.r, g + other.g, b + other.b}; }

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

  bool operator==(const Color &other) const { return r == other.r && g == other.g && b == other.b; }

private:
  static uint8_t process_component(float c) {
    // return Interval(0, 1).clamp(c) * 255;
    c /= 1 + c;                  // Reinhard tone mapping
    c = Interval(0, 1).clamp(c); // Clamping
    // c = std::pow(c, 1.0f / 2.2f); // Gamma correction
    return c * 255;
  };
};

Color inline operator*(float k, const Color &color) { return color * k; }