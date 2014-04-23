#ifndef INPUT_OUTPUT_MANAGER_MAR_2014
#define INPUT_OUTPUT_MANAGER_MAR_2014

#include <cstdio>
#include <cstring>
#include <cctype>
#include "settings.h"

const int BUF_LEN = 6500303;
char buff[BUF_LEN];

void load_image(const char * filename, Picture rgb, int &H, int &W) {
    // return if it is successfull
    FILE * f = fopen(filename, "rb");
    int bytesread = fread(buff, 1, BUF_LEN, f);
    if (bytesread < 0) ; // assertion omitted
    fclose(f);
    int i = 0;
    while (buff[i]) {
        if (buff[i] == 'P') {
            ++i;
            break;
        }
        ++i;
    }
    //int type = buff[i] - '0';
    ++i; // i shall assume it is exactly P6 type
    int value[3], count = 0;
    while (buff[i] && count < 3) {
        if (buff[i] == '#') {
            while (buff[i] != '\n') ++i;
            ++i;
        } else if (!isdigit(buff[i])) ++i;
        else {
            int k = 0;
            while (isdigit(buff[i]))
                k = k * 10 + buff[i++] - '0';
            value[count++] = k;
        }
    }
    // now take down the last end-of-line
    while (buff[i] != '\n') ++i; // after the while  buff[i] == \n

    H = value[1]; W = value[0];
    for (int x = 0; x < H; ++x)
        for (int y = 0; y < W; ++y)
            for (int color = 0; color < 3; ++color)
                rgb[x][y][color] = (unsigned char) (buff[++i]);
}

void load_image_gray(const char * filename, IntArray gray, int &H, int &W) {
    // return if it is successfull
    FILE * f = fopen(filename, "rb");
    int bytesread = fread(buff, 1, BUF_LEN, f);
    if (bytesread < 0);
    fclose(f);
    int i = 0;
    while (buff[i]) {
        if (buff[i] == 'P') {
            ++i;
            break;
        }
        ++i;
    }
    //int type = buff[i] - '0';
    ++i; // i shall assume it is exactly P5 type
    int value[3], count = 0;
    while (buff[i] && count < 3) {
        if (buff[i] == '#') {
            while (buff[i] != '\n') ++i;
            ++i;
        } else if (!isdigit(buff[i])) ++i;
        else {
            int k = 0;
            while (isdigit(buff[i]))
                k = k * 10 + buff[i++] - '0';
            value[count++] = k;
        }
    }
    // now take down the last end-of-line
    while (buff[i] != '\n') ++i;

    H = value[1]; W = value[0];
    for (int x = 0; x < H; ++x)
        for (int y = 0; y < W; ++y)
            gray[x][y] = (unsigned char) (buff[++i]);
}

void save_image(const char * filename, BytArray gray, int H, int W, int scale = 1) {
    FILE * f = fopen(filename, "wb");
    fprintf(f, "P5\n");
    fprintf(f, "#Produced by my silly program\n");
    fprintf(f, "%d %d\n255\n", W, H);
    int cnt = 0;
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            buff[cnt++] = gray[i][j] * scale;
    fwrite(buff, 1, cnt, f);
    fclose(f);
}

void save_image_rgb(const char * filename, Picture rgb, int H, int W, int scale = 1) {
    // remember th filename should be  .ppm
    FILE * f = fopen(filename, "wb");
    fprintf(f, "P6\n");
    fprintf(f, "#Produced by my silly program\n");
    fprintf(f, "%d %d\n255\n", W, H);
    int cnt = 0;
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            for (int c = 0; c < 3; c++)
                buff[cnt++] = rgb[i][j][c] * scale;
    fwrite(buff, 1, cnt, f);
    fclose(f);
}

#endif
