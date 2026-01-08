#pragma once

#include "Color.hpp"

#include <cstdint>
#include <memory>

#include <SFML/Graphics.hpp>

class Canvas {
  public:
    virtual int get_width() const = 0;
    virtual int get_height() const = 0;
    virtual void set_pixel(int row, int col, const sRGB &color) = 0;
    virtual sRGB get_pixel(int row, int col) const = 0;

    virtual ~Canvas() = default;
};

class SFML_Canvas : public Canvas {
  public:
    SFML_Canvas(int width, int height) : width(width), height(height), pixels(sf::PrimitiveType::Points, width * height) {
        for (int row = 0; row < height; ++row)
            for (int col = 0; col < width; ++col)
                pixels[row * width + col].position = {static_cast<float>(col), static_cast<float>(row + 1)};
    }

    int get_width() const override {
        return width;
    }

    int get_height() const override {
        return height;
    }

    void set_pixel(int row, int col, const sRGB &color) override {
        pixels[row * width + col].color = sf::Color(color.r, color.g, color.b);
    };

    sRGB get_pixel(int row, int col) const override {
        sf::Color color = pixels[row * width + col].color;
        return {color.r, color.g, color.b};
    }

    sf::VertexArray pixels;

  private:
    int width;
    int height;
};

class SimpleCanvas : public Canvas {
  public:
    SimpleCanvas(int width, int height) : width(width), height(height), pixels(height, std::vector<sRGB>(width)) {}

    int get_width() const override {
        return width;
    }

    int get_height() const override {
        return height;
    }

    void set_pixel(int row, int col, const sRGB &color) override {
        pixels[row][col] = color;
    };

    sRGB get_pixel(int row, int col) const override {
        return pixels[row][col];
    }

  private:
    int width;
    int height;
    std::vector<std::vector<sRGB>> pixels;
};