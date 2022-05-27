//
// Created by seeta on 2020/11/11.
//

#ifndef SEETA_AIP_SEETA_AIP_COLOR_MAP_H
#define SEETA_AIP_SEETA_AIP_COLOR_MAP_H

#include "seeta_aip_plot.h"

#include <random>

namespace seeta {
    namespace aip {
        namespace plot {
            namespace {
                const Color LightPink(0xFFB6C1);
                const Color Pink(0xFFC0CB);
                const Color Crimson(0xDC143C);
                const Color LavenderBlush(0xFFF0F5);
                const Color PaleVioletRed(0xDB7093);
                const Color HotPink(0xFF69B4);
                const Color DeepPink(0xFF1493);
                const Color MediumVioletRed(0xC71585);
                const Color Orchid(0xDA70D6);
                const Color Thistle(0xD8BFD8);
                const Color Plum(0xDDA0DD);
                const Color Violet(0xEE82EE);
                const Color Magenta(0xFF00FF);
                const Color Fuchsia(0xFF00FF);
                const Color DarkMagenta(0x8B008B);
                const Color Purple(0x800080);
                const Color MediumOrchid(0xBA55D3);
                const Color DarkVoilet(0x9400D3);
                const Color DarkOrchid(0x9932CC);
                const Color Indigo(0x4B0082);
                const Color BlueViolet(0x8A2BE2);
                const Color MediumPurple(0x9370DB);
                const Color MediumSlateBlue(0x7B68EE);
                const Color SlateBlue(0x6A5ACD);
                const Color DarkSlateBlue(0x483D8B);
                const Color Lavender(0xE6E6FA);
                const Color GhostWhite(0xF8F8FF);
                const Color Blue(0x0000FF);
                const Color MediumBlue(0x0000CD);
                const Color MidnightBlue(0x191970);
                const Color DarkBlue(0x00008B);
                const Color Navy(0x000080);
                const Color RoyalBlue(0x4169E1);
                const Color CornflowerBlue(0x6495ED);
                const Color LightSteelBlue(0xB0C4DE);
                const Color LightSlateGray(0x778899);
                const Color SlateGray(0x708090);
                const Color DoderBlue(0x1E90FF);
                const Color AliceBlue(0xF0F8FF);
                const Color SteelBlue(0x4682B4);
                const Color LightSkyBlue(0x87CEFA);
                const Color SkyBlue(0x87CEEB);
                const Color DeepSkyBlue(0x00BFFF);
                const Color LightBLue(0xADD8E6);
                const Color PowDerBlue(0xB0E0E6);
                const Color CadetBlue(0x5F9EA0);
                const Color Azure(0xF0FFFF);
                const Color LightCyan(0xE1FFFF);
                const Color PaleTurquoise(0xAFEEEE);
                const Color Cyan(0x00FFFF);
                const Color Aqua(0x00FFFF);
                const Color DarkTurquoise(0x00CED1);
                const Color DarkSlateGray(0x2F4F4F);
                const Color DarkCyan(0x008B8B);
                const Color Teal(0x008080);
                const Color MediumTurquoise(0x48D1CC);
                const Color LightSeaGreen(0x20B2AA);
                const Color Turquoise(0x40E0D0);
                const Color Auqamarin(0x7FFFAA);
                const Color MediumAquamarine(0x00FA9A);
                const Color MediumSpringGreen(0xF5FFFA);
                const Color MintCream(0x00FF7F);
                const Color SpringGreen(0x3CB371);
                const Color SeaGreen(0x2E8B57);
                const Color Honeydew(0xF0FFF0);
                const Color LightGreen(0x90EE90);
                const Color PaleGreen(0x98FB98);
                const Color DarkSeaGreen(0x8FBC8F);
                const Color LimeGreen(0x32CD32);
                const Color Lime(0x00FF00);
                const Color ForestGreen(0x228B22);
                const Color Green(0x008000);
                const Color DarkGreen(0x006400);
                const Color Chartreuse(0x7FFF00);
                const Color LawnGreen(0x7CFC00);
                const Color GreenYellow(0xADFF2F);
                const Color OliveDrab(0x556B2F);
                const Color Beige(0x6B8E23);
                const Color LightGoldenrodYellow(0xFAFAD2);
                const Color Ivory(0xFFFFF0);
                const Color LightYellow(0xFFFFE0);
                const Color Yellow(0xFFFF00);
                const Color Olive(0x808000);
                const Color DarkKhaki(0xBDB76B);
                const Color LemonChiffon(0xFFFACD);
                const Color PaleGodenrod(0xEEE8AA);
                const Color Khaki(0xF0E68C);
                const Color Gold(0xFFD700);
                const Color Cornislk(0xFFF8DC);
                const Color GoldEnrod(0xDAA520);
                const Color FloralWhite(0xFFFAF0);
                const Color OldLace(0xFDF5E6);
                const Color Wheat(0xF5DEB3);
                const Color Moccasin(0xFFE4B5);
                const Color Orange(0xFFA500);
                const Color PapayaWhip(0xFFEFD5);
                const Color BlanchedAlmond(0xFFEBCD);
                const Color NavajoWhite(0xFFDEAD);
                const Color AntiqueWhite(0xFAEBD7);
                const Color Tan(0xD2B48C);
                const Color BrulyWood(0xDEB887);
                const Color Bisque(0xFFE4C4);
                const Color DarkOrange(0xFF8C00);
                const Color Linen(0xFAF0E6);
                const Color Peru(0xCD853F);
                const Color PeachPuff(0xFFDAB9);
                const Color SandyBrown(0xF4A460);
                const Color Chocolate(0xD2691E);
                const Color SaddleBrown(0x8B4513);
                const Color SeaShell(0xFFF5EE);
                const Color Sienna(0xA0522D);
                const Color LightSalmon(0xFFA07A);
                const Color Coral(0xFF7F50);
                const Color OrangeRed(0xFF4500);
                const Color DarkSalmon(0xE9967A);
                const Color Tomato(0xFF6347);
                const Color MistyRose(0xFFE4E1);
                const Color Salmon(0xFA8072);
                const Color Snow(0xFFFAFA);
                const Color LightCoral(0xF08080);
                const Color RosyBrown(0xBC8F8F);
                const Color IndianRed(0xCD5C5C);
                const Color Red(0xFF0000);
                const Color Brown(0xA52A2A);
                const Color FireBrick(0xB22222);
                const Color DarkRed(0x8B0000);
                const Color Maroon(0x800000);
                const Color White(0xFFFFFF);
                const Color WhiteSmoke(0xF5F5F5);
                const Color Gainsboro(0xDCDCDC);
                const Color LightGrey(0xD3D3D3);
                const Color Silver(0xC0C0C0);
                const Color DarkGray(0xA9A9A9);
                const Color Gray(0x808080);
                const Color DimGray(0x696969);
                const Color Black(0x000000);
            }

