#include <iostream>

#include "seeta_aip.h"

#include "seeta_aip_struct.h"
#include "seeta_aip_shape.h"


void plot_shape(const seeta::aip::Shape &shape) {
    std::cout << "cpp: ";
    for (auto &a : shape.landmarks()) {
        std::cout << "(" << a.x << ", " << a.y << ")" << " ";
    }
    std::cout << std::endl;

    auto b = shape.raw();

    std::cout << "  c: ";
    for (int i = 0; i < b->landmarks.size; ++i) {
        auto &a = b->landmarks.data[i];
        std::cout << "(" << a.x << ", " << a.y << ")" << " ";
    }
    std::cout << std::endl;
}


int main() {
    std::cout << "Hello, World!" << std::endl;

    seeta::aip::Shape shape = seeta::aip::Points({{1, 2}});

    auto b = shape;

    seeta::aip::Shape c;
    c = shape;

    plot_shape(shape);
    plot_shape(b);
    plot_shape(c);

    seeta::aip::ImageData image(SEETA_AIP_VALUE_BYTE, 320, 320, 3, nullptr);

    std::cout << seeta::aip::Points({{1, 2}}) << std::endl;
    std::cout << seeta::aip::Lines({{1, 2}, {3, 4}}) << std::endl;
    std::cout << seeta::aip::Rectangle({1, 2}, {3, 4}) << std::endl;
    std::cout << seeta::aip::Parallelogram({1, 2}, {3, 4}, {5, 6}) << std::endl;
    std::cout << seeta::aip::Polygon({{1, 2}, {3, 4}}) << std::endl;
    std::cout << seeta::aip::Circle({1, 2}, 10) << std::endl;
    std::cout << seeta::aip::Cube({1, 2}, {3, 4}, {5, 6}) << std::endl;

    return 0;
}