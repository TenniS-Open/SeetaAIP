//
// Created by seeta on 2020/11/10.
//

#include "seeta_aip_plot.h"
#include "seeta_aip_image_io.h"

int main() {
    using namespace seeta::aip;
    auto image = imread("a.jpg");
    plot::rectangle(image, {100, 100}, {105, 105}, {255, 0, 0}, 3);
    imwrite("test.jpg", image);
    return 0;
}