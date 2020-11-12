//
// Created by seeta on 2020/11/10.
//

#include "seeta_aip_plot.h"
#include "seeta_aip_color_map.h"
#include "seeta_aip_image_io.h"
#include "seeta_aip_plot_text.h"

int main() {
    using namespace seeta::aip;
    seeta::aip::ImageData image;

    if (!image.width()) {
        image = seeta::aip::ImageData(SEETA_AIP_FORMAT_U8RGB, 640, 640, 3);
        memset(image.data(), 0, image.width() * image.height() * image.channels());
    }

    plot::rectangle(image, {100, 100}, {110, 110}, {plot::Red, 32}, -1);
    plot::rectangle(image, {100, 120}, {110, 130}, {plot::Red, 64}, -1);
    plot::rectangle(image, {100, 140}, {110, 150}, {plot::Red, 128}, -1);
    plot::rectangle(image, {100, 160}, {110, 170}, {plot::Red, 160}, -1);
    plot::rectangle(image, {100, 180}, {110, 190}, {plot::Red, 192}, -1);
    plot::rectangle(image, {100, 200}, {110, 210}, {plot::Red, 255}, -1);

    plot::circle(image, {200, 100}, 5, {255, 0, 0}, 3);
    plot::circle(image, {200, 200}, 9, {255, 0, 0}, 3);
    plot::circle(image, {200, 300}, 12, {255, 0, 0}, 1);
    plot::circle(image, {200, 400}, 19, {255, 0, 0}, 2);
    plot::circle(image, {200, 500}, 22, {255, 0, 0}, 4);
    plot::circle(image, {250, 100}, 1, {255, 0, 0}, -1);
    plot::circle(image, {250, 200}, 2, {255, 0, 0}, -1);
    plot::circle(image, {250, 300}, 4, {255, 0, 0}, -1);
    plot::circle(image, {250, 400}, 13, {255, 0, 0}, -1);
    plot::circle(image, {250, 500}, 22, {255, 0, 0}, -1);
    plot::rectangle(image, {300, 100}, {320, 130}, {255, 0, 0, 128}, 3);
    plot::rectangle(image, {300, 300}, {300, 300}, {255, 0, 0, 128}, 15);
    plot::rectangle(image, {300, 500}, {300, 300}, {255, 0, 0, 128}, 1);

    {
        SeetaAIPPoint o = {400, 400};
        float edge = 50;
        int lw = 1;
        plot::line(image, o, {o.x + edge / 2, o.y - edge}, plot::rcolor(3), lw);
        plot::line(image, o, {o.x + edge, o.y - edge}, plot::rcolor(4), lw);
        plot::line(image, o, {o.x + edge, o.y - edge / 2}, plot::rcolor(5), lw);
        plot::line(image, o, {o.x + edge, o.y}, plot::rcolor(6), lw);
        plot::line(image, o, {o.x + edge, o.y + edge / 2}, plot::rcolor(7), lw);
        plot::line(image, o, {o.x + edge, o.y + edge}, plot::rcolor(8), lw);
        plot::line(image, o, {o.x + edge / 2, o.y + edge}, plot::rcolor(9), lw);
        plot::line(image, o, {o.x, o.y + edge}, plot::rcolor(10), lw);
        plot::line(image, o, {o.x - edge / 2, o.y + edge}, plot::rcolor(11), lw);
        plot::line(image, o, {o.x - edge, o.y + edge}, plot::rcolor(12), lw);
        plot::line(image, o, {o.x - edge, o.y + edge / 2}, plot::rcolor(13), lw);
        plot::line(image, o, {o.x - edge, o.y}, plot::rcolor(14), lw);
        plot::line(image, o, {o.x - edge, o.y - edge / 2}, plot::rcolor(15), lw);
        plot::line(image, o, {o.x - edge, o.y - edge}, plot::rcolor(0), lw);
        plot::line(image, o, {o.x - edge / 2, o.y - edge}, plot::rcolor(1), lw);
        plot::line(image, o, {o.x, o.y - edge}, plot::rcolor(2), lw);
    }

    {
        SeetaAIPPoint o = {400, 200};
        float edge = 50;
        int lw = 3;
        plot::line(image, o, {o.x + edge / 2, o.y - edge}, plot::rcolor(3), lw);
        plot::line(image, o, {o.x + edge, o.y - edge}, plot::rcolor(4), lw);
        plot::line(image, o, {o.x + edge, o.y - edge / 2}, plot::rcolor(5), lw);
        plot::line(image, o, {o.x + edge, o.y}, plot::rcolor(6), lw);
        plot::line(image, o, {o.x + edge, o.y + edge / 2}, plot::rcolor(7), lw);
        plot::line(image, o, {o.x + edge, o.y + edge}, plot::rcolor(8), lw);
        plot::line(image, o, {o.x + edge / 2, o.y + edge}, plot::rcolor(9), lw);
        plot::line(image, o, {o.x, o.y + edge}, plot::rcolor(10), lw);
        plot::line(image, o, {o.x - edge / 2, o.y + edge}, plot::rcolor(11), lw);
        plot::line(image, o, {o.x - edge, o.y + edge}, plot::rcolor(12), lw);
        plot::line(image, o, {o.x - edge, o.y + edge / 2}, plot::rcolor(13), lw);
        plot::line(image, o, {o.x - edge, o.y}, plot::rcolor(14), lw);
        plot::line(image, o, {o.x - edge, o.y - edge / 2}, plot::rcolor(15), lw);
        plot::line(image, o, {o.x - edge, o.y - edge}, plot::rcolor(0), lw);
        plot::line(image, o, {o.x - edge / 2, o.y - edge}, plot::rcolor(1), lw);
        plot::line(image, o, {o.x, o.y - edge}, plot::rcolor(2), lw);
    }

    plot::put_uint8_ascii(image, 'A', {500, 100}, plot::Red, 1);

    plot::rectangle(image, {499, 99}, {499 + 10, 99 + 18}, plot::Blue, 1);

    plot::text(image, "TenniS: Tensor based Edge Neural Network Inference Systems",
               {500, 200}, plot::Pink, 1, image.width());

    plot::put_uint8_ascii(image, 'a', {520, 100}, plot::Red, 3);

    imwrite("test.png", image);
    return 0;
}