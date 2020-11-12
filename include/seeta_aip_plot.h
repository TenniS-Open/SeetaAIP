//
// Created by seeta on 2020/11/10.
//

#ifndef SEETA_AIP_SEETA_AIP_PLOT_H
#define SEETA_AIP_SEETA_AIP_PLOT_H

#include "seeta_aip_struct.h"
#include <cmath>
#include <climits>
#include <cfloat>

namespace seeta {
    namespace aip {
        namespace plot {
            class Color {
            public:
                uint8_t c1 = 0xff;
                uint8_t c2 = 0xff;
                uint8_t c3 = 0xff;
                uint8_t c4 = 0xff;

                Color() = default;

                Color(uint32_t c123)
                        : c1((c123 >> 16) & 0x000000ff)
                        , c2((c123 >> 8) & 0x000000ff)
                        , c3((c123) & 0x000000ff)
                        , c4(0xff) {}

                Color(uint8_t c1, uint8_t c2, uint8_t c3)
                        : c1(c1), c2(c2), c3(c3) {}

                Color(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t alpha)
                        : c1(c1), c2(c2), c3(c3), c4(alpha) {}

                Color(const Color &color, uint8_t alpha)
                        : c1(color.c1), c2(color.c2), c3(color.c3), c4(alpha) {}
            };

            class PutSetting {
            public:
                int32_t x;
                int32_t y;
                int32_t alpha;  //< [0, 255], 255 means full color plot
            };

            template <typename T>
            inline T _clamp(T x, T min, T max) {
                return std::max(min, std::min(x, max));
            }

            inline void put_uint8_pixel(SeetaAIPImageData image,
                                        int32_t x, int32_t y,
                                        const Color &color) {
                auto data = reinterpret_cast<uint8_t *>(image.data);
                auto pixel = &data[(y * image.width + x) * image.channels];
                int32_t alpha = color.c4;
                switch (image.channels) {
                    case 4:
                        pixel[3] = uint8_t(_clamp(pixel[3] + alpha * (255 - pixel[3]) / 255, 0, 255));
                    case 3:
                        pixel[2] = uint8_t(((255 - alpha) * pixel[2] + alpha * color.c3) / 255);
                    case 2:
                        pixel[1] = uint8_t(((255 - alpha) * pixel[1] + alpha * color.c2) / 255);
                    case 1:
                        pixel[0] = uint8_t(((255 - alpha) * pixel[0] + alpha * color.c1) / 255);
                }
            }

            static inline std::vector<PutSetting> _prepare_rectangle_solid(
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
                auto bottom = int32_t(lroundf(top + area_height + float(line_width)));
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
                for (decltype(top) y = top; y < c_top; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        setting.emplace_back(PutSetting({x, y, 0xff}));
                    }
                }
                for (decltype(top) y = c_top; y < c_bottom; ++y) {
                    for (decltype(left) x = left; x < c_left; ++x) {
                        setting.emplace_back(PutSetting({x, y, 0xff}));
                    }
                    for (decltype(left) x = c_right; x < right; ++x) {
                        setting.emplace_back(PutSetting({x, y, 0xff}));
                    }
                }
                for (decltype(top) y = c_bottom; y < bottom; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        setting.emplace_back(PutSetting({x, y, 0xff}));
                    }
                }
                return setting;
            }

