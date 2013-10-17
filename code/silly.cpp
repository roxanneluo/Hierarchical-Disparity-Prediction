#include "gridcode.h"
#include "forests.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
#include "imageresult.h"

#include "filter.h"


char file_name[4][300] =
    {"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm"};

Array3<unsigned char> rgb_left, rgb_right; // int -> unsigned char
Grid<unsigned char> disparity_left, disparity_right; // int -> unsigned char
Grid<unsigned char> disparity_left_ctmf, disparity_right_ctmf;
Grid<float> left_gradient, right_gradient;
Array3<double> cost_left, cost_right;
Graph left_graph, right_graph;
Forest forest_left, forest_right;
int max_disparity = 16;
int scale = 16;

int w = 1, b = 8;
double a = 1;

//TimeKeeper timer;

Grid<unsigned char> occlusion_left, occlusion_right;

// for image result
Forest left_support_forest, right_support_forest;
Grid<unsigned char> left_support_map, right_support_map;
Grid<unsigned char> left_tree_img, right_tree_img;

template <class type>
void refinement(Grid<type>& d_left, Grid<type>& d_right) {
  // find stable pixels by using left-right consisty check
  find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
  update_matching_cost(cost_left, cost_right, d_left, d_right,
      occlusion_left, occlusion_right);

  forest_left.compute_cost_on_tree(cost_left, 255 * 0.05);
  forest_right.compute_cost_on_tree(cost_right, 255 * 0.05);
  //timer.check("refinement   ");
  compute_disparity(cost_left, d_left);
  compute_disparity(cost_right, d_right);
}

void draw_tree() { 
  int times = 3;
  int x = 252, y = 167;
  draw_tree_and_RGBimage(left_tree_img, rgb_left,
      left_graph.trees, left_graph.n, times);
  draw_tree_and_RGBimage(right_tree_img, rgb_right,
      right_graph.trees, right_graph.n, times);
  left_support_forest.init(left_graph);
  right_support_forest.init(right_graph);
  draw_support_map(left_support_map, left_support_forest,
      left_graph, x, y, times);
  draw_support_map(right_support_map, right_support_forest,
      right_graph, x, y, times);
  save_image("MST_lefttreeimage.pgm", left_tree_img);
  save_image("MST_righttreeimage.pgm", right_tree_img);
  save_image("MST_leftsupportmap.pgm", left_support_map);
  save_image("MST_rightsupportmap.pgm", right_support_map);
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

//timer.reset();
  try {
    load_image(file_name[0], rgb_left);
    load_image(file_name[1], rgb_right);
  } catch (...) {
    puts("Error loading file");
    return 0;
  }
//	const int h = rgb_left.height;
//	const int w = rgb_left.width;

//timer.check("load         ");
  compute_gradient(left_gradient, rgb_left);
  compute_gradient(right_gradient, rgb_right);
//timer.check("gradient     ");
  compute_first_cost(cost_left, cost_right, rgb_left, rgb_right,
      left_gradient, right_gradient, max_disparity);
//timer.check("initial cost ");
  for (int i = 0; i < 3; ++i){
    median_filter(rgb_left[i]);
    median_filter(rgb_right[i]);
  }

//timer.check("ctmf for     ");
  left_graph.collect_edges(rgb_left, false, w, a, b);
  right_graph.collect_edges(rgb_right, false, w, a, b);
  left_graph.build_MST();
  right_graph.build_MST();

  forest_left.init(left_graph);
  forest_right.init(right_graph);
  forest_left.order_of_visit();
  forest_right.order_of_visit();

  draw_tree();

//timer.check("MST          ");
  forest_left.compute_cost_on_tree(cost_left);
  forest_right.compute_cost_on_tree(cost_right);
//timer.check("update a-cost");
  compute_disparity(cost_left, disparity_left);
  compute_disparity(cost_right, disparity_right);
  median_filter(disparity_left);
  median_filter(disparity_right);
//timer.check("unref-dispar ");
  refinement(disparity_left, disparity_right);
  median_filter(disparity_left);
  median_filter(disparity_right);

//timer.check("get disparity");
  save_image(file_name[2], disparity_left, scale);
  save_image(file_name[3], disparity_right, scale);
//timer.get_total_time();
  return 0;
}
