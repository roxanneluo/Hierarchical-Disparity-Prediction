#include <cstdlib>
#include "gridcode.h"
#include "forests1.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
#include "imageresult.h"
#include "filename.h"
#include "filter.h"

char file_name[2][300] =
    {"white_black.ppm","white_black_filter.ppm"};


Array3<unsigned char> rgb_left, rgb_right; // int -> unsigned char
/*Grid<unsigned char> disparity_left, disparity_right; // int -> unsigned char
Grid<unsigned char> disparity_left_ctmf, disparity_right_ctmf;
Grid<float> left_gradient, right_gradient;
Array3<double> cost_left, cost_right;
*/Graph left_graph, right_graph;
// Forest forest_left, forest_right;
/*int max_disparity = 16;
*/
double scale = 0.4;

Grid<unsigned char> occlusion_left, occlusion_right;

// for image result
Forest left_support_forest, right_support_forest;
Array3<unsigned char> left_support_map, right_support_map;
Grid<unsigned char> left_tree_img, right_tree_img;
Grid<unsigned char> left_gray_img, right_occ_img;

Grid<double> support_ratio;
// for input x, y
const int MAX_NUM = 20;
int num = 9;
int x[MAX_NUM]={25,25,25,40,40,40,50,50,50}, y[MAX_NUM] = {10,50,90,10,50,90,10,50,90};

int findBorder(int * border) {
  int W = rgb_left.width, H = rgb_left.height;
  int num = 0;
  int lastcolor = rgb_left[0][0][0];
  for (int j = 0; j < W; ++j) {
    if (rgb_left[0][0][j] != lastcolor) {
      border[num++] = j;
      lastcolor = rgb_left[0][0][j];
    }
  }
  return num;
}

int main(int args, char ** argv) {
  if (args > 1) {
    strcpy(file_name[0], argv[1]);
  }
  if (args > 2) {
    strcpy(file_name[1], argv[2]);
  }
  if (args > 3) {
    int start = 3;
    num = mylib::min((args-3)/2,MAX_NUM);
    for (int i = 0; i < num; ++i){
      x[i] = atoi(argv[start++]);
      y[i] = atoi(argv[start++]);
    }
  }
  
  try {
    load_image(file_name[0], rgb_left);
  } catch (...) {
    puts("Error loading file");
    return 0;
  }
  rgb2gray(left_gray_img, rgb_left);
  left_graph.collect_edges(rgb_left);
  left_graph.build_RandTree();

  int cnt = 0;
  // for (int i = 0; i < num; ++i) {
  //  // if (bad_point(x,y,occlusion_left)) {
  //     draw_support_map(x[i],y[i],file_name[1],++cnt,"RAND",left_support_forest, left_support_map, left_gray_img,left_graph, scale);
  //   //}
  // }
  // printf("%d\n", cnt);

  left_support_forest.init(left_graph);
  // int border[10], bordernum=0;
  // bordernum = findBorder(border);
  printf("%d,%d\n", rgb_left[0][50][49], rgb_left[0][50][50]);
  int border[2]={25, 75}, bordernum = 2;

  int sum = 0;
  int H = rgb_left.height, W = rgb_left.width;
  draw_support_map(49,0,file_name[1],++cnt,"RAND",left_support_forest, left_support_map, left_gray_img,left_graph, scale);
  draw_support_map(50,0,file_name[1],++cnt,"RAND",left_support_forest, left_support_map, left_gray_img,left_graph, scale);
  for (int n = 0; n < bordernum; ++n) {
    int j = border[n];
    for (int i = 0; i < rgb_left.height; ++i) {
       printf("%d\n", left_support_forest.findSupportSize(i,j, support_ratio, H, W));
       printf("%d\n", left_support_forest.findSupportSize(i,j-1, support_ratio, H, W));


      sum += left_support_forest.findSupportSize(i,j, support_ratio, H, W);
      sum += left_support_forest.findSupportSize(i,j-1, support_ratio, H, W);
    }
  }
  printf("avg bordersize = %f\n", double(sum)/(2*bordernum*rgb_left.height));
  return 0;
}
