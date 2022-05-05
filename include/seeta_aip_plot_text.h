//
// Created by kier on 2020/11/12.
//

#ifndef SEETA_AIP_SEETA_AIP_PLOT_TEXT_H
#define SEETA_AIP_SEETA_AIP_PLOT_TEXT_H

#include "seeta_aip_plot.h"
#include "seeta_aip_font_ascii.h"

#include <map>

namespace seeta {
    namespace aip {
        namespace plot {
            /**
             * p it top-left position
             * @param width
             * @param height
             * @param p
             * @param scale
             * @return
             */
            static inline std::vector<PutSetting> _prepare_font_pixel(
                    uint32_t width, uint32_t height,
                    const SeetaAIPPoint &p, float scale) {
                auto top = int32_t(floorf(p.y));
                auto bottom = int32_t(ceilf(p.y + scale + 1));
                auto left = int32_t(floorf(p.x));
                auto right = int32_t(ceilf(p.x + scale + 1));
                auto half = scale / 2;

                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = height;
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = width;

                std::vector<PutSetting> setting;
                for (decltype(top) y = top; y < bottom; ++y) {
                    auto ypart = std::min(y - (p.y - 0.5f),
                                          p.y + scale + 0.5f - y);
                    if (ypart <= 0) continue;
                    ypart = std::min(ypart, 1.0f);
                    for (decltype(left) x = left; x < right; ++x) {
                        auto xpart = std::min(x - (p.x - 0.5f),
                                              p.x + scale + 0.5f - x);
                        if (ypart <= 0) continue;
                        xpart = std::min(xpart, 1.0f);
                        auto part = ypart * xpart;
                        setting.emplace_back(PutSetting({x, y, 0xff}));
                    }
                }

                return setting;
            }

            class LetterFont {
            public:
                LetterFont(int ch) {
                    m_map.resize((Width() + 2) * (Height() + 2), 0);
                    if (ch & 0xffffff00) return;
                    auto &font = seeta_aip_font_8x16[ch];
                    for (int y = 0; y < 16; ++y) {
                        for (int x = 0; x < 8; ++x) {
                            set(x, y, font[y] & (1 << x) ? 1 : 0);
                        }
                    }
                }

                uint8_t get(int32_t x, int32_t y) const {
                    return m_map[(y + 1) * (Width() + 2) + x + 1];
                }

                void set(int32_t x, int32_t y, uint8_t v) {
                    m_map[(y + 1) * (Width() + 2) + x + 1] = v;
                }

                float get(float sx, float sy) const {
                    auto lx = int32_t(floorf(sx));
                    auto ly = int32_t(floorf(sy));
                    if (lx < -1 || lx >= Width() ||
                        ly < -1 || ly >= Height())
                        return 0;
                    auto frx = sx - lx;
                    auto fry = sy - ly;
                    auto flx = 1 - frx;
                    auto fly = 1 - fry;
                    return flx * fly * get(lx, ly)
                           + flx * fry * get(lx, ly + 1)
                           + frx * fly * get(lx + 1, ly)
                           + frx * fry * get(lx + 1, ly + 1);
                }

                static int32_t Width() { return 8; }

                static int32_t Height() { return 16; }

            private:
                std::vector<uint8_t> m_map;
            };

            static inline std::vector<PutSetting> _prepare_ascii(
                    uint32_t width, uint32_t height,
                    const LetterFont &font,
                    const SeetaAIPPoint &p, float scale) {
                auto area_width = LetterFont::Width() * scale;
                auto area_height = LetterFont::Height() * scale;

                auto top = int32_t(floorf(p.y));
                auto bottom = int32_t(ceilf(p.y + area_height + 1));
                auto left = int32_t(floorf(p.x));
                auto right = int32_t(ceilf(p.x + area_width + 1));

                if (top < 0) top = 0;
                if (bottom > int32_t(height)) bottom = int32_t(height);
                if (left < 0) left = 0;
                if (right > int32_t(width)) right = int32_t(width);

                std::vector<PutSetting> setting;
                for (decltype(top) y = top; y < bottom; ++y) {
                    for (decltype(left) x = left; x < right; ++x) {
                        // compute sample axis at original font
                        auto sx = (x - p.x) / scale;
                        auto sy = (y - p.y) / scale;
                        auto part = font.get(sx, sy);
                        if (part > 0.999) {
                            setting.emplace_back(PutSetting({x, y, 0xff}));
                        } else {
                            setting.emplace_back(PutSetting({x, y, int(part * 255)}));
                        }
                    }
                }
                return setting;
            }

