#ifndef INPUT_OUTPUT_MANAGER_MAR_2014
#define INPUT_OUTPUT_MANAGER_MAR_2014

#include <cstdio>
#include <string>
#include <cctype>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <png++/png.hpp>
#include "settings.hpp"
#include "io_disp.hpp"

using std::string;
const int BUF_LEN = MAX_WIDTH * MAX_HEIGHT * 3 + 2;
unsigned char buff[BUF_LEN];

void load_image_png(const char * filename, Picture rgb, int &H, int &W, int scale);
void load_image_ppm(const char * filename, Picture rgb, int &H, int &W, int scale);
void load_image(const char * filename, Picture rgb, int &H, int &W, int scale = 1) {
  string name = filename;
  int pos = name.find_last_of(".");
  string type = name.substr(pos+1);
  if (type == "ppm")
    load_image_ppm(filename, rgb, H, W, scale);
  else if (type == "png")
    load_image_png(filename, rgb, H, W, scale);
  else assert(false);
}



void load_image_ppm(const char * filename, Picture rgb, int &H, int &W, int scale = 1) {
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

template <typename Dtype>
void load_image_gray_png(const char * filename, Dtype disp[][MAX_WIDTH], int &H, int &W, 
    int scale = 1) {
  DisparityImage im(filename);
  H = im.height();
  W = im.width();
  for (int i = 0; i < H; ++i)
  for (int j = 0; j < W; ++j) {
    disp[i][j] = im.getDisp(j,i)/ scale;
  }
}

template <typename Dtype>
void load_image_gray_pgm(const char * filename, Dtype gray[][MAX_WIDTH], 
    int &H, int &W, int scale = 1) {
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
        for (int y = 0; y < W; ++y) {
            gray[x][y] = buff[++i]/scale; 
            //!!!buff must be UNSIGNED CHAR, otherwise, 255 is negative!!!
        }
}

template <typename Dtype>
void load_image_gray(const char * filename, Dtype disp[][MAX_WIDTH], int &H, int &W, 
    int scale = 1) {
  string name = filename;
  int pos = name.find_last_of(".");
  string type = name.substr(pos+1);
  if (type == "pgm")
    load_image_gray_pgm(filename, disp, H, W, scale);
  else if (type == "png")
    load_image_gray_png(filename, disp, H, W, scale);
  else assert(false);
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

void load_image_png(const char * filename, Picture rgb,int &H, int &W, int scale = 1) {
  png::image< png::rgb_pixel > image(filename);
  H = image.get_height();
  W = image.get_width();
  for (int i = 0; i < H; ++i)
  for (int j = 0; j < W; ++j) {
    png::rgb_pixel p = image.get_pixel(j,i);
    rgb[i][j][0] = p.red / scale;
    rgb[i][j][1] = p.green / scale;
    rgb[i][j][2] = p.blue / scale;
  }
}


void save_image_png(const char * filename, const BytArray disp, int H, int W, int scale = 1);
template <typename Dtype>
void save_image_pgm(const char * filename, Dtype gray[][MAX_WIDTH], int H, int W, int scale = 1);
void save_image(const char * filename, const BytArray disp, int H, int W, int scale = 1) {
  string name = filename;
  int pos = name.find_last_of(".");
  string type = name.substr(pos+1);
  if (type == "pgm")
    save_image_pgm(filename, disp, H, W, scale);
  else if (type == "png")
    save_image_png(filename, disp, H, W,scale);
  else
    assert(false);
}

void save_image_png(const char * filename, const BytArray disp, int H, int W, int scale) {
  DisparityImage im(W, H);
  for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j) {
      im.setDisp(j,i, disp[i][j] * scale / 256.0);
    }
  im.write(filename);
}

template <typename Dtype>
void save_image_pgm(const char * filename, Dtype gray[][MAX_WIDTH], int H, int W, int scale) {
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
    int color_sum;
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j) {
        //assert(rgb[i][j][0] == rgb[i][j][1] && rgb[i][j][1] == rgb[i][j][2]);
        color_sum = 0;
        for (int c = 0; c < 3; ++c)
          color_sum += rgb[i][j][c];  
        gray[i][j] = color_sum/3;
    }
}

template<typename Dtype>
void save_array(const char * filename, Dtype *a, int len) {
    std::ofstream fout(filename, std::ofstream::out);
    fout << len << std::endl;
    for (int i = 0; i < len; ++i)
        fout << a[i] << " ";
    fout.close();
}

template<typename Dtype>
void save_matrix(const char * filename, Dtype * a, int h, int w) {
    std::ofstream fout(filename, std::ofstream::out);
    fout << h << " " << w << std::endl;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j)
            fout << a[i * w + j] << " ";
        fout << std::endl;
    }
    fout.close();
}


#endif
