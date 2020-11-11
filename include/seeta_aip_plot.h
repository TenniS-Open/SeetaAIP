//
// Created by seeta on 2020/11/10.
//

#ifndef SEETA_AIP_SEETA_AIP_PLOT_H
#define SEETA_AIP_SEETA_AIP_PLOT_H

#include "seeta_aip_struct.h"
#include <cmath>

namespace seeta {
    namespace aip {
        namespace plot {
            class Color {
            public:
                uint8_t c1 = 0;
                uint8_t c2 = 0;
                uint8_t c3 = 0;
                uint8_t c4 = 0;

                Color() = default;

                explicit Color(uint8_t y)
                        : c1(y), c2(y), c3(y) {}

                Color(uint8_t c1, uint8_t c2, uint8_t c3)
                        : c1(c1), c2(c2), c3(c3) {}

                Color(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
                        : c1(c1), c2(c2), c3(c3), c4(c4) {}
            };

            class PutSetting {
            public:
                int32_t x;
                int32_t y;
                int32_t alpha;  //< [0, 255]
            };

            inline void put_uint8_pixel(SeetaAIPImageData image,
                                        int32_t x, int32_t y,
                                        const Color &color) {
                auto data = reinterpret_cast<uint8_t *>(image.data);
                auto pixel = &data[(y * image.width + x) * image.channels];
                int32_t alpha = color.c4;
                switch (image.channels) {
                    case 4: // alpha never changed
                    case 3:
                        pixel[2] = uint8_t((alpha * pixel[2] + (255 - alpha) * color.c3) / 255);
                    case 2:
                        pixel[1] = uint8_t((alpha * pixel[1] + (255 - alpha) * color.c2) / 255);
                    case 1:
                        pixel[0] = uint8_t((alpha * pixel[0] + (255 - alpha) * color.c1) / 255);
                }
            }

            inline std::vector<PutSetting> _prepare_rectangle_solid(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &p1, const SeetaAIPPoint &p2,
                    int line_width) {
                // check plot area
                if (line_width <= 0) return {};
                auto area_width = p2.x - p1.x;
                auto area_height = p2.y - p1.y;
                if (area_height < 0 || area_width < 0) return {};
                auto half = (line_width - 1) / 2;
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(lroundf(p1.y - half));
                auto bottom = int32_t(lroundf(top + area_height + float(line_width)));    // not included
                auto left = int32_t(lroundf(p1.x - half));
                auto right = int32_t(lroundf(left + area_width + float(line_width)));

                // compute not plot area
                auto c_top = top + line_width;
                auto c_bottom = bottom - line_width;
                auto c_left = left + line_width;
                auto c_right = right - line_width;
                if (c_bottom < c_top) c_bottom = c_top;
                if (c_right < c_left) c_right = c_left;

                // check all pixel in graph
                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = height;
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = width;

                std::vector<PutSetting> setting;
                for (decltype(top) y = top; y < bottom; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        // not plot in center area
                        if (y >= c_top && y < c_bottom
                            && x >= c_left && x < c_right)
                            continue;
                        // plot pixel
                        setting.emplace_back(PutSetting({x, y, 0}));
                    }
                }

                return setting;
            }

            inline std::vector<PutSetting> _prepare_fill_rectangle_solid(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &p1, const SeetaAIPPoint &p2) {
                // check plot area
                auto area_width = p2.x - p1.x;
                auto area_height = p2.y - p1.y;
                if (area_height < 0 || area_width < 0) return {};
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(lroundf(p1.y));
                auto bottom = int32_t(lroundf(top + area_height + 1));    // not included
                auto left = int32_t(lroundf(p1.x));
                auto right = int32_t(lroundf(left + area_width + 1));

                // check all pixel in graph
                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = height;
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = width;

                std::vector<PutSetting> setting;
                for (decltype(top) y = top; y < bottom; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        // plot pixel
                        setting.emplace_back(PutSetting({x, y, 0}));
                    }
                }

                return setting;
            }

            inline float _pow2(float x) { return x * x; }

            inline float _distance(const SeetaAIPPoint &a, const SeetaAIPPoint &b) {
                return sqrtf(_pow2(a.x - b.x) + _pow2(a.y - b.y));
            }

            inline std::vector<PutSetting> _prepare_fill_circle(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &center, int radius) {
                // check plot area
                auto area_width = 2 * radius;
                auto area_height = 2 * radius;
                if (area_height < 0 || area_width < 0) return {};
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(lroundf(center.y - radius));
                auto bottom = int32_t(lroundf(top + area_height + 1));    // not included
                auto left = int32_t(lroundf(center.x - radius));
                auto right = int32_t(lroundf(left + area_width + 1));

                // check all pixel in graph
                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = height;
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = width;

                std::vector<PutSetting> setting;
                for (decltype(top) y = top; y < bottom; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        // plot pixel
                        auto d = _distance(center, {float(x), float(y)});
                        auto part = radius + 0.5f - d;
                        if (part < 0) continue;
                        if (part > 1) {
                            setting.emplace_back(PutSetting({x, y, 0}));
                        } else {
                            setting.emplace_back(PutSetting({x, y, int32_t((1 - part) * 255)}));
                        }
                    }
                }

