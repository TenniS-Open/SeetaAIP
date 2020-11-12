//
// Created by seeta on 2020/11/10.
//

#include "seeta_aip_plot.h"
#include "seeta_aip_image_io.h"

seeta::aip::plot::Color random_color(int seed) {
    srand(seed);
    return {uint8_t(rand() % 255),
            uint8_t(rand() % 255),
            uint8_t(rand() % 255)};
}

int main() {
    using namespace seeta::aip;
    seeta::aip::ImageData image;

    if (!image.width()) {
        image = seeta::aip::ImageData(SEETA_AIP_FORMAT_U8RGB, 640, 640, 3);
        memset(image.data(), 0, image.width() * image.height() * image.channels());
    }

    plot::rectangle(image, {100, 100}, {110, 110}, {plot::red, 32}, -1);
    plot::rectangle(image, {100, 120}, {110, 130}, {plot::red, 64}, -1);
    plot::rectangle(image, {100, 140}, {110, 150}, {plot::red, 128}, -1);
    plot::rectangle(image, {100, 160}, {110, 170}, {plot::red, 160}, -1);
    plot::rectangle(image, {100, 180}, {110, 190}, {plot::red, 192}, -1);
    plot::rectangle(image, {100, 200}, {110, 210}, {plot::red, 255}, -1);

    plot::circle(image, {200, 100}, 5, {255, 0, 0}, 3);
    plot::circle(image, {200, 200}, 9, {255, 0, 0}, 3);
    plot::circle(image, {200, 300}, 12, {255, 0, 0}, 1);
    plot::circle(image, {200, 400}, 19, {255, 0, 0}, 3);
    plot::circle(image, {200, 500}, 22, {255, 0, 0}, 6);
    plot::circle(image, {250, 100}, 5, {255, 0, 0}, -1);
    plot::circle(image, {250, 200}, 9, {255, 0, 0}, -1);
    plot::circle(image, {250, 300}, 12, {255, 0, 0}, -1);
    plot::circle(image, {250, 400}, 19, {255, 0, 0}, -1);
    plot::circle(image, {250, 500}, 22, {255, 0, 0}, -1);
    plot::rectangle(image, {300, 100}, {320, 130}, {255, 0, 0, 128}, 3);
    plot::rectangle(image, {300, 300}, {300, 300}, {255, 0, 0, 128}, 15);
    plot::rectangle(image, {300, 500}, {300, 300}, {255, 0, 0, 128}, 1);

    {
        SeetaAIPPoint o = {400, 400};
        float edge = 50;
        int lw = 1;
        plot::line(image, o, {o.x + edge / 2, o.y - edge}, random_color(3), lw);
        plot::line(image, o, {o.x + edge, o.y - edge}, random_color(4), lw);
        plot::line(image, o, {o.x + edge, o.y - edge / 2}, random_color(5), lw);
        plot::line(image, o, {o.x + edge, o.y}, random_color(6), lw);
        plot::line(image, o, {o.x + edge, o.y + edge / 2}, random_color(7), lw);
        plot::line(image, o, {o.x + edge, o.y + edge}, random_color(8), lw);
        plot::line(image, o, {o.x + edge / 2, o.y + edge}, random_color(9), lw);
        plot::line(image, o, {o.x, o.y + edge}, random_color(10), lw);
        plot::line(image, o, {o.x - edge / 2, o.y + edge}, random_color(11), lw);
        plot::line(image, o, {o.x - edge, o.y + edge}, random_color(12), lw);
        plot::line(image, o, {o.x - edge, o.y + edge / 2}, random_color(13), lw);
        plot::line(image, o, {o.x - edge, o.y}, random_color(14), lw);
        plot::line(image, o, {o.x - edge, o.y - edge / 2}, random_color(15), lw);
        plot::line(image, o, {o.x - edge, o.y - edge}, random_color(0), lw);
        plot::line(image, o, {o.x - edge / 2, o.y - edge}, random_color(1), lw);
        plot::line(image, o, {o.x, o.y - edge}, random_color(2), lw);
    }

    {
        SeetaAIPPoint o = {400, 200};
        float edge = 50;
        int lw = 4;
        plot::line(image, o, {o.x + edge / 2, o.y - edge}, random_color(3), lw);
        plot::line(image, o, {o.x + edge, o.y - edge}, random_color(4), lw);
        plot::line(image, o, {o.x + edge, o.y - edge / 2}, random_color(5), lw);
        plot::line(image, o, {o.x + edge, o.y}, random_color(6), lw);
        plot::line(image, o, {o.x + edge, o.y + edge / 2}, random_color(7), lw);
        plot::line(image, o, {o.x + edge, o.y + edge}, random_color(8), lw);
        plot::line(image, o, {o.x + edge / 2, o.y + edge}, random_color(9), lw);
        plot::line(image, o, {o.x, o.y + edge}, random_color(10), lw);
        plot::line(image, o, {o.x - edge / 2, o.y + edge}, random_color(11), lw);
        plot::line(image, o, {o.x - edge, o.y + edge}, random_color(12), lw);
        plot::line(image, o, {o.x - edge, o.y + edge / 2}, random_color(13), lw);
        plot::line(image, o, {o.x - edge, o.y}, random_color(14), lw);
        plot::line(image, o, {o.x - edge, o.y - edge / 2}, random_color(15), lw);
        plot::line(image, o, {o.x - edge, o.y - edge}, random_color(0), lw);
        plot::line(image, o, {o.x - edge / 2, o.y - edge}, random_color(1), lw);
        plot::line(image, o, {o.x, o.y - edge}, random_color(2), lw);
    }

    imwrite("test.jpg", image);
    return 0;
}