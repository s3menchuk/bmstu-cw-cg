#include "Canvas.hpp"

void accumulate_frame(Canvas &target, const Canvas &source, int frame_num) {
    int width = target.get_width();
    int height = target.get_height();

#pragma omp parallel for schedule(dynamic)
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            Color prev_color = target.get_pixel(row, col);
            Color curr_color = source.get_pixel(row, col);
            Color updt_color = (prev_color * (frame_num - 1) + curr_color) / frame_num;
            target.set_pixel(row, col, updt_color);
        }
    }
}