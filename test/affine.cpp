//
// Created by kier on 2020/11/14.
//

#include "seeta_aip_affine.h"
#include "seeta_aip_alignment.h"
#include "seeta_aip_image_io.h"

#include "seeta_aip_opencv.h"

int main() {
    std::string path = "a.jpg";

    auto image = seeta::aip::imread(path);

    auto scale_x = 0.5f;
    auto scale_y = 0.5f;
    auto width = image.width() * scale_x;
    auto height = image.height() * scale_y;

    image = seeta::aip::convert(2, SEETA_AIP_FORMAT_U8BGR, image);

    auto resize = seeta::aip::scale(2, image, scale_x, scale_y);

    seeta::aip::imwrite("test_affine.png", resize);

    cv::Mat mat(image.height(), image.width(), CV_8UC(image.channels()), image.data<uint8_t>());

    cv::resize(mat, mat, cv::Size(), scale_x, scale_y);

    cv::imwrite("test_affine_opencv.png", mat);

    return 0;
}

