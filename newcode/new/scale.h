#ifndef SCALE_H_OCT_2013
#define SCALE_H_OCT_2013

#include "gridcode.h"

// This is a box filter.

template <class type>
type average (const type& a, const type& b) {
    type ans = type((a + b) / 2);
    return ans;
}

template <class type>
void scale_image(Array3<type>& large_image, Array3<type>& scaled) {
    int h = large_image.height;
    int w = large_image.width;
    int arr = large_image.array;

    int hs = h / 2;
    int ws = w / 2;

    type average_i1, average_i2;

    scaled.reset(arr, hs, ws);
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < hs; i++) {
            int i2 = i * 2;
            for (int j = 0; j < ws; j++) {
                int j2 = j * 2;
                average_i1 = (j2 + 1 < w) ?
                             average(large_image[k][i2][j2], large_image[k][i2][j2 + 1]) :
                             large_image[k][i2][j2];
                if (i2 + 1 < h) {
                    average_i2 = (j2 + 1 < w) ?
                                 average(large_image[k][i2 + 1][j2],
                                         large_image[k][i2 + 1][j2 + 1]) :
                                 large_image[k][i2 + 1][j2];
                } else {
                    average_i2 = average_i1;
                }
                scaled[k][i][j] = average(average_i1, average_i2);
            }
        }
    }
}


#endif
