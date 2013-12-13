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

char file_name[6][300] =
    {"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm", "filter_left", "MST"};


Array3<unsigned char> rgb_left, rgb_right; // int -> unsigned char
Grid<unsigned char> disparity_left, disparity_right; // int -> unsigned char
Grid<unsigned char> disparity_left_ctmf, disparity_right_ctmf;
Grid<float> left_gradient, right_gradient;
Array3<double> cost_left, cost_right;
Graph left_graph, right_graph;
Forest forest_left, forest_right;
int max_disparity = 16;
int scale = 16;


Grid<unsigned char> occlusion_left, occlusion_right;

// for image result
Forest left_support_forest, right_support_forest;
Array3<unsigned char> left_support_map, right_support_map;
Grid<unsigned char> left_tree_img, right_tree_img;
Grid<unsigned char> left_occ_img, right_occ_img;

// for input x, y
const int num = 5;
int x[num], y[num];


template <class type>
void refinement(Grid<type>& d_left, Grid<type>& d_right) {
  // find stable pixels by using left-right consisty check
  find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
  update_matching_cost(cost_left, cost_right, d_left, d_right,
      occlusion_left, occlusion_right);

  forest_left.compute_cost_on_tree(cost_left, 255 * 0.05);
  forest_right.compute_cost_on_tree(cost_right, 255 * 0.05);
  compute_disparity(cost_left, d_left);
  compute_disparity(cost_right, d_right);
}


int main(int args, char ** argv) {
  if (args > 2) {
    strcpy(file_name[0], argv[1]);
    strcpy(file_name[1], argv[2]);
  }
  if (args > 3) {
    max_disparity = atoi(argv[3]);
    scale = 256 / max_disparity;
  }
  if (args > 4) {
    scale = atoi(argv[4]);
  }
  if (args > 6) {
    strcpy(file_name[2], argv[5]);
    strcpy(file_name[3], argv[6]);
  }
  if (args > 7) {
    strcpy(file_name[4], argv[7]);
    //strcpy(file_name[5], argv[8]);
  }
  if (args > 8) 
    strcpy(file_name[5], argv[8]);
  if (args > 18) {
    int start = 9;
    for (int i = 0; i < num; ++i){
      x[i] = atoi(argv[start++]);
      y[i] = atoi(argv[start++]);
    }
  }

  try {
    load_image(file_name[0], rgb_left);
    load_image(file_name[1], rgb_right);
  } catch (...) {
    puts("Error loading file");
    return 0;
  }
//  const int h = rgb_left.height;
//  const int w = rgb_left.width;

  compute_gradient(left_gradient, rgb_left);
  compute_gradient(right_gradient, rgb_right);
  compute_first_cost(cost_left, cost_right, rgb_left, rgb_right,
      left_gradient, right_gradient, max_disparity);
  for (int i = 0; i < 3; ++i){
    median_filter(rgb_left[i], 1);
    median_filter(rgb_right[i], 1);
  }

  left_graph.collect_edges(rgb_left);
  right_graph.collect_edges(rgb_right);
  left_graph.build_RandTree();
  right_graph.build_RandTree();

  forest_left.init(left_graph);
  forest_right.init(right_graph);
  forest_left.order_of_visit();
  forest_right.order_of_visit();
  
  forest_left.compute_cost_on_tree(cost_left);
  forest_right.compute_cost_on_tree(cost_right);
  compute_disparity(cost_left, disparity_left);
  compute_disparity(cost_right, disparity_right);
  median_filter(disparity_left);
  median_filter(disparity_right);
  find_stable_pixels(disparity_left, disparity_right, occlusion_left, occlusion_right);
  show_stable_pixels(disparity_left, occlusion_left, left_occ_img);
  show_stable_pixels(disparity_right, occlusion_right, right_occ_img);
  save_image(file_name[2], left_occ_img, scale);
  save_image(file_name[3], right_occ_img, scale);

  int cnt = 0;
  for (int i = 0; i < 5; ++i) {
   // if (bad_point(x,y,occlusion_left)) {
      draw_support_map(x[i],y[i],file_name[4],++cnt,file_name[5],left_support_forest, left_support_map, left_occ_img,left_graph, scale);
    //}
  }
  printf("%d\n", cnt);


  return 0;
}