                return setting;
            }

            inline std::vector<PutSetting> _prepare_circle(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &center, int radius,
                    int line_width) {
                // check plot area
                if (line_width <= 0) return {};
                auto area_width = 2 * radius;
                auto area_height = 2 * radius;
                if (area_height < 0 || area_width < 0) return {};
                auto half = (line_width - 1) / 2;
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(lroundf(center.y - radius - half));
                auto bottom = int32_t(lroundf(top + area_height + float(line_width)));    // not included
                auto left = int32_t(lroundf(center.x - radius - half));
                auto right = int32_t(lroundf(left + area_width + float(line_width)));

                // check all pixel in graph
                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = height;
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = width;

                std::vector<PutSetting> setting;
                for (decltype(top) y = top; y < bottom; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        // plot pixel
                        auto d = _distance(center, {float(x), float(y)});
                        // ignore out point
                        if (d > radius + half + 0.5f) continue;
                        if (d < radius - line_width + half - 0.5f) continue;
                        // plot edge point
                        float alpha = 0;
                        if (d > radius + half - 0.5f) {
                            alpha = d - (radius + half - 0.5f);
                        } else if (d < radius - line_width + half + 0.5f) {
                            alpha = (radius - line_width + half + 0.5f) - d;
                        }
                        // not plot in center area
                        setting.emplace_back(PutSetting({x, y, int32_t(alpha * 255)}));
                    }
                }

                return setting;
            }

            /**
             *
             * @param image BYTE type HWC format image.
             * @param p1 top-left point
             * @param p2 right-bottom point
             * @param color color according to image format
             * @param line_width line with, -1 (or negative value) for fill.
             */
            inline void rectangle(SeetaAIPImageData image,
                                  const SeetaAIPPoint &p1, const SeetaAIPPoint &p2,
                                  const Color &color, int line_width = 3) {
                auto format = SEETA_AIP_IMAGE_FORMAT(image.format);
                auto type = ImageData::GetType(format);
                auto channels = ImageData::GetChannels(format, image.channels);
                if ((format & 0xffff0000) == 0x80000) {
                    throw Exception(
                            std::string("AIP image plot only support HWC format, got ") + format_string(format));
                }
                if (type != SEETA_AIP_VALUE_BYTE) {
                    throw Exception(
                            std::string("AIP image plot only support BYTE type, got ") + type_string(format));
                }
                image.channels = channels;  // fix channels if not mismatched.
                // compute all pixel to draw
                auto setting = line_width < 0
                               ? _prepare_fill_rectangle_solid(image.width, image.height, p1, p2)
                               : _prepare_rectangle_solid(image.width, image.height, p1, p2, line_width);
                // plot each point
                auto c = color;
                // auto alpha = c.c4;
                for (auto &p : setting) {
                    // c.c4 = 255 - uint8_t(((255 - alpha) * (255 - p.alpha)) / 255);
                    put_uint8_pixel(image, p.x, p.y, c);
                }
            }

            /**
             *
             * @param image BYTE type HWC format image.
             * @param center
             * @param radius
             * @param color color according to image format
             * @param line_width line with, -1 (or negative value) for fill.
             */
            inline void circle(SeetaAIPImageData image,
                               const SeetaAIPPoint &center, int radius,
                               const Color &color, int line_width = 3) {
                auto format = SEETA_AIP_IMAGE_FORMAT(image.format);
                auto type = ImageData::GetType(format);
                auto channels = ImageData::GetChannels(format, image.channels);
                if ((format & 0xffff0000) == 0x80000) {
                    throw Exception(
                            std::string("AIP image plot only support HWC format, got ") + format_string(format));
                }
                if (type != SEETA_AIP_VALUE_BYTE) {
                    throw Exception(
                            std::string("AIP image plot only support BYTE type, got ") + type_string(format));
                }
                image.channels = channels;  // fix channels if not mismatched.
                // compute all pixel to draw
                auto setting = line_width < 0
                               ? _prepare_fill_circle(image.width, image.height, center, radius)
                               : _prepare_circle(image.width, image.height, center, radius, line_width);
                // plot each point
                auto c = color;
                auto alpha = c.c4;
                for (auto &p : setting) {
                    c.c4 = 255 - uint8_t(((255 - alpha) * (255 - p.alpha)) / 255);
                    put_uint8_pixel(image, p.x, p.y, c);
                }
            }

            /**
             *
             * @param image BYTE type HWC format image.
             * @param p1 start point
             * @param p2 end point
             * @param color color according to image format
             * @param line_width line with, -1 (or negative value) for fill.
             */
            inline void line(SeetaAIPImageData image,
                             const SeetaAIPPoint &p1, const SeetaAIPPoint &p2,
                             const Color &color, int line_width = 3);
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_PLOT_H