            inline Color hue_color(float hue) {
                if (hue < 0) hue = 0;
                if (hue > 1) hue = 1;
                auto field = int(hue * 6);
                if (field > 5) field = 5;
                auto deep = (hue - float(field) / 6.0f) * 6.0f;
                if (deep < 0) deep = 0;
                if (deep > 1) deep = 1;
                auto v = uint8_t(deep * 255);
                switch (field) {
                    default: return {0, 0, 0};
                    case 0: return {255, v, 0};
                    case 1: return {uint8_t(255 - v), 255, 0};
                    case 2: return {0, 255, v};
                    case 3: return {0, uint8_t(255 - v), 255};
                    case 4: return {v, 0, 255};
                    case 5: return {255, 0, uint8_t(255 - v)};
                }
            }

            /**
             * got random color by seed at range[min, max]
             * @return
             */
            inline Color rcolor(int seed, uint8_t min, uint8_t max) {
                // std::default_random_engine generator(seed);
                std::mt19937 g(seed);
                std::uniform_int_distribution<int> r(min, max);
                return Color(uint8_t(r(g)), uint8_t(r(g)), uint8_t(r(g)));
            }

            /**
             * got random color by seed at range[128, 255]
             * @return
             */
            inline Color rcolor(int seed) {
                std::mt19937 g(seed);
                std::uniform_real_distribution<float> u(0, 1);
                return hue_color(u(g));
            }
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_COLOR_MAP_H