            static inline std::vector<PutSetting> _prepare_ascii(
                    uint32_t width, uint32_t height,
                    int ch,
                    const SeetaAIPPoint &p, float scale) {
                if (ch & 0xffffff00) return {};
                LetterFont font(ch);
                return _prepare_ascii(width, height, font, p, scale);
            }

            static void put_uint8_ascii(SeetaAIPImageData image,
                                        int ch,
                                        const SeetaAIPPoint &left_top, const Color &color,
                                        float font_scale = 1.0f) {
                if (ch & 0xffffff00) return;
                if (font_scale < 1.0f) font_scale = 1.0f;
                auto &font = seeta_aip_font_8x16[ch];
                std::vector<PutSetting> setting = _prepare_ascii(
                        image.width, image.height, ch, left_top, font_scale);
                auto c = color;
                auto alpha = c.c4;
                for (auto &p : setting) {
                    c.c4 = alpha * p.alpha / 255;
                    put_uint8_pixel(image, p.x, p.y, c);
                }
                return;
            }

            class TextEditor {
            public:
                TextEditor(
                        float x, float y,
                        float scale = 1.0f,
                        int endl = -1)
                        : m_scale(scale) {
                    m_start_x = int(x);
                    m_start_y = int(y);
                    m_cursor_x = x;
                    m_cursor_y = y;
                    m_endl = endl;
                }

                LetterFont &font(int ch) {
                    auto it = m_cache.find(ch);
                    if (it != m_cache.end()) {
                        return it->second;
                    }
                    LetterFont letter(ch);
                    auto it_b = m_cache.insert(std::make_pair(ch, letter));
                    return it_b.first->second;
                }

                SeetaAIPPoint cursor() const {
                    return {m_cursor_x, m_cursor_y};
                }

                /**
                 *
                 * @param ch
                 * @return NULL if ch is control letter.
                 */
                LetterFont *type(int ch) {
                    if (ch == '\t') {
                        auto shift = 8 - m_count % 8;
                        m_count += shift;
                        m_cursor_x += float(shift) * float(LetterFont::Width() + 1) * m_scale;
                        try_next_letter_endl();
                        return nullptr;
                    }

                    if (ch == '\r') {
                        m_cursor_x = float(m_start_x);
                        m_count = 0;
                        return nullptr;
                    }

                    if (ch == '\n') {
                        m_cursor_y += float(LetterFont::Height() + 1) * m_scale;
                        m_cursor_x = float(m_start_x);
                        m_count = 0;
                        m_line += 1;
                        return nullptr;
                    }

                    m_cursor_x += (LetterFont::Width() + 1) * m_scale;
                    try_next_letter_endl();

                    return &font(ch);
                }

                int line() const {
                    return m_line;
                }

            private:
                void try_next_letter_endl() {
                    auto next_edge = cursor().x + float(LetterFont::Width()) * m_scale;
                    if (m_endl > 0 && int(next_edge) >= m_endl) {
                        m_cursor_y += float(LetterFont::Height() + 1) * m_scale;
                        m_cursor_x = float(m_start_x);
                        m_count = 0;
                        m_line += 1;
                    }
                }

                std::map<int, LetterFont> m_cache;
                float m_scale = 1.0f;

                int m_start_x = 0;
                int m_start_y = 0;
                int m_endl = 0;

                float m_cursor_x = 0;
                float m_cursor_y = 0;

                int m_count = 0;
                int m_line = 0;
            };

            /**
             * Put text
             * @param image BYTE type HWC format image.
             * @param msg ascii message
             * @param left_top message top left message
             * @param color color according to image format
             * @param font_scale based on 8x16 font
             * @param endl end line, -1 for no endl. If set, letter approching endl would start new line.
             * @return put text line number
             */
            static int text(SeetaAIPImageData image,
                            const std::string &msg,
                            const SeetaAIPPoint &left_top, const Color &color,
                            float font_scale = 1.0f,
                            int endl = -1) {
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

                TextEditor editor(left_top.x, left_top.y, font_scale, endl);
                for (auto &ch : msg) {
                    auto cursor = editor.cursor();
                    auto font = editor.type(ch);
                    if (!font) continue;
                    put_uint8_ascii(image, ch, cursor, color, font_scale);
                }

                return editor.line() + 1;
            }
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_PLOT_TEXT_H
