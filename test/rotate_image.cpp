//
// Created by kier on 2020/6/29.
//

#include "seeta_aip_engine.h"
#include "seeta_aip_opencv.h"

#include <iostream>

int main() {
    using namespace seeta::aip;

    Instance instance("../lib/rotate_image", "cpu", {});
    instance.set("angle", -270);

    try {
        seeta::aip::cv::ImageData image = ::cv::imread("1.png");

        auto result = instance.forward(0, image);
        auto output = result.images.data[0];

        ::cv::Mat show(output.height, output.width, CV_8UC(output.channels), output.data);

        ::cv::imshow("show", show);
        ::cv::waitKey();
    } catch (const std::exception &e) {
        std::cerr << "Exception about: " << e.what() << std::endl;
    }


    instance.dispose();

    return 0;
}