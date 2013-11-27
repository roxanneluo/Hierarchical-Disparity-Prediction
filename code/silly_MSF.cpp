#include <vector>
#include <cstdlib>
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
#include "support_match.h"
#include "gen_interval.h"

using namespace std;

char file_name[5][300] =
    {"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm", "tsukuba"};

int max_disparity = 16;
int scale = 16;
const int level = 2;

// typedef Array3<unsigned char> ARRAY3;
// typedef Grid<unsigned char> GRID;

vector< Array3<unsigned char> > rgb_left(level), rgb_right(level); // int -> unsigned char
vector< Grid<unsigned char> > disparity_left(level), disparity_right(level); // int -> unsigned char

vector<Graph> graph_left(level), graph_right(level);
vector<Forest> forest_left(level), forest_right(level);

vector< Grid<unsigned char> > occlusion_left(level), occlusion_right(level);

// for image result
Forest left_support_forest, right_support_forest;
Grid<unsigned char> left_support_map[level], right_support_map[level];
Grid<unsigned char> left_tree_img, right_tree_img;
Array1<double> support_prob_left, support_prob_right;
Array1<double> initial_prob_left, initial_prob_right;
Grid<double> prob_matrix_left, prob_matrix_right;

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
void refinement(int i) {
  // find stable pixels by using left-right consisty check
  // left_graph.build_MST();
  // right_graph.build_MST();

  // forest_left.init(left_graph);
  // forest_right.init(right_graph);
  // forest_left.order_of_visit();
  // forest_right.order_of_visit();
	Array3<double> cost_left, cost_right;
  cost_left.reset(max_disparity, disparity_left[i].height, disparity_left[i].width);
	cost_right.reset(max_disparity, disparity_right[i].height, disparity_right[i].width);
  // find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
  update_matching_cost(cost_left, cost_right, disparity_left[i], disparity_right[i],
      occlusion_left[i], occlusion_right[i]);
  forest_left[i].compute_cost_on_tree(cost_left, 255 * 0.05);
  forest_right[i].compute_cost_on_tree(cost_right, 255 * 0.05);
  compute_disparity(cost_left, disparity_left[i]);
  compute_disparity(cost_right, disparity_right[i]);
	median_filter(disparity_left[i]);
	median_filter(disparity_right[i]);
  save_image("lefterror.pgm", occlusion_left[i]);
  save_image("righterror.pgm", occlusion_right[i]);
} /*
char *get_file_name(char *filename, const char * prefix, int para, const char * suffix) {
    strcpy(filename, prefix);
    strcat(filename, "_");
    char num[10] = {char(para),'\0'};
    //itoa(para, num, 10);
    strcat(filename, num);
    strcat(filename, "_");
    strcat(filename, suffix);
    return filename;
}
void draw_tree(int level_index) {
    int times = 3;
    int x = 100, y = 80;
    left_support_forest.init(graph_left[level_index]);
    right_support_forest.init(graph_right[level_index]);
    draw_support_map(left_support_map[level_index], left_support_forest,
      graph_left[level_index], x, y, times);
    draw_support_map(right_support_map[level_index], right_support_forest,
      graph_right[level], x, y, times);

    char filename[50];
    save_image(get_file_name(filename,"MSF_left_supportmap",level_index,".pgm"), left_support_map[level_index]);
    save_image(get_file_name(filename,"MSF_right_supportmap",level_index,".pgm"), right_support_map[level_index]);
}*/
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
	}

  try {
    load_image(file_name[0], rgb_left[0]);
    load_image(file_name[1], rgb_right[0]);
  } catch (...) {
    puts("Error loading file");
    return 0;
  }

  // Get support image.
	SupportMatch sm(rgb_left[0], rgb_right[0],
			rgb_left[0].width, rgb_left[0].height);
	// Find support match for left and right image respectively.
  sm.FindSupportMatch();

  // Check the reliable points for left and right image.
  // NOTICE(xuejiaobai): variable scale is used in the output maps.	
	// save_image(strcat(file_name[0], "_left_initial_reliable_points.pgm"),
	// 		       sm.support_left_map, scale);
	// save_image(strcat(file_name[1], "_right_initial_reliable_points.pgm"),
	// 		       sm.support_right_map, scale);

	// Get scaled image.
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

	support_prob_left.reset(max_disparity + 1);
	support_prob_right.reset(max_disparity + 1);
	gen_support_prob(sm.support_left_map, support_prob_left, max_disparity);
	gen_support_prob(sm.support_right_map, support_prob_right, max_disparity);

	vector<double> gmm = read_prob_matrix("gmm");

	for (int i = level - 1; i >= 0; --i) {
		bool is_highest_layer = (i == level - 1) ? true : false;
    bool is_lowest_layer = (i == 0) ? true : false;
    Array3<double> cost_left, cost_right;
    if (is_highest_layer) {
		  compute_first_matching_cost(rgb_left[i], rgb_right[i],
				  cost_left, cost_right, max_disparity / pi[i]);
			build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
				  forest_left[i], forest_right[i]);
		} else {
			// Not the highest layer. BUILD forest from here!
			compute_first_matching_cost(rgb_left[i], rgb_right[i],
					cost_left, cost_right, disparity_left[i + 1], max_disparity / pi[i]);
		  build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
				  forest_left[i], forest_right[i],
					// disparity_left[i + 1], disparity_right[i + 1]
					occlusion_left[i + 1], occlusion_right[i + 1]
					);
		}
		disparity_computation(forest_left[i], forest_right[i],
				cost_left, cost_right, disparity_left[i], disparity_right[i]);
	  
// 		find_stable_pixels(disparity_left[i], disparity_right[i],
// 		  	 occlusion_left[i], occlusion_right[i]);
	  
// 		refinement(i);
		
		// disparity prediction model
		if (!is_lowest_layer) {
		  initial_prob_left.reset(max_disparity / pi[i] + 1);
		  initial_prob_right.reset(max_disparity / pi[i] + 1);
			// generate initial prob vector for level i + 1.
      gen_initial_prob(disparity_left[i], initial_prob_left, max_disparity / pi[i]);
		  gen_initial_prob(disparity_right[i], initial_prob_right, max_disparity / pi[i]);
			
		  prob_matrix_left.reset(max_disparity / pi[i] + 1,
			  	                   max_disparity / pi[i] * 2 - 1);

		  prob_matrix_right.reset(max_disparity / pi[i] + 1,
			  	                    max_disparity / pi[i] * 2 - 1);
      // generate small given large matrix.
	    gen_small_given_large(prob_matrix_left, gmm);	
      gen_small_given_large(prob_matrix_right, gmm);
      // generate large given small matrix.
		  gen_large_given_small(initial_prob_left,
			  	                  prob_matrix_left,
				  									support_prob_left);
		  gen_large_given_small(initial_prob_right,
			  	                  prob_matrix_right,
				  									support_prob_right);
		  // Output for test.
			if (args > 7)
			  save_large_given_small(prob_matrix_left, file_name[4]);
		}
		find_stable_pixels(disparity_left[i], disparity_right[i],
		  	 occlusion_left[i], occlusion_right[i]);
	  
		refinement(i);

	}

    save_image(file_name[2], disparity_left[0], scale);
    save_image(file_name[3], disparity_right[0], scale);

    //for (int i = 0; i < level; ++i)
       // draw_tree(0);
	return 0;
}
