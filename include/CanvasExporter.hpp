#pragma once

#include "Canvas.hpp"

#include <fstream>
#include <string>

class CanvasExporter {
  public:
    virtual bool save(const Canvas &canvas, const std::string &path) const = 0;

    virtual ~CanvasExporter() = default;
};

class CanvasPPMTextExporter : public CanvasExporter {
  public:
    bool save(const Canvas &canvas, const std::string &filename) const {
        std::ofstream fout(filename);
        if (!fout.is_open())
            return false;
        fout << "P3" << '\n';
        fout << canvas.get_width() << ' ' << canvas.get_height() << '\n';
        fout << 255 << '\n';
        for (size_t row = 0; row < canvas.get_height(); ++row) {
            for (size_t col = 0; col < canvas.get_width(); ++col) {
                sRGB color = canvas.get_pixel(row, col);
                fout << static_cast<int>(color.r) << ' ' << static_cast<int>(color.g) << ' ' << static_cast<int>(color.b) << '\n';
            }
        }
        fout.close();
        return true;
    }
};

class CanvasPPMBinaryExporter : public CanvasExporter {
  public:
    bool save(const Canvas &canvas, const std::string &filename) const {
        std::ofstream fout(filename, std::ios::binary);
        if (!fout.is_open())
            return false;
        fout << "P6" << '\n';
        fout << canvas.get_width() << ' ' << canvas.get_height() << '\n';
        fout << 255 << '\n';
        for (size_t row = 0; row < canvas.get_height(); ++row) {
            for (size_t col = 0; col < canvas.get_width(); ++col) {
                sRGB color = canvas.get_pixel(row, col);
                fout.write(reinterpret_cast<char *>(&color), 3);
            }
        }
        fout.close();
        return true;
    }
};