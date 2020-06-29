//
// Created by kier on 2020/6/28.
//

#include "seeta_aip_package_v2.h"
#include "seeta_aip_shape.h"

#include <iostream>

class MyPackage : public seeta::aip::PackageV2 {
public:
    using self = MyPackage;

    int angle = 90;

    MyPackage() {
        bind_property("angle", angle,
                [](int a){ return a >= 0 ? a % 90 == 0 : -a % 90 == 0; },
                "angle must be multiple of 90");
    }

    void create(const SeetaAIPDevice &device,
                const std::vector<std::string> &models,
                const std::vector<SeetaAIPObject> &objects) override {
        if (models.size() != 0) throw seeta::aip::Exception(SEETA_AIP_ERROR_MODEL_MISMATCH);
    }

    void free() override {
    }

    void reset() override {
    }

    template <size_t N>
    static inline void copy_pixel(uint8_t *dst, const uint8_t *src) {
        class Block {
        public:
            uint8_t data[N];
        };
        reinterpret_cast<Block *>(dst)[0] = reinterpret_cast<const Block*>(src)[0];
    }

    static void copy_pixel(uint8_t *dst, const uint8_t *src, size_t N) {
        switch (N) {
            default:
                std::memcpy(dst, src, N);
                break;
            case 0:
                break;
#define COPY_PIXEL_N(N) case N: copy_pixel<N>(dst, src); break;
            COPY_PIXEL_N(1)
            COPY_PIXEL_N(2)
            COPY_PIXEL_N(3)
            COPY_PIXEL_N(4)
            COPY_PIXEL_N(5)
            COPY_PIXEL_N(6)
            COPY_PIXEL_N(7)
            COPY_PIXEL_N(8)
            COPY_PIXEL_N(9)
            COPY_PIXEL_N(10)
            COPY_PIXEL_N(11)
            COPY_PIXEL_N(12)
            COPY_PIXEL_N(13)
            COPY_PIXEL_N(14)
            COPY_PIXEL_N(15)
            COPY_PIXEL_N(16)
#undef COPY_PIXEL_N
        }
    }

    template <int A>
    void rotate(const SeetaAIPImageData &src, seeta::aip::ImageData &dst);

    template <>
    void rotate<0>(const SeetaAIPImageData &src, seeta::aip::ImageData &dst) {
        dst = seeta::aip::ImageData(SEETA_AIP_IMAGE_FORMAT(src.format),
                src.number, src.width, src.height, src.channels, src.data);
    }

    template <>
    void rotate<90>(const SeetaAIPImageData &src, seeta::aip::ImageData &dst) {
        dst = seeta::aip::ImageData(SEETA_AIP_IMAGE_FORMAT(src.format),
                                    src.number, src.height, src.width, src.channels);
        for (int n = 0; n < src.number; ++n) {
            for (int h = 0; h < src.height; ++h) {
                auto dst_channels = src.channels;

                auto dst_width = src.height;
                auto dst_height = src.width;

                auto dst_h = dst_height - 1;
                auto dst_w = h;
                auto dst_step = -(int64_t)dst_width * dst_channels;

                auto src_step = src.channels;
                auto src_data = reinterpret_cast<const uint8_t *>(src.data) + (n * src.height + h) * src.width * src.channels;
                auto dst_data = reinterpret_cast<uint8_t *>(dst.data()) + ((n * dst_height + dst_h) * dst_width + dst_w) * dst_channels;

                for (int w = 0; w < src.width; ++w) {
                    copy_pixel(dst_data, src_data, src.channels);

                    src_data += src_step;
                    dst_data += dst_step;
                }
            }
        }
    }

    template <>
    void rotate<180>(const SeetaAIPImageData &src, seeta::aip::ImageData &dst) {
        dst = seeta::aip::ImageData(SEETA_AIP_IMAGE_FORMAT(src.format),
                                    src.number, src.width, src.height, src.channels);
        for (int n = 0; n < src.number; ++n) {
            for (int h = 0; h < src.height; ++h) {
                auto dst_channels = src.channels;

                auto dst_width = src.width;
                auto dst_height = src.height;

                auto dst_h = dst_height - 1 - h;
                auto dst_w = dst_width - 1;
                auto dst_step = -(int64_t)dst_channels;

                auto src_step = src.channels;
                auto src_data = reinterpret_cast<const uint8_t *>(src.data) + (n * src.height + h) * src.width * src.channels;
                auto dst_data = reinterpret_cast<uint8_t *>(dst.data()) + ((n * dst_height + dst_h) * dst_width + dst_w) * dst_channels;

                for (int w = 0; w < src.width; ++w) {
                    copy_pixel(dst_data, src_data, src.channels);

                    src_data += src_step;
                    dst_data += dst_step;
                }
            }
        }
    }

    template <>
    void rotate<270>(const SeetaAIPImageData &src, seeta::aip::ImageData &dst) {
        dst = seeta::aip::ImageData(SEETA_AIP_IMAGE_FORMAT(src.format),
                                    src.number, src.height, src.width, src.channels);
        for (int n = 0; n < src.number; ++n) {
            for (int h = 0; h < src.height; ++h) {
                auto dst_channels = src.channels;

                auto dst_width = src.height;
                auto dst_height = src.width;

                auto dst_h = 0;
                auto dst_w = dst_width - 1 - h;
                auto dst_step = dst_width * dst_channels;

                auto src_step = src.channels;
                auto src_data = reinterpret_cast<const uint8_t *>(src.data) + (n * src.height + h) * src.width * src.channels;
                auto dst_data = reinterpret_cast<uint8_t *>(dst.data()) + ((n * dst_height + dst_h) * dst_width + dst_w) * dst_channels;

                for (int w = 0; w < src.width; ++w) {
                    copy_pixel(dst_data, src_data, src.channels);

                    src_data += src_step;
                    dst_data += dst_step;
                }
            }
        }
    }

    void forward_0(const std::vector<SeetaAIPImageData> &images,
                   const std::vector<SeetaAIPObject> &objects) {
        if (images.size() != 1) throw seeta::aip::Exception(SEETA_AIP_ERROR_MISMATCH_REQUIRED_INPUT_IMAGE);
        auto &image = images[0];
        result.images.resize(1);
        auto fix_angle = angle >= 0 ? angle % 360 : 360 - (-angle) % 360;
        switch (fix_angle) {
            default:
                throw seeta::aip::Exception("Can not rotate to " + std::to_string(angle));
            case 90:
                rotate<90>(image, result.images[0]);
                break;
            case 180:
                rotate<180>(image, result.images[0]);
                break;
            case 270:
                rotate<270>(image, result.images[0]);
                break;
            case 0:
                rotate<0>(image, result.images[0]);
                break;
        }
    }

    void forward(
            uint32_t method_id,
            const std::vector<SeetaAIPImageData> &images,
            const std::vector<SeetaAIPObject> &objects) override {
        switch (method_id) {
            default:
                throw seeta::aip::Exception(SEETA_AIP_ERROR_METHOD_ID_OUT_OF_RANGE);
            case 0:
                forward_0(images, objects);
                break;
        }
    }
};

int32_t seeta_aip_load(struct SeetaAIP *aip, uint32_t size) {
    CHECK_AIP_SIZE(aip, size)
    static const char *support[] = {"cpu", "gpu", nullptr};
    seeta::aip::setup_aip_header(*aip,
                                 "RotateImage",
                                 "Rotate image in multiply of 90 degree",
                                 "rotate_image",
                                 "v1",
                                 "0.0.1",
                                 support);
    seeta::aip::setup_aip_entry<MyPackage>(*aip);
    return 0;
}