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

    std::cout << "Load image: [" << image.width() << ", " << image.height() << ", " << image.channels() << "]" << std::endl;

    auto scale_x = 0.5f;
    auto scale_y = 0.5f;
    auto width = image.width() * scale_x;
    auto height = image.height() * scale_y;

    image = seeta::aip::convert(2, SEETA_AIP_FORMAT_U8BGR, image);

    seeta::aip::imwrite("rotate_left_90.png", seeta::aip::rotate_left_90(2, image));
    seeta::aip::imwrite("rotate_180.png", seeta::aip::rotate_180(2, image));
    seeta::aip::imwrite("rotate_right_90.png", seeta::aip::rotate_right_90(2, image));

    {
        float degree = 45;
        using namespace seeta::aip;
        auto M = affine::identity<float>();
        stack(M, affine::translate<float>(-float(image.width() / 2), -float(image.height() / 2)));
        stack(M, affine::rotate<float>(degree));
        stack(M, affine::translate<float>(image.width() / 2, image.height() / 2));
        M = affine::inverse(M);
        auto tmp =
                seeta::aip::affine_sample2d(
                        2, M.data(), image, 0, 0, image.width(), image.height());
        seeta::aip::imwrite(
                "rotate_right_45.png",tmp);
    }

    seeta::aip::imwrite("test_affine.png", seeta::aip::scale(2, image, scale_x, scale_y));

    cv::Mat mat(image.height(), image.width(), CV_8UC(image.channels()), image.data<uint8_t>());

    cv::resize(mat, mat, cv::Size(), scale_x, scale_y);

    cv::imwrite("test_affine_opencv.png", mat);

    return 0;
}

