#ifndef _IMAGE_STACK_
#define _IMAGE_STACK_

#include "settings.hpp"

class ImageLayer {
public:
  int H, W;
  Picture rgb;
  FloPicture lab;
  FloArray gradient;

  ImageLayer()
    :H(-1), W(-1){}
  void shrinkPicture(ImageLayer & obj);
  template <typename Dtype>
  //set this's H and W to be half of large H or W
  void shrinkPicture(Dtype small[][MAX_WIDTH][3], Dtype large[][MAX_WIDTH][3], int large_h, int large_w);
  void computeLab();
  void computeGradient();
  void computeLabGradient();
  void computeLabEdGradient(); // euclidean distance

// NOTE: disparity is shrinked into half, too
template <typename Dtype>
static void shrinkDisp(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w);

template <typename Dtype>
static void shrinkGray(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w);

template <typename Dtype>
static void shrinkArray(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w, 
    bool halve_disp);
};



// median filter
/*
void ImageLayer::shrinkPicture(ImageLayer & obj) {
  const int MED_RADIUS = 2;

  obj.H = H/2;
  obj.W = W/2;

  int x,y;
  int a[MED_RADIUS * MED_RADIUS];
  for (int i = 0; i < obj.H; ++i)
  for (int j = 0; j < obj.W; ++j) 
  for (int c = 0; c < 3; ++c) {
    x = 2*i, y = 2*j;
    int cnt = 0;
    for (int ii = 0; ii < 2; ++ii)    
    for (int jj = 0; jj < 2; ++jj)
      a[cnt++] = rgb[x + ii][y + jj][c];
    std::sort(a, a + (MED_RADIUS * MED_RADIUS));
    obj.rgb[i][j][c] = a[1];
  }
}
*/

//mean
void ImageLayer::shrinkPicture(ImageLayer & obj) {
    obj.H = H / 2;
    obj.W = W / 2;
    for (int i = 0; i < H; ++i) 
    for (int k = 0; k < 3; ++k)
        rgb[i][W][k] = rgb[i][W - 1][k];
    for (int i = 0; i < W; ++i) 
    for (int k = 0; k < 3; ++k)
        rgb[H][i][k] = rgb[H - 1][i][k];
    for (int k = 0; k <3; ++k) rgb[H][W][k] = rgb[H-1][W-1][k];
    
    for (int i = 0; i < obj.H; ++i)
    for (int j = 0; j < obj.W; ++j)
    for (int k = 0; k < 3; ++k) {
        int x = i * 2, y = j * 2;
        obj.rgb[i][j][k] = (rgb[x][y][k]+rgb[x+1][y][k]+rgb[x][y+1][k]+rgb[x+1][y+1][k]) / 4;
    }
}

template <typename Dtype>
void ImageLayer::shrinkPicture(Dtype small[][MAX_WIDTH][3], Dtype large[][MAX_WIDTH][3], 
    int large_h, int large_w) {
  const int MED_RADIUS = 2;
  H = large_h/2;
  W = large_w/2;

  int x,y;
  int a[MED_RADIUS * MED_RADIUS];
  for (int i = 0; i < H; ++i)
  for (int j = 0; j < W; ++j) 
  for (int c = 0; c < 3; ++c) {
    x = 2*i, y = 2*j;
    small[i][j][c] = (large[x][y][c]+large[x+1][y][c]+large[x][y+1][c]+large[x+1][y+1][c]) / 4;
  }
}


/*
 * median filter
template <typename Dtype>
void ImageLayer::shrinkPicture(Dtype small[][MAX_WIDTH][3], Dtype large[][MAX_WIDTH][3], 
    int large_h, int large_w) {
  const int MED_RADIUS = 2;
  H = large_h/2;
  W = large_w/2;

  int x,y;
  int a[MED_RADIUS * MED_RADIUS];
  for (int i = 0; i < H; ++i)
  for (int j = 0; j < W; ++j) 
  for (int c = 0; c < 3; ++c) {
    x = 2*i, y = 2*j;
    int cnt = 0;
    for (int ii = 0; ii < 2; ++ii)    
    for (int jj = 0; jj < 2; ++jj)
      a[cnt++] = large[x + ii][y + jj][c];
    std::sort(a, a + (MED_RADIUS * MED_RADIUS));
    small[i][j][c] = a[1];
  }
}
*/

void ImageLayer::computeLab() {
  for (int i = 0; i < H; ++i)
  for (int j = 0; j < W; ++j) 
    misc::RGB_to_LAB(rgb[i][j], lab[i][j]);
}


