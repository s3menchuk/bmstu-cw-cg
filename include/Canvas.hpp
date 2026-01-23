#pragma once

#include "Color.hpp"

#include <cstdint>
#include <memory>

#include <SFML/Graphics.hpp>

class Canvas {
  public:
    virtual int get_width() const = 0;
    virtual int get_height() const = 0;
    virtual void set_pixel(int row, int col, Color color) = 0;
    virtual Color get_pixel(int row, int col) const = 0;

    virtual ~Canvas() = default;
};

class SimpleCanvas : public Canvas {
  public:
    SimpleCanvas(int width, int height) : width(width), height(height), pixels(height, std::vector<Color>(width, Color(0, 0, 0))) {}

    int get_width() const override {
        return width;
    }

    int get_height() const override {
        return height;
    }

    void set_pixel(int row, int col, Color color) override {
        pixels[row][col] = color;
    };

    Color get_pixel(int row, int col) const override {
        return pixels[row][col];
    }

  private:
    int width;
    int height;
    std::vector<std::vector<Color>> pixels;
};

class SFML_Canvas : public Canvas {
  public:
    SFML_Canvas(int width, int height) : width(width), height(height), pixels(sf::PrimitiveType::Points, width * height), canvas(width, height) {
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

    void set_pixel(int row, int col, Color color) override {
        canvas.set_pixel(row, col, color);
        sRGB rgb = color.as_srgb();
        pixels[row * width + col].color = sf::Color(rgb.r, rgb.g, rgb.b);
    };

    Color get_pixel(int row, int col) const override {
        return canvas.get_pixel(row, col);
    }

    sf::VertexArray pixels;

  private:
    int width;
    int height;
    SimpleCanvas canvas;
};

void accumulate_frame(Canvas &target, const Canvas &source, int frame_num);