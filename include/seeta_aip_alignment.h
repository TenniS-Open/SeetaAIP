//
// Created by kier on 2020/11/17.
//

#ifndef SEETA_AIP_SEETA_AIP_ALIGNMENT_H
#define SEETA_AIP_SEETA_AIP_ALIGNMENT_H

#include "seeta_aip_graphics2d.h"

#include <vector>
#include <cfloat>
#include <climits>

namespace seeta {
    namespace aip {
        /**
         *
         * @param src length N * 2
         * @param dst length N * 2
         * @param N
         * @param M length 3 * 3
         * @return
         * compute transform matrix which can trans points from src to dst, src = M * dst
         * @note M can directly parse to affine_sample2d
         */
        static inline bool alignment2d(
                const float *src,
                const float *dst,
                int N,
                float *M) {
            const float *std_points = dst;
            const float *src_points = src;

            float sum_x = 0, sum_y = 0;
            float sum_u = 0, sum_v = 0;
            float sum_xx_yy = 0;
            float sum_ux_vy = 0;
            float sum_vx_uy = 0;

            for (int c = 0; c < N; ++c) {
                int x_off = c * 2;
                int y_off = x_off + 1;
                sum_x += std_points[c * 2];
                sum_y += std_points[c * 2 + 1];
                sum_u += src_points[x_off];
                sum_v += src_points[y_off];
                sum_xx_yy += std_points[c * 2] * std_points[c * 2] +
                             std_points[c * 2 + 1] * std_points[c * 2 + 1];
                sum_ux_vy += std_points[c * 2] * src_points[x_off] +
                             std_points[c * 2 + 1] * src_points[y_off];
                sum_vx_uy += src_points[y_off] * std_points[c * 2] -
                             src_points[x_off] * std_points[c * 2 + 1];
            }

            if (sum_xx_yy <= FLT_EPSILON) return false;

            float q = sum_u - sum_x * sum_ux_vy / sum_xx_yy
                      + sum_y * sum_vx_uy / sum_xx_yy;
            float p = sum_v - sum_y * sum_ux_vy / sum_xx_yy
                      - sum_x * sum_vx_uy / sum_xx_yy;
            float r = float(N) - (sum_x * sum_x + sum_y * sum_y) / sum_xx_yy;

            if (!(r > FLT_EPSILON || r < -FLT_EPSILON)) return false;

            float a = (sum_ux_vy - sum_x * q / r - sum_y * p / r) / sum_xx_yy;
            float b = (sum_vx_uy + sum_y * q / r - sum_x * p / r) / sum_xx_yy;
            float c = q / r;
            float d = p / r;

            M[0] = M[4] = a;
            M[1] = -b;
            M[3] = b;
            M[2] = c;
            M[5] = d;

            M[6] = M[7] = 0;
            M[8] = 1;

            return true;
        }

        /**
         *
         * @param src N points
         * @param dst N points
         * @return trans form 2d
         * compute transform matrix which can trans points from src to dst, src = M * dst
         * @note M can directly parse to affine_sample2d
         */
        static inline Trans2D<float> alignment2d(
                const std::vector<Vec2D<float>> &src,
                const std::vector<Vec2D<float>> &dst) {
            auto N = std::min(src.size(), dst.size());
            std::vector<float> src_data(src.size() * 2);
            std::vector<float> dst_data(src.size() * 2);
            for (size_t i = 0; i < N; ++i) {
                src_data[i * 2] = src[i][0];
                src_data[i * 2 + 1] = src[i][1];
                dst_data[i * 2] = dst[i][0];
                dst_data[i * 2 + 1] = dst[i][1];
            }
            Trans2D<float> M = affine::identity<float>();
            alignment2d(src_data.data(), dst_data.data(), int(N), M.data());
            return M;
        }
    }
}

#endif //SEETA_AIP_SEETA_AIP_ALIGNMENT_H
