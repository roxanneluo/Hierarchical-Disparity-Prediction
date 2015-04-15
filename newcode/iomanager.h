#ifndef INPUT_OUTPUT_MANAGER_MAR_2014
#define INPUT_OUTPUT_MANAGER_MAR_2014

#include <cstdio>
#include <cstring>
#include <cctype>
#include <fstream>
#include <algorithm>
#include <cassert>
#include "settings.h"

const int BUF_LEN = MAX_WIDTH * MAX_HEIGHT * 3 + 2;
char buff[BUF_LEN];

void load_image(const char * filename, Picture rgb, int &H, int &W, int scale = 1) {
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
                rgb[x][y][color] = (unsigned char) (buff[++i]/scale);
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

/*
void save_image(const char * filename, BytArray gray, int H, int W, int scale = 1) {
  std::ofstream fout(filename, std::ofstream::binary);
  fout << "P5\n" 
    << "#Produced by my silly program\n"
    << W << " " <<  H << " " << 255;
  
  int cnt = 0;
  for (int i = 0; i < H; ++i)
      for (int j = 0; j < W; ++j) 
          buff[cnt++] = gray[i][j] * scale;
  fout.write(buff, cnt);
  fout.close();
}
*/

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

void rgb2gray(BytArray gray, const Picture rgb, int H, int W) {
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j) {
        assert(rgb[i][j][0] == rgb[i][j][1] && rgb[i][j][1] == rgb[i][j][2]);
        gray[i][j] = rgb[i][j][0];  
    }
}

template<typename Dtype>
void save_array(const char * filename, Dtype *a, int len) {
    std::ofstream fout(filename, std::ofstream::out);
    for (int i = 0; i < len; ++i)
        fout << a[i] << " ";
    fout.close();
}

template<typename Dtype>
void save_matrix(const char * filename, Dtype ** a, int h, int w) {
    std::ofstream fout(filename, std::ofstream::out);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j)
            fout << a[i][j] << " ";
        fout << std::endl;
    }
    fout.close();
}

//median filter
/*
void shrinkGray(BytArray small, BytArray large, int smallh, int smallw,
    bool halve_disp = true) {
  const int MED_RADIUS = 2;
  int x,y;
  int a[MED_RADIUS * MED_RADIUS];
  for (int i = 0; i < smallh; ++i)
  for (int j = 0; j < smallw; ++j) {
    x = 2*i, y = 2*j;
    int cnt = 0;
    for (int ii = 0; ii < 2; ++ii)    
    for (int jj = 0; jj < 2; ++jj)
      a[cnt++] = large[x + ii][y + jj];
    std::sort(a, a + (MED_RADIUS * MED_RADIUS));
    small[i][j] = a[1];
    if (halve_disp)
      small[i][j] /= 2;
  }
}
*/

// mean filter
void shrinkGray(BytArray small, BytArray large, int smallh, int smallw,
    bool halve_disp = true) {
  int x,y;
  for (int i = 0; i < smallh; ++i)
  for (int j = 0; j < smallw; ++j) {
    x = 2*i; y = 2*j;
    small[i][j] = (large[x][y] + large[x][y+1] + large[x+1][y] + large[x+1][y+1])/4;
    // note this is int division
    if (halve_disp)
      small[i][j] /= 2;
  } 
}
#endif
