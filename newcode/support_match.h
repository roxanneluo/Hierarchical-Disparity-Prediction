#ifndef SUPPORT_MATCH_H
#define SUPPORT_MATCH_H

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <emmintrin.h>
#include "sobel_filter.h"
#include "descriptor.h"
enum setting {ROBOTICS, MIDDLEBURY};

struct parameters {
    int disp_min;
    int disp_max;
    float support_threshold;
    int support_texture;
    int candidate_stepsize;
    int incon_threshold;
    int incon_window_size;
    int incon_min_support;
    bool add_corners;
    int grid_size;
    float beta;
    float gamma;
    float sigma;
    float sradius;
    int match_texture;
    int lr_threshold;
    float speckle_sim_threshold;
    int speckle_size;
    int ipol_gap_width;
    bool filter_median;
    bool filter_adaptive_mean;
    bool postprocess_only_left;
    bool subsampling;

    // conctructor;
    void set(setting s) {
        if (s == ROBOTICS) {
            disp_min = 0;
            disp_max = 255;
            support_threshold = 0.85;
            support_texture = 10;
            candidate_stepsize = 5;
            incon_window_size = 5;
            incon_threshold = 5;
            incon_min_support = 5;
            add_corners = 0;
            grid_size = 20;
            beta = 0.02;
            gamma = 3;
            sigma = 1;
            sradius = 2;
            match_texture = 1;
            lr_threshold = 2;
            speckle_sim_threshold = 1;
            speckle_size = 200;
            ipol_gap_width = 3;
            filter_median = 0;
            filter_adaptive_mean = 1;
            postprocess_only_left = 1;
            subsampling = 0;
        } else {
            disp_min = 0;
            disp_max = 255;
            support_threshold = 0.95;
            support_texture = 10;
            candidate_stepsize = 4;
            incon_window_size = 4;
            incon_threshold = 4;
            incon_min_support = 4;
            add_corners = 1;
            grid_size = 20;
            beta = 0.02;
            gamma = 5;
            sigma = 1;
            sradius = 3;
            match_texture = 0;
            lr_threshold = 2;
            speckle_sim_threshold = 1;
            speckle_size = 200;
            ipol_gap_width = 5000;
            filter_median = 1;
            filter_adaptive_mean = 0;
            postprocess_only_left = 0;
            subsampling = 0;
        }
    }
};


namespace supportmatch {

unsigned char* I1;
unsigned char* I2;
BytArray left_gray, right_gray;
int width, height, bpl;
parameters para;

void rgb2gray(BytArray gray, Picture rgb, float offset = 0, float scale = 1) {
    float co[3] = { 0.299, 0.587, 0.114 };
    float grayscale;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            grayscale = 0.5;
            for (int k = 0; k < 3; ++k)
                grayscale+=rgb[i][j][k]*co[k];
            if (offset == 0)
                gray[i][j] = grayscale;
            else gray[i][j] = scale*grayscale+offset-256*scale;
        }
}

inline unsigned int GetAddressOffsetImage(int u, int v, int width) {
    return v * width + u;
}

// The reliable pixels are stored in the support_left_map & support_right_map.
// support_left_map[i][j] == 0 => unreliable pixels
// support_left_map[i][j] == d (d > 0) pixel(i, j) is reliable pixel and
// the disparity of that pixel is d!!!
BytArray support_left_map, support_right_map;

void makeSupportMatch (Picture left, Picture right,
              int h, int w, setting s = MIDDLEBURY) {
    height = h; width = w;
    bpl = width + 15 - (width - 1) % 16;

    rgb2gray(left_gray, left);
    rgb2gray(right_gray, right);
    // make I1 I2 zero

		I1 = (unsigned char*)_mm_malloc(bpl * height * sizeof(unsigned char), 16);
		I2 = (unsigned char*)_mm_malloc(bpl * height * sizeof(unsigned char), 16);

    for (int i = 0; i < height; ++i) 
        for (int j = 0; j < width; ++j) {
            I1[i * bpl + j] = left_gray[i][j];
            I2[i * bpl + j] = right_gray[i][j];
        }

    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            support_left_map[i][j] = support_right_map[i][j] = 0;
    para.set(s);
}