            static inline std::vector<PutSetting> _prepare_fill_rectangle_solid(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &p1, const SeetaAIPPoint &p2) {
                // check plot area
                auto area_width = p2.x - p1.x;
                auto area_height = p2.y - p1.y;
                if (area_height < 0 || area_width < 0) return {};
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(lroundf(p1.y));
                auto bottom = int32_t(lroundf(top + area_height + 1));
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
                        setting.emplace_back(PutSetting({x, y, 0xff}));
                    }
                }

                return setting;
            }

            static inline float _pow2(float x) { return x * x; }

            static inline float _distance(const SeetaAIPPoint &a, const SeetaAIPPoint &b) {
                return sqrtf(_pow2(a.x - b.x) + _pow2(a.y - b.y));
            }

            static inline std::vector<PutSetting> _prepare_fill_circle(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &center, int radius) {
                // check plot area
                auto area_width = 2 * radius;
                auto area_height = 2 * radius;
                if (area_height < 0 || area_width < 0) return {};
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(floorf(center.y - radius));
                auto bottom = int32_t(ceilf(center.y + radius + 1));
                auto left = int32_t(floorf(center.x - radius));
                auto right = int32_t(ceilf(center.x + radius + 1));

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
                        if (part <= 0) continue;
                        if (part >= 1) {
                            setting.emplace_back(PutSetting({x, y, 0xff}));
                        } else {
                            setting.emplace_back(PutSetting({x, y, int32_t(part * 255)}));
                        }
                    }
                }

                return setting;
            }

            static inline std::vector<PutSetting> _prepare_circle(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &center, int radius,
                    int line_width) {
                // check plot area
                if (line_width <= 0) return {};
                auto area_width = 2 * radius;
                auto area_height = 2 * radius;
                if (area_height < 0 || area_width < 0) return {};
                auto half = float(line_width) / 2;
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(floorf(center.y - radius - half));
                auto bottom = int32_t(ceilf(center.y + radius + half + 1));
                auto left = int32_t(floorf(center.x - radius - half));
                auto right = int32_t(ceilf(center.x + radius + half + 1));

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
                        auto part = std::min(radius + half + 0.5f - d,
                                             d - (radius - half - 0.5f));
                        if (part <= 0) continue;
                        if (part >= 1) {
                            setting.emplace_back(PutSetting({x, y, 0xff}));
                        } else {
                            setting.emplace_back(PutSetting({x, y, int32_t(part * 255)}));
                        }
                    }
                }

                return setting;
            }

            /**
             * line for ax + by + c = 0
             */
            class Line {
            public:
                Line() = default;
                Line(float a, float b, float c)
                        : a(a), b(b), c(c) {}

                Line(const SeetaAIPPoint &a, const SeetaAIPPoint &b) {
                    auto x1 = a.x;
                    auto y1 = a.y;
                    auto x2 = b.x;
                    auto y2 = b.y;
                    // for (y2-y1)x-(x2-x1)y-x1(y2-y1)+y1(x2-x1)=0
                    this->a = y2 - y1;
                    this->b = x1 - x2;
                    this->c = y1 * (x2 - x1) - x1 * (y2 - y1);
                }

                double distance(const SeetaAIPPoint &p) const {
                    return std::fabs(a * p.x + b * p.y + c) / sqrtf(a * a + b * b);
                }

                // get y by x
                float get_y(float x) const {
                    if (near_zero(b)) return 0;
                    return (a * x + c) / -b;
                }

                // get x by y
                float get_x(float y) const {
                    if (near_zero(a)) return 0;
                    return (b * y + c) / -a;
                }

                static bool near_zero(float f) {
                    return f <= FLT_EPSILON && -f <= FLT_EPSILON;
                }

                float k() const {
                    if (near_zero(b)) return 0;
                    return -a / b;
                }

                float theta() const {
                    return atanf(k());
                }

                SeetaAIPPoint projection(const SeetaAIPPoint &p) const {
                    if (near_zero(a)) {
                        SeetaAIPPoint result;
                        result.x = p.x;
                        result.y = -c / b;
                        return  result;
                    }
                    if (near_zero(b)) {
                        SeetaAIPPoint result;
                        result.x = -c / a;
                        result.y = p.y;
                        return result;
                    }
                    // y = kx + b  <==>  ax + by + c = 0
                    auto k = -a / b;
                    SeetaAIPPoint o = {0, -c / b};
                    SeetaAIPPoint project = {0};
                    project.x = (float) ((p.x / k + p.y - o.y) / (1 / k + k));
                    project.y = (float) (-1 / k * (project.x - p.x) + p.y);
                    return project;
                }

                float a = 0;
                float b = 0;
                float c = 0;
            };


            static inline std::vector<PutSetting> _prepare_line(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &p1, const SeetaAIPPoint &p2,
                    int line_width) {
                if (_distance(p1, p2) < 1e-3) {
                    return _prepare_fill_circle(width, height, p1, line_width / 2);
                }
                // check plot area
                if (line_width <= 0) return {};
                // make point left2right
                float min_x = std::min(p1.x, p2.x);
                float max_x = std::max(p1.x, p2.x);
                float min_y = std::min(p1.y, p2.y);
                float max_y = std::max(p1.y, p2.y);

                auto area_width = max_x - min_x;
                auto area_height = max_y - min_y;
                auto half = float(line_width) / 2;
                // get int area: [top, bottom) x [left, right)
                auto top = int32_t(floorf(min_y - half));
                auto bottom = int32_t(ceilf(max_y + half + 1));
                auto left = int32_t(floorf(min_x - half));
                auto right = int32_t(ceilf(max_x + half + 1));

                // check all pixel in graph
                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = height;
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = width;

                Line line(p1, p2);

                std::vector<PutSetting> setting;
                if (area_height > area_width) {
                    // H scan
                    float scan_width = (line_width + 1) / (cosf(atanf(fabsf(line.b / line.a))));
                    SeetaAIPPoint s1 = p1, s2 = p2;
                    if (s2.y < s1.y) std::swap(s1, s2);
                    for (decltype(top) y = top; y < bottom; ++y) {
                        auto center_x = line.get_x(y);
                        auto scan_left = int32_t(floorf(center_x - scan_width / 2));
                        auto scan_right = int32_t(ceilf(center_x + scan_width / 2 + 1));
                        if (scan_left < 0) scan_left = 0;
                        if (scan_right > int32_t(width)) scan_right = width;
                        for (decltype(scan_left) x = scan_left; x < scan_right; ++x) {
                            SeetaAIPPoint fp = {float(x), float(y)};
                            float d = 0;
                            auto proj = line.projection(fp);
                            if (proj.y < s1.y) {
                                d = _distance(fp, s1);
                            } else if (proj.y > s2.y) {
                                d = _distance(fp, s2);
                            } else {
                                d = _distance(fp, proj);
                            }
                            auto part = half + 0.5f - d;
                            if (part <= 0) continue;
                            if (part >= 1) {
                                setting.emplace_back(PutSetting({x, y, 0xff}));
                            } else {
                                setting.emplace_back(PutSetting({x, y, int32_t(part * 255)}));
                            }
                        }
                    }
                } else {
                    // V scan
                    float scan_width = (line_width + 1) / (cosf(atanf(fabsf(line.a / line.b))));
                    SeetaAIPPoint s1 = p1, s2 = p2;
                    if (s2.x < s1.x) std::swap(s1, s2);
                    for (decltype(left) x = left; x < right; ++x) {
                        auto center_y = line.get_y(x);
                        auto scan_top = int32_t(floorf(center_y - scan_width / 2));
                        auto scan_bottom = int32_t(ceilf(center_y + scan_width / 2 + 1));
                        if (scan_top < 0) scan_top = 0;
                        if (scan_bottom > int32_t(height)) scan_bottom = height;
                        for (decltype(scan_top) y = scan_top; y < scan_bottom; ++y) {
                            SeetaAIPPoint fp = {float(x), float(y)};
                            float d = 0;
                            auto proj = line.projection(fp);
                            if (proj.x < s1.x) {
                                d = _distance(fp, s1);
                            } else if (proj.x > s2.x) {
                                d = _distance(fp, s2);
                            } else {
                                d = _distance(fp, proj);
                            }
                            auto part = half + 0.5f - d;
                            if (part <= 0) continue;
                            if (part >= 1) {
                                setting.emplace_back(PutSetting({x, y, 0xff}));
                            } else {
                                setting.emplace_back(PutSetting({x, y, int32_t(part * 255)}));
                            }
                        }
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
            static void rectangle(SeetaAIPImageData image,
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
            static void circle(SeetaAIPImageData image,
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
                    c.c4 = alpha * p.alpha / 255;
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
            static void line(SeetaAIPImageData image,
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
                auto setting = _prepare_line(image.width, image.height, p1, p2, line_width);
                // plot each point
                auto c = color;
                auto alpha = c.c4;
                for (auto &p : setting) {
                    c.c4 = alpha * p.alpha / 255;
                    put_uint8_pixel(image, p.x, p.y, c);
                }
            }
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_PLOT_H
