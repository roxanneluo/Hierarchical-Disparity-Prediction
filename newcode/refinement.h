#ifndef REFINEMENT_CODE_H_OCT_2013
#define REFINEMENT_CODE_H_OCT_2013

#include "gridcode.h"
#include "mylib.h"
#include "blind.h"

template <class type>
void find_seen_stable_pixels(Grid<type> &disparity_left,
                             Grid<type> &disparity_right,
                             Grid<type>& occlusion_left,
                             Grid<type>& occlusion_right) {
    occlusion_left.reset(disparity_left.height, disparity_left.width);
    occlusion_right.reset(disparity_right.height, disparity_right.width);
    occlusion_left.white();
    occlusion_right.white();

    // Left-right consisty check.
    // occlusion_left[i][j] is 0 if (i, j) is a stable pixel.
    // If disparity_left[i][j] != disparity_right[i][j - d], then (i,j) is an
    // unstable pixel and occlusion_left[i][j] equals to 255.
    // Do it for disparity_right as same strategy.

    Point *points = new Point[disparity_left.width];
    int cnt, l, r, x, y;
    type d;
    for (int i = 0; i < disparity_left.height; ++i) {
        cnt = 0;
        for (int j = 0; j < disparity_left.width; ++j) {
            d = disparity_left[i][j];
            if (j - d >= 0
                    && d != 0 && abs(disparity_right[i][j - d] - d) < 1) {
                points[cnt].x = j-disparity_left[i][j];// //
                points[cnt++].y = j+disparity_left[i][j];
            }
        }
        std::sort(points, points+cnt);
        int j = 0;
        while (j < cnt) {
            x = points[j].x;
            y = points[j++].y;
            l = (x+y)/2;
            occlusion_left[i][l] = 0;
            while (j < cnt && points[j].x == x) {
                ++j;
                printf("%d,",j);
            }
        }
    }

    for (int i = 0; i < disparity_right.height; ++i) {
        cnt = 0;
        for (int j = 0; j < disparity_right.width; ++j) {
            d = disparity_right[i][j];
            if (j + d < disparity_left.width
                    && d != 0 && abs(disparity_left[i][j + d] - d) < 1) {
                points[cnt].x = j+disparity_right[i][j]; //
                points[cnt++].y = -j+disparity_right[i][j];
            }
        }
        std::sort(points, points+cnt);
        int j = 0;
        while (j < cnt) {
            x = points[j].x;
            y = points[j++].y;
            r = (x-y)/2;
            occlusion_right[i][r] = 0;
            while (j < cnt && points[j].x == x) {
                printf("%d,",j);
                ++j;
            }
        }
    }
}
template <class type>
void find_stable_pixels(Grid<type> &disparity_left,
                        Grid<type> &disparity_right,
                        Grid<type>& occlusion_left,
                        Grid<type>& occlusion_right) {
    occlusion_left.reset(disparity_left.height, disparity_left.width);
    occlusion_right.reset(disparity_right.height, disparity_right.width);
    occlusion_left.zero();
    occlusion_right.zero();

    // Left-right consisty check.
    // occlusion_left[i][j] is 0 if (i, j) is a stable pixel.
    // If disparity_left[i][j] != disparity_right[i][j - d], then (i,j) is an
    // unstable pixel and occlusion_left[i][j] equals to 255.
    // Do it for disparity_right as same strategy.
    for (int i = 0; i < disparity_left.height; ++i) {
        for (int j = 0; j < disparity_left.width; ++j) {
            type d = disparity_left[i][j];
            if (j - d >= 0) {
                if (d == 0 || abs(disparity_right[i][j - d] - d) >= 1) {
                    occlusion_left[i][j] = 255;
                }
            } else {
                occlusion_left[i][j] = 255;
            }

            d = disparity_right[i][j];
            if (j + d < disparity_right.width) {
                if (d == 0 || abs(disparity_left[i][j + d] - d) >= 1) {
                    occlusion_right[i][j] = 255;
                }
            } else {
                occlusion_right[i][j] = 255;
            }
        }
    }
}

template <class type>
void update_matching_cost(Array3<double>& cost_left,
                          Array3<double>& cost_right,
                          Grid<type>& disparity_left,
                          Grid<type>& disparity_right,
                          Grid<type>& occlusion_left,
                          Grid<type>& occlusion_right) {
    // If (i,j) is an unstable pixel, cost is 0 for all disparities.
    // Otherwise update it.
    int arr = cost_left.array;
    int h = cost_left.height;
    int w = cost_left.width;
    cost_left.zero();
    cost_right.zero();
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (occlusion_left[i][j] == 0) {
                for (int d = 0; d < arr; ++d) {
                    cost_left[d][i][j] = abs (d - disparity_left[i][j]);
                }
            }
            if (occlusion_right[i][j] == 0) {
                for (int d = 0; d < arr; ++d) {
                    cost_right[d][i][j] = abs (d - disparity_right[i][j]);
                }
            }
        }
    }
}

#endif
