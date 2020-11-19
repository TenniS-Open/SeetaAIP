//
// Created by kier on 2020/11/14.
//

#ifndef SEETA_AIP_SEETA_AIP_AFFINE_H
#define SEETA_AIP_SEETA_AIP_AFFINE_H

#include "seeta_aip_graphics2d.h"
#include "seeta_aip_struct.h"

#include <iostream>

namespace seeta {
    namespace aip {
        namespace _ {
            /*
             * +-+-+
             * |0|1|
             * +-+-+
             * |2|3|
             * +-+-+
             */
            struct LineSampleRatio {
                float r0 = 0;
                float r1 = 0;
                float r2 = 0;
                float r3 = 0;
            };

            /**
             * return if need to sample
             * @param ratio
             * @param x
             * @param y
             * @param width
             * @param height
             * @return
             */
            bool GetSampleRatio(
                    LineSampleRatio *ratio,
                    float x, float y, uint32_t width, uint32_t height) {
                *ratio = LineSampleRatio();
                if (x < -1 || x >= float(width)
                    || y < -1 || y >= float(height)) {
                    return false;
                }

                if (x < 0) {
                    ratio->r1 = ratio->r3 = 1 + x;
                } else if (x >= float(width - 1)) {
                    ratio->r0 = ratio->r2 = float(width + 1) - x;
                } else {
                    auto left = int32_t(floorf(x));
                    auto right = left + 1;
                    ratio->r0 = ratio->r2 = right - x;
                    ratio->r1 = ratio->r3 = x - left;
                }

                if (y < 0) {
                    auto r23 = 1 + y;
                    ratio->r2 *= r23;
                    ratio->r3 *= r23;
                    ratio->r0 = ratio->r1 = 0;
                } else if (y >= float(height - 1)) {
                    auto r01 = float(height + 1) - y;
                    ratio->r0 *= r01;
                    ratio->r1 *= r01;
                    ratio->r2 = ratio->r3 = 0;
                } else {
                    auto top = int32_t(floorf(y));
                    auto bottom = top + 1;
                    auto r01 = bottom - y;
                    auto r23 = y - top;
                    ratio->r0 *= r01;
                    ratio->r1 *= r01;
                    ratio->r2 *= r23;
                    ratio->r3 *= r23;
                }

                return true;
            }

            template <typename T, typename K>
            static inline T clamp_to(T min, T max, K value) {
                return T(std::max(K(min), std::min(K(max), value)));
            }
        }

        static inline void sample_uint8_pixel(
                SeetaAIPImageData image,
                float x, float y, uint8_t *out,
                uint32_t out_shift) {
            auto N = image.number;
            auto H = image.height;
            auto W = image.width;
            auto C = image.channels;

            _::LineSampleRatio ratio;
            bool need_sample = _::GetSampleRatio(&ratio, x, y, W, H);
            if (!need_sample) {
                for (decltype(N) n = 0; n < N; ++n) {
                    std::memset(out, 0, C);
                    out += out_shift;
                }
                return;
            }

            auto left = int32_t(floorf(x));
            auto top = int32_t(floorf(y));

            auto p0 = &reinterpret_cast<uint8_t *>(image.data)[((top * W) + left) * C];
            auto next_image_shift = H * W * C;
            auto next_line_shift = W * C;
            for (decltype(N) n = 0; n < N; ++n) {
                auto p1 = p0 + C;
                auto p2 = p0 + next_line_shift;
                auto p3 = p2 + C;
                for (decltype(C) c = 0; c < C; ++c) {
                    auto p = (ratio.r0 == 0 ? 0 : float(p0[c]) * ratio.r0) +
                            (ratio.r1 == 0 ? 0 : float(p1[c]) * ratio.r1) +
                            (ratio.r2 == 0 ? 0 : float(p2[c]) * ratio.r2) +
                            (ratio.r3 == 0 ? 0 : float(p3[c]) * ratio.r3);
                    out[c] = _::clamp_to<uint8_t>(0, 255, p);

                }
                p0 += next_image_shift;
                out += out_shift;
            }
        }

