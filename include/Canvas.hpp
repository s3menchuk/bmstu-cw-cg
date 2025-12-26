#pragma once

#include "Color.hpp"

#include <cstdint>
#include <memory>

#include <SFML/Graphics.hpp>

class Canvas {
  public:
    virtual size_t get_width() const = 0;
    virtual size_t get_height() const = 0;
    virtual void set_pixel(size_t row, size_t col, const sRGB &color) = 0;
    virtual sRGB get_pixel(size_t row, size_t col) const = 0;

    virtual ~Canvas() = default;
};

class SFML_Canvas : public Canvas {
  public:
    SFML_Canvas(size_t width, size_t height) : width(width), height(height), pixels(sf::PrimitiveType::Points, width * height) {
        for (size_t row = 0; row < height; ++row)
            for (size_t col = 0; col < width; ++col)
                pixels[row * width + col].position = {static_cast<float>(col), static_cast<float>(row + 1)};
    }

    size_t get_width() const override {
        return width;
    }

    size_t get_height() const override {
        return height;
    }

    void set_pixel(size_t row, size_t col, const sRGB &color) override {
        pixels[row * width + col].color = sf::Color(color.r, color.g, color.b);
    };

    sRGB get_pixel(size_t row, size_t col) const override {
        sf::Color color = pixels[row * width + col].color;
        return {color.r, color.g, color.b};
    }

    sf::VertexArray pixels;

  private:
    size_t width;
    size_t height;
};

class SimpleCanvas : public Canvas {
  public:
    SimpleCanvas(size_t width, size_t height) : width(width), height(height), pixels(height, std::vector<sRGB>(width)) {}

    size_t get_width() const override {
        return width;
    }

    size_t get_height() const override {
        return height;
    }

    void set_pixel(size_t row, size_t col, const sRGB &color) override {
        pixels[row][col] = color;
    };

    sRGB get_pixel(size_t row, size_t col) const override {
        return pixels[row][col];
    }

  private:
    size_t width;
    size_t height;
    std::vector<std::vector<sRGB>> pixels;
};