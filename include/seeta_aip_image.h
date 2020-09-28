//
// Created by kier on 2020/9/27.
//

#ifndef SEETA_AIP_SEETA_AIP_IMAGE_H
#define SEETA_AIP_SEETA_AIP_IMAGE_H

#include "seeta_aip_struct.h"

namespace seeta {
    namespace aip {
        namespace _ {
            template<typename SRC, typename DST,
                    typename=typename std::enable_if<std::is_convertible<SRC, DST>::value>::type>
            inline void cast(int threads, const SRC *src, DST *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { dst[i] = DST(src[i]); }
                } else {
                    for (int i = 0; i < N; ++i) { *dst++ = DST(*src++); }
                }
            }

            template<typename SRC, typename DST,
                    typename=typename std::enable_if<std::is_convertible<SRC, DST>::value>::type>
            inline void cast(int threads, const SRC *src, DST *dst, int N, SRC scale) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, scale, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { dst[i] = DST(scale * src[i]); }
                } else {
                    for (int i = 0; i < N; ++i) { *dst++ = DST(scale * *src++); }
                }
            }

            template<typename DST>
            inline void cast_to(int threads, const void *src, SEETA_AIP_VALUE_TYPE src_type, DST *dst, int N,
                                float data_scale = 1) {
                switch (src_type) {
                    default:
                    case SEETA_AIP_VALUE_VOID:
                        break;
                    case SEETA_AIP_VALUE_BYTE:
                        cast<uint8_t, DST>(threads, reinterpret_cast<const uint8_t *>(src), dst, N);
                        break;
                    case SEETA_AIP_VALUE_INT32:
                        cast<int32_t, DST>(threads, reinterpret_cast<const int32_t *>(src), dst, N);
                        break;
                    case SEETA_AIP_VALUE_FLOAT32:
                        cast<float, DST>(threads, reinterpret_cast<const float *>(src), dst, N, data_scale);
                        break;
                    case SEETA_AIP_VALUE_FLOAT64:
                        cast<double, DST>(threads, reinterpret_cast<const double *>(src), dst, N, data_scale);
                        break;
                }
            }

            inline size_t value_type_width(SEETA_AIP_VALUE_TYPE type) {
                switch (type) {
                    default:
                    case SEETA_AIP_VALUE_VOID:
                        return 0;
                    case SEETA_AIP_VALUE_BYTE:
                        return 1;
                    case SEETA_AIP_VALUE_FLOAT32:
                    case SEETA_AIP_VALUE_INT32:
                        return 4;
                    case SEETA_AIP_VALUE_FLOAT64:
                        return 8;
                }
            }
        }

        inline void cast(int threads,
                         const void *src, SEETA_AIP_VALUE_TYPE src_type,
                         void *dst, SEETA_AIP_VALUE_TYPE dst_type,
                         int N, float data_scale = 1) {
            if (src_type == dst_type) {
                if (src != dst) {
                    std::memcpy(dst, src, _::value_type_width(src_type) * N);
                }
                return;
            }
            switch (dst_type) {
                default:
                case SEETA_AIP_VALUE_VOID:
                    break;
                case SEETA_AIP_VALUE_BYTE:
                    _::cast_to<uint8_t>(threads, src, src_type, reinterpret_cast<uint8_t *>(dst), N, data_scale);
                    break;
                case SEETA_AIP_VALUE_INT32:
                    _::cast_to<int32_t>(threads, src, src_type, reinterpret_cast<int32_t *>(dst), N, data_scale);
                    break;
                case SEETA_AIP_VALUE_FLOAT32:
                    _::cast_to<float>(threads, src, src_type, reinterpret_cast<float *>(dst), N, data_scale);
                    break;
                case SEETA_AIP_VALUE_FLOAT64:
                    _::cast_to<double>(threads, src, src_type, reinterpret_cast<double *>(dst), N, data_scale);
                    break;
            }
        }

        namespace _ {
            inline SEETA_AIP_IMAGE_FORMAT casted_format(SEETA_AIP_IMAGE_FORMAT original_format,
                                                        SEETA_AIP_VALUE_TYPE casted_type) {
                (void) (original_format);
                switch (casted_type) {
                    default:
                        throw seeta::aip::Exception("Unknown value type.");
                    case SEETA_AIP_VALUE_VOID:
                        throw seeta::aip::Exception("There is no image format VOID");
                    case SEETA_AIP_VALUE_BYTE:
                        return SEETA_AIP_FORMAT_U8RAW;
                    case SEETA_AIP_VALUE_INT32:
                        return SEETA_AIP_FORMAT_I32RAW;
                    case SEETA_AIP_VALUE_FLOAT32:
                        return SEETA_AIP_FORMAT_F32RAW;
                    case SEETA_AIP_VALUE_FLOAT64:
                        throw seeta::aip::Exception("There is no image format FLOAT64");
                }
            }

            enum cvt_format {
                NO_CONVERT,
                BGR2RGB,
                BGR2BGRA,
                BGR2RGBA,
                BGRA2BGR,
                BGRA2RGB,
                BGRA2RGBA,
                Y2BGR,
                Y2BGRA,
                BGR2Y,
                BGRA2Y,
                RGB2Y,
                RGBA2Y,
            };

            inline cvt_format serch_u8image_cvt_format(SEETA_AIP_IMAGE_FORMAT src, SEETA_AIP_IMAGE_FORMAT dst) {
                // U8RGB, U8BGR, U8RGBA, U8BGRA, U8Y
                static cvt_format table[5][5] = {
                        {NO_CONVERT, BGR2RGB,    BGR2BGRA,   BGR2RGBA,   RGB2Y},
                        {BGR2RGB,    NO_CONVERT, BGR2RGBA,   BGR2BGRA,   BGR2Y},
                        {BGRA2BGR,   BGRA2RGB,   NO_CONVERT, BGRA2RGBA,  RGBA2Y},
                        {BGRA2RGB,   BGRA2BGR,   BGRA2RGBA,  NO_CONVERT, BGRA2Y},
                        {Y2BGR,      Y2BGR,      Y2BGRA,     Y2BGRA,     NO_CONVERT},
                };
                int i = src - 1001;
                int j = dst - 1001;
                return table[i][j];
            }

            inline void _bgr2rgb(const uint8_t *src, uint8_t *dst) {
                auto tmp = src[0];
                dst[0] = src[2];
                dst[1] = src[1];
                dst[2] = tmp;
            }

            inline void convert_uimage_bgr2rgb(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
                    const auto N3 = N * 3;
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N3, src, dst)
#endif
                    for (int n = 0; n < N3; n += 3) { _bgr2rgb(src + n, dst + n); }
                } else {
                    for (int i = 0; i < N; ++i, src += 3, dst += 3) { _bgr2rgb(src, dst); }
                }
            }

            inline void _bgr2bgra(const uint8_t *src, uint8_t *dst) {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = 0;
            }

            inline void convert_uimage_bgr2bgra(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _bgr2bgra(src + i * 3, dst + i * 4); }
                } else {
                    for (int i = 0; i < N; ++i, src += 3, dst += 4) { _bgr2bgra(src, dst); }
                }
            }

            inline void _bgr2rgba(const uint8_t *src, uint8_t *dst) {
                dst[0] = src[2];
                dst[1] = src[1];
                dst[2] = src[0];
                dst[3] = 0;
            }

            inline void convert_uimage_bgr2rgba(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _bgr2rgba(src + i * 3, dst + i * 4); }
                } else {
                    for (int i = 0; i < N; ++i, src += 3, dst += 4) { _bgr2rgba(src, dst); }
                }
            }

            inline void _bgra2bgr(const uint8_t *src, uint8_t *dst) {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
            }

            inline void convert_uimage_bgra2bgr(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _bgra2bgr(src + i * 4, dst + i * 3); }
                } else {
                    for (int i = 0; i < N; ++i, src += 4, dst += 3) { _bgra2bgr(src, dst); }
                }
            }

            inline void _bgra2rgb(const uint8_t *src, uint8_t *dst) {
                dst[0] = src[2];
                dst[1] = src[1];
                dst[2] = src[0];
            }

            inline void convert_uimage_bgra2rgb(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _bgra2rgb(src + i * 4, dst + i * 3); }
                } else {
                    for (int i = 0; i < N; ++i, src += 4, dst += 3) { _bgra2rgb(src, dst); }
                }
            }

            inline void _bgra2rgba(const uint8_t *src, uint8_t *dst) {
                auto tmp = src[0];
                dst[0] = src[2];
                dst[1] = src[1];
                dst[2] = tmp;
                dst[3] = src[3];
            }

            inline void convert_uimage_bgra2rgba(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
                    const auto N4 = N * 4;
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N4, src, dst)
#endif
                    for (int n = 0; n < N4; n += 4) { _bgra2rgba(src + n, dst + n); }
                } else {
                    for (int i = 0; i < N; ++i, src += 4, dst += 4) { _bgra2rgba(src, dst); }
                }
            }

            inline void _y2bgr(const uint8_t *src, uint8_t *dst) {
                dst[0] = src[0];
                dst[1] = src[0];
                dst[2] = src[0];
            }

            inline void convert_uimage_y2bgr(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _y2bgr(src + i, dst + i * 3); }
                } else {
                    for (int i = 0; i < N; ++i, src += 1, dst += 3) { _y2bgr(src, dst); }
                }
            }

            inline void _y2bgra(const uint8_t *src, uint8_t *dst) {
                dst[0] = src[0];
                dst[1] = src[0];
                dst[2] = src[0];
                dst[3] = 0;
            }

            inline void convert_uimage_y2bgra(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _y2bgra(src + i, dst + i * 4); }
                } else {
                    for (int i = 0; i < N; ++i, src += 1, dst += 4) { _y2bgra(src, dst); }
                }
            }

            inline void _bgr2y(const uint8_t *src, uint8_t *dst) {
                auto B = src[0];
                auto G = src[1];
                auto R = src[2];
                auto Y = (R * uint32_t(19595) + G * uint32_t(38469) + B * uint32_t(7472)) >> 16;
                dst[0] = Y > 255 ? 255 : Y;
            }

            inline void convert_uimage_bgr2y(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _bgr2y(src + i * 3, dst + i); }
                } else {
                    for (int i = 0; i < N; ++i, src += 3, dst += 1) { _bgr2y(src, dst); }
                }
            }

            inline void convert_uimage_bgra2y(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _bgr2y(src + i * 4, dst + i); }
                } else {
                    for (int i = 0; i < N; ++i, src += 4, dst += 1) { _bgr2y(src, dst); }
                }
            }

            inline void _rgb2y(const uint8_t *src, uint8_t *dst) {
                auto B = src[2];
                auto G = src[1];
                auto R = src[0];
                auto Y = (R * uint32_t(19595) + G * uint32_t(38469) + B * uint32_t(7472)) >> 16;
                dst[0] = Y > 255 ? 255 : Y;
            }

            inline void convert_uimage_rgb2y(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _rgb2y(src + i * 3, dst + i); }
                } else {
                    for (int i = 0; i < N; ++i, src += 3, dst += 1) { _rgb2y(src, dst); }
                }
            }

            inline void convert_uimage_rgba2y(int threads, const uint8_t *src, uint8_t *dst, int N) {
                if (threads > 1) {
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads) default(none) shared(N, src, dst)
#endif
                    for (int i = 0; i < N; ++i) { _rgb2y(src + i * 4, dst + i); }
                } else {
                    for (int i = 0; i < N; ++i, src += 4, dst += 1) { _rgb2y(src, dst); }
                }
            }

            inline void convert_uimage(int threads,
                                       cvt_format cvt_code, const void *src, void *dst,
                                       int pixel_number) {
                static decltype(convert_uimage_bgr2rgb) *converter[] = {
                        nullptr,
                        convert_uimage_bgr2rgb,
                        convert_uimage_bgr2bgra,
                        convert_uimage_bgr2rgba,
                        convert_uimage_bgra2bgr,
                        convert_uimage_bgra2rgb,
                        convert_uimage_bgra2rgba,
                        convert_uimage_y2bgr,
                        convert_uimage_y2bgra,
                        convert_uimage_bgr2y,
                        convert_uimage_bgra2y,
                        convert_uimage_rgb2y,
                        convert_uimage_rgba2y,
                };
                auto func = converter[cvt_code];
                if (!func) return;
                func(threads,
                     reinterpret_cast<const uint8_t *>(src),
                     reinterpret_cast<uint8_t *>(dst),
                     pixel_number);
            }

            inline void convert_u8image(int threads,
                                        const void *src_data, void *dst_data,
                                        int src_channels, int dst_channels,
                                        SEETA_AIP_IMAGE_FORMAT src_format,
                                        SEETA_AIP_IMAGE_FORMAT dst_format,
                                        int pixel_number) {
                if ((src_format < 1000 || dst_format < 1000) && src_channels != dst_channels) {
                    throw seeta::aip::Exception("Can not convert mismatch channel images with format U8Raw");
                }
                if (src_format < 1000 || dst_format < 1000) {
                    // raw no need to channels swap
                    if (src_data != dst_data) {
                        std::memcpy(dst_data, src_data, pixel_number * src_channels);
                    }
                    return;
                }
                auto cvt_code = _::serch_u8image_cvt_format(src_format, dst_format);
                if (cvt_code != 0) {
                    _::convert_uimage(threads, cvt_code, src_data, dst_data, pixel_number);
                } else {
                    // no swap, so copy input to output
                    if (src_data != dst_data) {
                        std::memcpy(dst_data, src_data, pixel_number * src_channels);
                    }
                }
            }

            inline void convert_f32image(int threads,
                                         const void *src_data, void *dst_data,
                                         int src_channels, int dst_channels,
                                         SEETA_AIP_IMAGE_FORMAT src_format,
                                         SEETA_AIP_IMAGE_FORMAT dst_format,
                                         int pixel_number) {
                if (src_format != SEETA_AIP_FORMAT_F32RAW || dst_format != SEETA_AIP_FORMAT_F32RAW) {
                    throw seeta::aip::Exception("Float image format only support F32Raw for now.");
                }
                if (src_channels != dst_channels) {
                    throw seeta::aip::Exception("Can not convert mismatch channel images with format F32Raw");
                }
                if (src_data != dst_data) {
                    std::memcpy(dst_data, src_data, pixel_number * src_channels * 4);
                }
            }

            inline void convert_i32image(int threads,
                                         const void *src_data, void *dst_data,
                                         int src_channels, int dst_channels,
                                         SEETA_AIP_IMAGE_FORMAT src_format,
                                         SEETA_AIP_IMAGE_FORMAT dst_format,
                                         int pixel_number) {
                if (src_format != SEETA_AIP_FORMAT_I32RAW || dst_format != SEETA_AIP_FORMAT_I32RAW) {
                    throw seeta::aip::Exception("Integer image format only support I32Raw for now.");
                }
                if (src_channels != dst_channels) {
                    throw seeta::aip::Exception("Can not convert mismatch channel images with format I32Raw");
                }
                if (src_data != dst_data) {
                    std::memcpy(dst_data, src_data, pixel_number * src_channels * 4);
                }
            }
        }

        inline void convert(int threads,
                            SeetaAIPImageData src, SeetaAIPImageData dst,
                            float data_scale = 255.0) {
            auto SRC_N = src.number * src.height * src.width;
            auto DST_N = dst.number * dst.height * dst.width;
            if (SRC_N != DST_N) {
                throw seeta::aip::Exception("Convert image pixels' number must be equal.");
            }
            auto src_format = SEETA_AIP_IMAGE_FORMAT(src.format);
            auto dst_format = SEETA_AIP_IMAGE_FORMAT(dst.format);
            auto src_type = seeta::aip::ImageData::GetType(src_format);
            auto dst_type = seeta::aip::ImageData::GetType(dst_format);
            auto src_data = src.data;
            auto dst_data = dst.data;
            auto src_channels = seeta::aip::ImageData::GetChannels(src_format, src.channels);
            auto dst_channels = seeta::aip::ImageData::GetChannels(dst_format, dst.channels);
            // step1. convert dtype
            std::shared_ptr<char> buffer;
            if (src_type != dst_type) {
                if (src_channels == dst_channels) {
                    // for output channels same with input channels
                    cast(threads, src.data, src_type, dst.data, dst_type, SRC_N * src_channels, data_scale);
                    src_type = dst_type;
                    src_data = dst.data;
                    src_format = _::casted_format(src_format, dst_type);
                } else {
                    // use buffer to convert data
                    buffer.reset(new char[SRC_N * src_channels * _::value_type_width(dst_type)],
                                 std::default_delete<char[]>());
                    cast(threads, src.data, src_type, buffer.get(), dst_type, SRC_N * src_channels, data_scale);
                    src_type = dst_type;
                    src_data = buffer.get();
                    src_format = _::casted_format(src_format, dst_type);
                }
            }
            // step2. swap channels
            switch (dst_type) {
                default:
                    break;
                case SEETA_AIP_VALUE_BYTE:
                    _::convert_u8image(threads, src_data, dst_data, src_channels, dst_channels,
                                       src_format, dst_format, int(SRC_N));
                    break;
                case SEETA_AIP_VALUE_INT32:
                    _::convert_i32image(threads, src_data, dst_data, src_channels, dst_channels,
                                        src_format, dst_format, int(SRC_N));
                    break;
                case SEETA_AIP_VALUE_FLOAT32:
                    _::convert_f32image(threads, src_data, dst_data, src_channels, dst_channels,
                                        src_format, dst_format, int(SRC_N));
                    break;
            }
        }

        /**
         * Use `threads` to convert image to `format`.
         * @param threads using threads(OpenMP)
         * @param format wanted format
         * @param image original image
         * @param data_scale used when convert float value to wanted format. the image value while multiply `data_scale`.
         * @return
         */
        inline seeta::aip::ImageData convert(int threads,
                            SEETA_AIP_IMAGE_FORMAT format,
                            SeetaAIPImageData image,
                            float data_scale = 255.0) {
            seeta::aip::ImageData converted(format,
                                            image.number,
                                            image.width,
                                            image.height,
                                            seeta::aip::ImageData::GetChannels(format, image.channels));
            convert(threads, image, SeetaAIPImageData(converted), data_scale);
            return converted;
        }

        /**
         * Use `threads` to convert image to `format`.
         * @param threads using threads(OpenMP)
         * @param format wanted format
         * @param image original image
         * @param data_scale used when convert float value to wanted format. the image value while multiply `data_scale`.
         * @return
         */
        inline seeta::aip::ImageData convert(int threads,
                                             SEETA_AIP_IMAGE_FORMAT format,
                                             const seeta::aip::ImageData &image,
                                             float data_scale = 255.0) {
            return convert(threads, format, SeetaAIPImageData(image), data_scale);
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_IMAGE_H
