#ifndef the_small_functions_hehe
#define the_small_functions_hehe

#include <cstring>
#include <cstdlib>
//#include "median.h"
#include "ctmf.h"

#include <cstdio>

namespace misc {

inline int max(int a, int b) { if (a > b) return a; else return b; }
inline double max(double a, double b) { if (a > b) return a; else return b; }

inline int min(int a, int b) { if (a < b) return a; else return b; }
inline double min(double a, double b) { if (a < b) return a; else return b; }

inline int abs(int a) { if (a < 0) return -a; else return a; }
inline double abs(double a) { if (a < 0) return -a; else return a; }

void process_args(int args, char ** argv) {
    if (args > 2) { strcpy(file_name[0], argv[1]); strcpy(file_name[1], argv[2]);  }
    if (args > 3) { max_disparity = atoi(argv[3]); scale = 256 / max_disparity;  }
    if (args > 4) { scale = atoi(argv[4]);  }
    if (args > 6) { strcpy(file_name[2], argv[5]); strcpy(file_name[3], argv[6]);  }
    if (args > 7) { tot_threshold = atof(argv[7]); }
    //if (args > 7) { strcpy(file_name[4], argv[7]);  }
}

/*
BytArray temp;
void median_filter(BytArray a, int h, int w, int radius = 2) {
  //must copy first. otherwise, it's modified while calculating
  for (int i = 0; i < h; ++i)
  for (int j = 0; j < w; ++j)
    temp[i][j] = a[i][j];
  median::median_filter(temp[0], a[0], h, w, radius);
}

void median_filter_rgb(Picture a, int h, int w, int radius = 2) {
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) 
          temp[i][j] = a[i][j][c];
        median::median_filter(temp[0], temp[0], h, w, radius);
        for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
          a[i][j][c] = temp[i][j];
    }
}
*/

// ctmf's version of median.
BytArray tmp4ctmf, tmp4ctmf2;
void median_filter(BytArray a, int h, int w, int radius = 2) {
    for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) tmp4ctmf[i][j] = a[i][j];
    int step = &a[1][1] - &a[0][1];
    ctmf(tmp4ctmf[0], a[0], w, h, step, step, radius, 1, 1024*512);
    //ctmf(tmp4ctmf[0], a[0], w, h, step, step, radius, 1, w * h);
}

void median_filter_rgb(Picture a, int h, int w, int radius = 2) {
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) tmp4ctmf[i][j] = a[i][j][c];
        int step = &tmp4ctmf[1][1] - &tmp4ctmf[0][1];
        ctmf(tmp4ctmf[0], tmp4ctmf2[0], w, h, step, step, radius, 1, 1024*512);
        for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) a[i][j][c] = tmp4ctmf2[i][j];
    }
}

float LABf(float t) 
{
    float k = 903.3;
    float e = 0.008856;
    if (t > e)
        return pow(t, 1.0/3);
    return (k * t + 16.0) / 116.0;
}

float XYZf(int x)
{
    double y = x/255.0;
    if (y > 0.04045)
        return pow((y+0.055)/1.055, 2.4) * 100;
    return y / 12.92 * 100;
}

void RGB_to_LAB(const int * rgb, float * lab)
{
    float X = 0.4124*XYZf(rgb[0]) + 0.3576*XYZf(rgb[1]) + 0.1192*XYZf(rgb[2]);
    float Y = 0.2126*XYZf(rgb[0]) + 0.7152*XYZf(rgb[1]) + 0.0722*XYZf(rgb[2]);
    float Z = 0.0193*XYZf(rgb[0]) + 0.1192*XYZf(rgb[1]) + 0.9505*XYZf(rgb[2]);

    X /= 95.047;
    Y /= 100;
    Z /= 108.883;

    lab[0] = max(0.0, 116.0 * LABf(Y) - 16);
    lab[1] = 500 * (LABf(X) - LABf(Y));
    lab[2] = 200 * (LABf(Y) - LABf(Z));


}

}

#endif
