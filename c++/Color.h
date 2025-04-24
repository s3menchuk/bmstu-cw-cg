#pragma once

#include <cstdint>
#include <algorithm>

class Color
{
public:
	Color() : Color(0, 0, 0) {}
	Color(uint8_t R, uint8_t G, uint8_t B) : R(R), G(G), B(B) {}
	uint8_t R, G, B;

	static Color normalize(size_t r, size_t g, size_t b) {
		uint16_t max = std::max({ r, g, b });
		if (max > 255) {
			float k = 255.0f / max;
			r *= k;
			g *= k;
			b *= k;
		}
		return Color(r, g, b);
	}

	Color operator*(float k) const {
		return normalize(static_cast<size_t>(R) * k, static_cast<size_t>(G) * k, static_cast<size_t>(B) * k);
	}

	Color operator/(float k) const {
		return *this * (1 / k);
	}

	Color operator*(const Color& other) const {
		return normalize(static_cast<size_t>(R) * other.R, static_cast<size_t>(G) * other.G, static_cast<size_t>(B) * other.B);
	}

	Color operator+(const Color& other) const {
		return normalize(static_cast<size_t>(R) + other.R, static_cast<size_t>(G) + other.G, static_cast<size_t>(B) + other.B);
	}

	Color& operator+=(const Color& other) {
		*this = *this + other;
		return *this;
	}

	Color& operator*=(float scalar) {
		*this = *this * scalar;
		return *this;
	}

	Color& operator*=(const Color& other) {
		*this = *this * other;
		return *this;
	}
};

Color operator*(float k, const Color& color);

const Color BLACK(0, 0, 0);
const Color WHITE(255, 255, 255);

const Color RED(255, 0, 0);
const Color GREEN(0, 255, 0);
const Color BLUE(0, 0, 255);
const Color SKY_BLUE(127, 178, 255);

const Color YELLOW(255, 255, 0);
const Color PURPLE(139, 0, 255);
const Color ORANGE(255, 165, 0);
const Color TEAL(0, 128, 128);

const Color GRAY(128, 128, 128);