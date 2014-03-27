#ifndef ARITHMETIC_CODE_MAR_2014
#define ARITHMETIC_CODE_MAR_2014

#include "memman.h"
#include "mylib.h"

void compute_gradient (FloArray gradient, Picture rgb, int height, int width) {
    double co[3] = { 0.299, 0.587, 0.114 };
    double plus = 0.5, minus = 0.5, gray;
    for (int x = 0; x < height; ++x) {
        for (int j = 0; j < 3; ++j)  {
            plus += co[j] * rgb[x][1][j]; // * rgb[j][x][1] not rgb[j][x][0]
            minus += co[j] * rgb[x][0][j]; // * rgb[j][x][0] not rgb[j][x][1]
        }
        gray = plus;
        gradient[x][0] = 127.5 + plus - minus;
        for (int i = 1; i <= width - 2; ++i) {
            plus = 0.5;
            for (int j = 0; j < 3; ++j)
                plus += co[j] * rgb[x][i + 1][j];
            gradient[x][i] = 0.5 * (plus - minus) + 127.5;
            minus = gray;
            gray = plus;
        }
        gradient[x][width - 1] = plus - minus + 127.5;
    }
}

#endif
