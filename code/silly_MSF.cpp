#include "gridcode.h"
#include "arithmetics.h"
#include "scale.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
//#include "imageresult.h"
#include "MSF.h"
#include "filter.h"
#include "algorithm"
#include <vector>

using namespace std;

char file_name[4][300] =
    {"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm"};

int max_disparity = 16;
int scale = 16;
int level = 3;

// typedef Array3<unsigned char> ARRAY3;
// typedef Grid<unsigned char> GRID;

vector< Array3<unsigned char> > rgb_left(level), rgb_right(level); // int -> unsigned char
vector< Grid<unsigned char> > disparity_left(level), disparity_right(level); // int -> unsigned char

vector<Graph> graph_left(level), graph_right(level);
vector<Forest> forest_left(level), forest_right(level);


vector< Grid<unsigned char> > occlusion_left(level), occlusion_right(level);

// for image result
Forest left_support_forest, right_support_forest;
Grid<unsigned char> left_support_map, right_support_map;
Grid<unsigned char> left_tree_img, right_tree_img;
/*
void draw_tree() {
  int times = 3;
  int x = 252, y = 167;
  draw_tree_and_RGBimage(left_tree_img, rgb_left[0],
      graph_left[0].trees, graph_left[0].n, times);
  draw_tree_and_RGBimage(right_tree_img, rgb_right[0],
      graph_right[0].trees, graph_right[0].n, times);
  left_support_forest.init(graph_left[0]);
  right_support_forest.init(graph_right[0]);
  draw_support_map(left_support_map, left_support_forest,
      graph_left[0], x, y, times);
  draw_support_map(right_support_map, right_support_forest,
      graph_right[0], x, y, times);
  save_image("RandTree_lefttreeimage.pgm", left_tree_img);
  save_image("RandTree_righttreeimage.pgm", right_tree_img);
  save_image("RandTree_leftsupportmap.pgm", left_support_map);
  save_image("RandTree_rightsupportmap.pgm", right_support_map);
}
*/
template <class type>
void refinement(Grid<type>& d_left, Grid<type>& d_right,
		Grid<type>& occ_left, Grid<type>& occ_right) {
  // find stable pixels by using left-right consisty check
  // left_graph.build_MST();
  // right_graph.build_MST();

  // forest_left.init(left_graph);
  // forest_right.init(right_graph);
  // forest_left.order_of_visit();
  // forest_right.order_of_visit();
 
	Array3<double> cost_left, cost_right;
  cost_left.reset(max_disparity, d_left.height, d_left.width);
	cost_right.reset(max_disparity, d_right.height, d_right.width);
  // find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
  update_matching_cost(cost_left, cost_right, d_left, d_right,
      occ_left, occ_right);

  forest_left[0].compute_cost_on_tree(cost_left, 255 * 0.05);
  forest_right[0].compute_cost_on_tree(cost_right, 255 * 0.05);
  compute_disparity(cost_left, d_left);
  compute_disparity(cost_right, d_right);
  save_image("lefterror.pgm", occ_left);
  save_image("righterror.pgm", occ_right);
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

  try {
    load_image(file_name[0], rgb_left[0]);
    load_image(file_name[1], rgb_right[0]);
  } catch (...) {
    puts("Error loading file");
    return 0;
  }

  for (int i = 0; i < level - 1; ++i) {
	  scale_image(rgb_left[i], rgb_left[i + 1]);
		scale_image(rgb_right[i], rgb_right[i + 1]);
	}

  int pi[level];
	memset(pi, 0, sizeof(pi));
	pi[0] = 1;
	for (int i = 1; i < level; ++i) {
	  pi[i] = 2 * pi[i - 1];
	}

	for (int i = level - 1; i >= 0; --i) {
		bool is_highest_layer = (i == level - 1) ? true : false;

    Array3<double> cost_left, cost_right;
    if (is_highest_layer) {
		  compute_first_matching_cost(rgb_left[i], rgb_right[i],
				  cost_left, cost_right, max_disparity / pi[i]);
			build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
				  forest_left[i], forest_right[i]);
		} else {
			compute_first_matching_cost(rgb_left[i], rgb_right[i],
					cost_left, cost_right, disparity_left[i + 1], max_disparity / pi[i]);
		  build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
				  forest_left[i], forest_right[i], occlusion_left[i + 1], occlusion_right[i + 1]);
		}
		disparity_computation(forest_left[i], forest_right[i],
				cost_left, cost_right, disparity_left[i], disparity_right[i]);

	  find_stable_pixels(disparity_left[i], disparity_right[i],
		  	 occlusion_left[i], occlusion_right[i]);
	}
  refinement(disparity_left[0], disparity_right[0], occlusion_left[0], occlusion_right[0]);
  median_filter(disparity_left[0]);
  median_filter(disparity_right[0]);

  save_image(file_name[2], disparity_left[0], scale);
  save_image(file_name[3], disparity_right[0], scale);
	return 0;
}