void RemoveInconsistentSupportPoints(short* D_can,
                                     int D_can_width, int D_can_height) {
    for (int u_can = 0; u_can < D_can_width; u_can++) {
        for (int v_can = 0; v_can < D_can_height; v_can++) {
            short d_can = *(D_can +
                            GetAddressOffsetImage(u_can, v_can, D_can_width));
            if (d_can >= 0) {
                // compute number of other points supporting the current point.
                int support = 0;
                for (int u_can_2 = u_can - para.incon_window_size;
                        u_can_2 <= u_can + para.incon_window_size;
                        u_can_2++) {
                    for (int v_can_2 = v_can - para.incon_window_size;
                            v_can_2 <= v_can + para.incon_window_size;
                            v_can_2++) {
                        if (u_can_2 >= 0 &&
                                v_can_2 >= 0 &&
                                u_can_2 < D_can_width &&
                                v_can_2 < D_can_height) {
                            short d_can_2 = *(D_can +
                                              GetAddressOffsetImage(u_can_2, v_can_2, D_can_width));
                            if (d_can_2 >= 0 &&
                                    misc::abs(d_can - d_can_2) <= para.incon_threshold)
                                support++;
                        }
                    }
                }
                // invalidate support point if number of supporting points is too low.
                if (support < para.incon_min_support)
                    *(D_can + GetAddressOffsetImage(u_can, v_can, D_can_width)) = -1;
            }
        }
    }
}
// Add if necessary.
void RemoveRedundantSupportPoints(short* D_can,
                                  int D_can_width, int D_can_height,
                                  int redun_man_dist, int redun_threshold, bool vertical) {

}
// Add if necessary.
void AddCornerSupportPoints() {

}

