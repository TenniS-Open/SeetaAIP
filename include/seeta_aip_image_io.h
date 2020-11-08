//
// Created by seeta on 2020/10/30.
//

#ifndef SEETA_AIP_SEETA_AIP_IMAGE_IO_H
#define SEETA_AIP_SEETA_AIP_IMAGE_IO_H

#include "seeta_aip_struct.h"
#include "seeta_aip_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

#include <cstring>
#include <cstdlib>

namespace seeta {
    namespace aip {
            static seeta::aip::ImageData imread(const std::string &filename) {
                int iw, ih, n;
                iw = ih = n = 0;
                unsigned char *data = stbi_load(filename.c_str(), &iw, &ih, &n, 3);
                if (!data) return seeta::aip::ImageData();
                seeta::aip::ImageData result(SEETA_AIP_FORMAT_U8RGB, iw, ih, n, data);
                stbi_image_free(data);
                return result;
            }

            static int imwrite(const std::string &filename, const SeetaAIPImageData &image) {
                auto rgb_image_t = convert(1, SEETA_AIP_FORMAT_U8RGB, image);
                const SeetaAIPImageData &rgb_image = rgb_image_t;
                auto c_filename = filename.c_str();
                const char *ext = strrchr(c_filename, '.');
                if (!ext)
                {
                    return stbi__err("unknown image type", "image not of any known type, or corrupt");
                }

                if ((strcasecmp(ext, ".jpeg") == 0) || (strcasecmp(ext, ".jpg") == 0))
                {
                    return  stbi_write_jpg(c_filename,
                                           rgb_image.width, rgb_image.height, rgb_image.channels, rgb_image.data,
                                           90);
                }
                else if (strcasecmp(ext, ".png") == 0)
                {
                    return stbi_write_png(c_filename,
                                          rgb_image.width, rgb_image.height, rgb_image.channels, rgb_image.data,
                                          0);
                }
                else if (strcasecmp(ext, ".tga") == 0)
                {
                    return  stbi_write_tga(c_filename,
                                           rgb_image.width, rgb_image.height, rgb_image.channels, rgb_image.data);
                }
                else if (strcasecmp(ext, ".hdr") == 0)
                {
                    return  stbi_write_hdr(c_filename,
                                           rgb_image.width, rgb_image.height, rgb_image.channels,
                                           (float *)rgb_image.data);;
                }
                else if (strcasecmp(ext, ".bmp") == 0)
                {
                    return  stbi_write_bmp(c_filename,
                                           rgb_image.width, rgb_image.height, rgb_image.channels,
                                           rgb_image.data);;
                }

                return stbi__err("unknown image type", "Image not of any known type, or corrupt");

            }
    }
}

#endif //SEETA_AIP_SEETA_AIP_IMAGE_IO_H