void ImageLayer::computeGradient () {
    double co[3] = { 0.299, 0.587, 0.114 };
    double plus = 0.5, minus = 0.5, gray;
    for (int x = 0; x < H; ++x) {
        for (int j = 0; j < 3; ++j)  {
            plus += co[j] * rgb[x][1][j]; // * rgb[j][x][1] not rgb[j][x][0]
            minus += co[j] * rgb[x][0][j]; // * rgb[j][x][0] not rgb[j][x][1]
        }
        gray = plus;
        gradient[x][0] = 127.5 + plus - minus;
        for (int i = 1; i <= W - 2; ++i) {
            plus = 0.5;
            for (int j = 0; j < 3; ++j)
                plus += co[j] * rgb[x][i + 1][j];
            gradient[x][i] = 0.5 * (plus - minus) + 127.5;
            minus = gray;
            gray = plus;
        }
        gradient[x][W - 1] = plus - minus + 127.5;
    }
}


void ImageLayer::computeLabGradient () {
    double co[3] = { 0.34, 0.33, 0.33 };
    double plus = 0.5, minus = 0.5, gray;
    for (int x = 0; x < H; ++x) {
        for (int j = 0; j < 3; ++j)  {
            plus += co[j] * lab[x][1][j]; // * lab[j][x][1] not lab[j][x][0]
            minus += co[j] * lab[x][0][j]; // * lab[j][x][0] not lab[j][x][1]
        }
        gray = plus;
        gradient[x][0] =  plus - minus;
        for (int i = 1; i <= W - 2; ++i) {
            plus = 0.5;
            for (int j = 0; j < 3; ++j)
                plus += co[j] * lab[x][i + 1][j];
            gradient[x][i] = 0.5 * (plus - minus);
            minus = gray;
            gray = plus;
        }
        gradient[x][W - 1] = plus - minus;
    }
}


// euclidean length in lab space
void ImageLayer::computeLabEdGradient () {
    double co[3] = { 0.34, 0.33, 0.33 };
    float plus;
    for (int x = 0; x < H; ++x) {
        gradient[x][0] = 0;  
        for (int c = 0; c < 3; ++c)  {
            gradient[x][0] +=  
              (lab[x][1][c] - lab[x][0][c]) * 
              (lab[x][1][c] - lab[x][0][c]);
        }
        gradient[x][0] = sqrt(gradient[x][0]);
        for (int i = 1; i <= W - 2; ++i) {
            plus = 0;
            for (int j = 0; j < 3; ++j)
                plus +=  
                  (lab[x][i + 1][j] - lab[x][i-1][j])
                 *(lab[x][i + 1][j] - lab[x][i-1][j]) ;
            gradient[x][i] = 0.5 * sqrt(plus);
        }
        gradient[x][W - 1] = 0;
        for (int c = 0; c < 3; ++c)
            gradient[x][W - 1] +=
              (lab[x][W-1][c] - lab[x][W-2][c]) * 
              (lab[x][W-1][c] - lab[x][W-2][c]);
        gradient[x][W-1] = sqrt(gradient[x][W-1]);
    }
}

template <typename Dtype>
void ImageLayer::shrinkDisp(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w) {
  shrinkArray(small, small_h, small_w, large, large_h, large_w, true);
}

template <typename Dtype>
void ImageLayer::shrinkGray(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w) {
  shrinkArray(small, small_h, small_w, large, large_h, large_w, false);
}


template <typename Dtype>
void ImageLayer::shrinkArray(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w, 
    bool halve_disp) {
  const int MED_RADIUS = 2;
  small_h = large_h/2;
  small_w = large_w/2;

  int x,y;
  double divisor = halve_disp? 8:4;
  for (int i = 0; i < small_h; ++i)
  for (int j = 0; j < small_w; ++j) { 
    x = 2*i, y = 2*j;
    small[i][j]= (large[x][y]+large[x+1][y]+large[x][y+1]+large[x+1][y+1]) / divisor;
  }
}

/*
 * median filter
// NOTE: disparity is shrinked into half, too
template <typename Dtype>
void ImageLayer::shrinkArray(Dtype small [][MAX_WIDTH], int &small_h, int &small_w,
    const Dtype large[][MAX_WIDTH], int large_h, int large_w, 
    bool halve_disp) {
  const int MED_RADIUS = 2;
  small_h = large_h/2;
  small_w = large_w/2;

  int x,y;
  int a[MED_RADIUS * MED_RADIUS];
  for (int i = 0; i < small_h; ++i)
  for (int j = 0; j < small_w; ++j) { 
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

#endif