inline short ComputeMatchingDisparity (const int& u, const int& v,
                                       unsigned char* I1_desc, unsigned char* I2_desc,
                                       const bool& right_image) {
    const int u_step = 2;
    const int v_step = 2;
    const int window_size = 3;

    int desc_offset_1 = -16 * u_step - 16 * width * v_step;
    int desc_offset_2 = +16 * u_step - 16 * width * v_step;
    int desc_offset_3 = -16 * u_step + 16 * width * v_step;
    int desc_offset_4 = +16 * u_step + 16 * width * v_step;

    __m128i xmm1, xmm2, xmm3, xmm4, xmm5, xmm6;
    // check if we are inside the image region.
    if (u >= window_size + u_step &&
            u <= width - window_size - 1 - u_step &&
            v >= window_size + v_step &&
            v <= height - window_size - 1 - v_step) {
        int line_offset = 16 * width * v;
        unsigned char* I1_line_addr;
        unsigned char* I2_line_addr;

        if (!right_image) {
            I1_line_addr = I1_desc + line_offset;
            I2_line_addr = I2_desc + line_offset;
        } else {
            I1_line_addr = I2_desc + line_offset;
            I2_line_addr = I1_desc + line_offset;
        }

        unsigned char* I1_block_addr = I1_line_addr + 16 * u;
        unsigned char* I2_block_addr;

        int sum = 0;
        for (int i = 0; i < 16; ++i)
            sum += abs((int)(*(I1_block_addr + i)) - 128);
        if (sum < para.support_texture)
            return -1;

        xmm1 = _mm_load_si128((__m128i*)(I1_block_addr + desc_offset_1));
        xmm2 = _mm_load_si128((__m128i*)(I1_block_addr + desc_offset_2));
        xmm3 = _mm_load_si128((__m128i*)(I1_block_addr + desc_offset_3));
        xmm4 = _mm_load_si128((__m128i*)(I1_block_addr + desc_offset_4));

        // declare match energy for each disparity
        int u_warp;

        // base match
        short min_1_E = 32676;
        short min_1_d = -1;
        short min_2_E = 32676;
        short min_2_d = -1;

        // get valid disparity range
        int disp_min_valid = misc::max(para.disp_min, 0);
        int disp_max_valid = para.disp_max;
        if (!right_image) {
            disp_max_valid = misc::min(para.disp_max, u - window_size - u_step);
        } else {
            disp_max_valid = misc::min(para.disp_max,
                                        width - u - window_size - u_step);
        }

        // assume, that we can compute at least 10 disparities for this pixel
        if (disp_max_valid - disp_min_valid < 10)
            return -1;

        // for all disparities do
        for (short d = disp_min_valid; d <= disp_max_valid; d++) {
            // warp u c:oordinate
            u_warp = right_image ? u + d : u - d;
            I2_block_addr = I2_line_addr + 16 * u_warp;
            xmm6 = _mm_load_si128((__m128i*)(I2_block_addr + desc_offset_1));

            xmm6 = _mm_sad_epu8(xmm1, xmm6);
            xmm5 = _mm_load_si128((__m128i*)(I2_block_addr + desc_offset_2));

            xmm6 = _mm_add_epi16(_mm_sad_epu8(xmm2, xmm5), xmm6);
            xmm5 = _mm_load_si128((__m128i*)(I2_block_addr + desc_offset_3));

            xmm6 = _mm_add_epi16(_mm_sad_epu8(xmm3, xmm5), xmm6);
            xmm5 = _mm_load_si128((__m128i*)(I2_block_addr + desc_offset_4));

            xmm6 = _mm_add_epi16(_mm_sad_epu8(xmm4, xmm5), xmm6);
            sum = _mm_extract_epi16(xmm6, 0) + _mm_extract_epi16(xmm6, 4);

            if (sum < min_1_E) {
                min_1_E = sum;
                min_1_d = d;
            } else if (sum < min_2_E) {
                min_2_E = sum;
                min_2_d = d;
            }
        }

        // check if best and second best match are available and
        // if matching ratio is sufficient.
        if (min_1_d >= 0 &&
                min_2_d >= 0 &&
                (float)min_1_E < para.support_threshold * (float)min_2_E) {
            return min_1_d;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

void ComputeSupportMatches(unsigned char* I1_desc,
                           unsigned char* I2_desc) {

    // be sure tht at half resolution we only need data
    // from every second line.
    int D_candidate_stepsize = para.candidate_stepsize;
    if (para.subsampling)
        D_candidate_stepsize += D_candidate_stepsize % 2;

    // create matrix for saving disparity candidates.
    int D_can_width = 0;
    int D_can_height = 0;

    for (int u = 0; u < width; u += D_candidate_stepsize) D_can_width++;
    for (int v = 0; v < height; v += D_candidate_stepsize) D_can_height++;
    short* D_can = (short*)calloc(D_can_width * D_can_height, sizeof(short));

    // loop variables
    int u, v;
    short d, d2;

    // for all point candidates in image 1 do
    for (int u_can = 1; u_can < D_can_width; u_can++) {
        u = u_can * D_candidate_stepsize;
        for (int v_can = 1; v_can < D_can_height; v_can++) {
            v = v_can * D_candidate_stepsize;
            // initialize disparity candidate to invalid.
            *(D_can + GetAddressOffsetImage(u_can, v_can, D_can_width)) = -1;
            d = ComputeMatchingDisparity(u, v, I1_desc, I2_desc, false);
            if (d >= 0) {
                d2 = ComputeMatchingDisparity(u - d, v, I1_desc, I2_desc, true);
                if (d2 >= 0 && abs(d - d2) <= para.lr_threshold)
                    *(D_can + GetAddressOffsetImage(u_can, v_can, D_can_width)) = d;
            }
        }
    }

    // remove inconsistent support points.
    RemoveInconsistentSupportPoints(D_can, D_can_width, D_can_height);

    // get left & right support maps.
    for (int u_can = 1; u_can < D_can_width; u_can++) {
        for (int v_can = 1; v_can < D_can_height; v_can++) {
            int x = u_can * D_candidate_stepsize;
            int y = v_can * D_candidate_stepsize;
            if (*(D_can + GetAddressOffsetImage(u_can, v_can, D_can_width)) >= 0) {
                support_left_map[y][x] =
                    *(D_can + GetAddressOffsetImage(u_can, v_can, D_can_width));
            }
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char d = support_left_map[y][x];
            if (x - d >= 0)
                support_right_map[y][x - d] = d;
        }
    }
    free(D_can);
}

void findSupportMatch() {
    Descriptor desc1(I1, width, height, bpl, para.subsampling);
    Descriptor desc2(I2, width, height, bpl, para.subsampling);
    ComputeSupportMatches(desc1.I_desc, desc2.I_desc);
}

}
#endif