        static inline void sample_uint8_pixel(
                SeetaAIPImageData image,
                int x, int y, uint8_t *out,
                uint32_t out_shift) {
            auto N = image.number;
            auto H = image.height;
            auto W = image.width;
            auto C = image.channels;

            if (x < 0 || x >= W || y < 0 || y >= H) {
                for (decltype(N) n = 0; n < N; ++n) {
                    std::memset(out, 0, C);
                    out += out_shift;
                }
                return;
            }

            auto p0 = &reinterpret_cast<uint8_t *>(image.data)[((y * W) + x) * C];
            auto next_image_shift = H * W * C;
            auto next_line_shift = W * C;
            for (decltype(N) n = 0; n < N; ++n) {
                auto p1 = p0 + C;
                auto p2 = p0 + next_line_shift;
                auto p3 = p2 + C;
                for (decltype(C) c = 0; c < C; ++c) {
                    out[c] = p0[c];
                }
                p0 += next_image_shift;
                out += out_shift;
            }
        }

        /**
         * apply affine on given image
         * @param threads using computing threads. must enable OpenMP
         * @param M affine 3x3 matrix
         * @param image source image
         * @param x sample dest image start x
         * @param y sample dest image start y
         * @param width sample dest image width
         * @param height sample dest image height
         * @return image with same format with source image, shape `width x height`
         * @note src = M * dst.
         */
        static seeta::aip::ImageData affine_sample2d(
                int threads, const float *M, SeetaAIPImageData image,
                int x, int y, int width, int height) {
            auto format = SEETA_AIP_IMAGE_FORMAT(image.format);
            auto type = ImageData::GetType(format);
            auto channels = ImageData::GetChannels(format, image.channels);
            if (type != SEETA_AIP_VALUE_BYTE) {
                throw Exception(
                        std::string("AIP image affine only support BYTE type, got ") + type_string(format));
            }
            image.channels = channels;  // fix channels if not mismatched.
            auto src = image;
            bool is_chw = (format & 0xffff0000) == 0x80000;
            if (is_chw) {
                // trans image to hwc format to sample
                src.format = SEETA_AIP_FORMAT_U8RAW;
                src.number = src.number * src.channels;
                src.channels = 1;
            }
            // loop to sample
            seeta::aip::ImageData dst(format, image.number,
                                      width, height, image.channels);
            auto out = dst.data<uint8_t>();
            auto out_shift = is_chw
                    ? dst.width() * dst.height()
                    : dst.width() * dst.height() * dst.channels();

            auto H = int(dst.height());
            auto W = int(dst.width());
            auto C = int(src.channels);

            auto top = y;
            auto bottom = y + H;
            auto left = x;
            auto right = x + W;

            Trans2D<float> T;
            std::memcpy(T.data(), M, 9 * sizeof(float));

#ifdef _OPENMP
#pragma omp parallel for num_threads(threads)
#endif
            for (decltype(bottom) j = top; j < bottom; ++j) {
                for (decltype(right) i = left; i < right; ++i) {
                    Vec3D<float> p(i, j);
                    auto local_out = &out[(j * W + i) * C];
                    p = transform(T, p);
                    sample_uint8_pixel(src, p.x(), p.y(), local_out, out_shift);
                }
            }

            return dst;
        }

        /**
         *
         * @param threads using computing threads. must enable OpenMP
         * @param image source image
         * @param width resized image width
         * @param height resized image height
         * @return resize image `width x height`
         */
        static seeta::aip::ImageData resize(
                int threads, SeetaAIPImageData image, int width, int height) {
            float scale_x = static_cast<float>(image.width) / width;
            float scale_y = static_cast<float>(image.height) / height;
            float bias_x = scale_x / 2 - 0.5f;
            float bias_y = scale_y / 2 - 0.5f;

            float M[] = {
                    scale_x, 0, bias_x,
                    0, scale_y, bias_y,
                    0, 0, 1,
            };

            return affine_sample2d(threads, M, image, 0, 0, width, height);
        }

        /**
         *
         * @param threads using computing threads. must enable OpenMP
         * @param image source image
         * @param scale_x resized image width
         * @param scale_y resized image height
         * @return resize image `image.width*scale_x x image.height*scale_y`
         */
        static seeta::aip::ImageData scale(
                int threads, SeetaAIPImageData image, float scale_x, float scale_y) {
            float bias_x = 0.5f / scale_x - 0.5f;
            float bias_y = 0.5f / scale_y - 0.5f;

            float M[] = {
                    1 / scale_x, 0, bias_x,
                    0, 1 / scale_y, bias_y,
                    0, 0, 1,
            };

            auto width = int(roundf(image.width * scale_x));
            auto height = int(roundf(image.height * scale_y));

            return affine_sample2d(threads, M, image, 0, 0, width, height);
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_AFFINE_H
