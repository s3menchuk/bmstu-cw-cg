#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

class Color {
  public:
	constexpr Color() : Color(0, 0, 0) {}
	constexpr Color(uint8_t c) : Color(c, c, c) {}
	constexpr Color(uint8_t R, uint8_t G, uint8_t B) : R(R), G(G), B(B) {}
	constexpr Color(const std::array<float, 3> &a) : Color(a[0] * 255, a[1] * 255, a[2] * 255) {}

	uint8_t R, G, B;

	static Color normalize(size_t r, size_t g, size_t b) {
		uint16_t max = std::max({r, g, b});
		if (max > 255) {
			float k = 255.0f / max;
			r *= k;
			g *= k;
			b *= k;
		}
		return Color(r, g, b);
	}

	std::array<float, 3> as_float_rgb() const {
		return {static_cast<float>(R) / 255, static_cast<float>(G) / 255, static_cast<float>(B) / 255};
	}

	Color operator*(float k) const {
		return normalize(static_cast<size_t>(R) * k, static_cast<size_t>(G) * k, static_cast<size_t>(B) * k);
	}

	Color operator/(float k) const { return *this * (1 / k); }

	Color operator*(const Color &other) const {
		return normalize(static_cast<size_t>(R) * other.R, static_cast<size_t>(G) * other.G,
						 static_cast<size_t>(B) * other.B);
	}

	Color operator+(const Color &other) const {
		return normalize(static_cast<size_t>(R) + other.R, static_cast<size_t>(G) + other.G,
						 static_cast<size_t>(B) + other.B);
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

	bool operator==(const Color &other) const { return R == other.R && G == other.G && B == other.B; }
};

Color inline operator*(float k, const Color &color) { return color * k; }

constexpr Color BLACK(0, 0, 0);
constexpr Color WHITE(255, 255, 255);

constexpr Color RED(255, 0, 0);
constexpr Color GREEN(0, 255, 0);
constexpr Color BLUE(0, 0, 255);
constexpr Color SKY_BLUE(127, 178, 255);

constexpr Color YELLOW(255, 255, 0);
constexpr Color PURPLE(139, 0, 255);
constexpr Color ORANGE(255, 165, 0);
constexpr Color TEAL(0, 128, 128);

constexpr Color GRAY(128, 128, 128);