/* This code is the implemention 
    of the probability driven algorithm of MSF */

#include <vector>
#include <cstdlib>
#include "gridcode.h"
#include "arithmetics.h"
#include "scale.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
#include "MSF2.h"
#include "filter.h"
#include "algorithm"
#include "support_match.h"
#include "gen_interval.h"

using namespace std;

char file_name[5][300] =
    {"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm", "tsukuba"};

int max_disparity = 16;
int scale = 16;
const int level = 3;

TimeKeeper timer;

// typedef Array3<unsigned char> ARRAY3;
// typedef Grid<unsigned char> GRID;

vector< Array3<unsigned char> > rgb_left(level), rgb_right(level); // int -> unsigned char
vector< Grid<unsigned char> > disparity_left(level + 1), disparity_right(level + 1); // int -> unsigned char

vector<Graph> graph_left(level), graph_right(level);
vector<Forest> forest_left(level), forest_right(level);

vector< Grid<unsigned char> > occlusion_left(level), occlusion_right(level);

// for image result
Forest left_support_forest, right_support_forest;
Grid<unsigned char> left_support_map[level], right_support_map[level];
Grid<unsigned char> left_tree_img, right_tree_img;
Array1<double> support_prob;
Array1<double> initial_prob;
Grid<double> prob_matrix;
Grid<int> interval;
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
void refinement(int i, int d_max) {
  // find stable pixels by using left-right consisty check

  Array3<double> cost_left, cost_right;
  cost_left.reset(d_max, disparity_left[i].height, disparity_left[i].width);
  cost_right.reset(d_max, disparity_right[i].height, disparity_right[i].width);
  
	// update_matching_cost(cost_left, cost_right, disparity_left[i], disparity_right[i],
  //     occlusion_left[i], occlusion_right[i]);
  
  forest_left[i].update_matching_cost_on_tree_with_interval(cost_left, disparity_left[i], occlusion_left[i]);
	forest_right[i].update_matching_cost_on_tree_with_interval(cost_right, disparity_right[i], occlusion_right[i]);

	forest_left[i].compute_cost_on_tree_with_interval(cost_left, 255 * 0.05);
  forest_right[i].compute_cost_on_tree_with_interval(cost_right, 255 * 0.05);

	// compute_disparity(cost_left, disparity_left[i]);
  // compute_disparity(cost_right, disparity_right[i]);
  
  forest_left[i].compute_disparity_on_tree_with_interval(cost_left, disparity_left[i]);
	forest_right[i].compute_disparity_on_tree_with_interval(cost_right, disparity_right[i]);
	
	median_filter(disparity_left[i]);
  median_filter(disparity_right[i]);
  // save_image("lefterror.pgm", occlusion_left[i]);
  // save_image("righterror.pgm", occlusion_right[i]);
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
  // SupportMatch sm(rgb_left[0], rgb_right[0],
  //    rgb_left[0].width, rgb_left[0].height);
  // Find support match for left and right image respectively.
  // sm.FindSupportMatch();

  // Get scaled image.
  for (int i = 0; i < level - 1; ++i) {
    scale_image(rgb_left[i], rgb_left[i + 1]);
    scale_image(rgb_right[i], rgb_right[i + 1]);
  }

  int pi[level + 1];
  memset(pi, 0, sizeof(pi));
  pi[0] = 1;
  for (int i = 1; i <= level; ++i) {
    pi[i] = 2 * pi[i - 1];
  }
  
	// support_prob.reset(max_disparity + 1); 
	// gen_support_prob(sm.support_left_map, sm.support_right_map, support_prob, max_disparity);  
  
  Array1<double> gmm;
  // Array1<double> gmm0, gmm1; 
 	
  for (int i = level - 1; i >= 0; --i) {
    bool is_highest_layer = (i == level - 1) ? true : false;
    bool is_lowest_layer = (i == 0) ? true : false;
    Array3<double> cost_left, cost_right;

    // disparity prediction model
    if (is_highest_layer) { 
			interval.reset(max_disparity / pi[i + 1]/* + 1*/, 2);
			
			for (int j = 0; j < interval.height; ++j) {
			  interval[j][0] = 0;
				interval[j][1] = max_disparity / pi[i] - 1;
			
			}	
			disparity_left[level].reset(rgb_left[i].height / 2, rgb_left[i].width / 2);
			disparity_left[level].zero();
			disparity_right[level].reset(rgb_right[i].height / 2, rgb_right[i].width / 2);
			disparity_right[level].zero();
		} else {
			// left vector.
	    SupportMatch sm(rgb_left[i], rgb_right[i], rgb_left[i].width, rgb_left[i].height);
	    	  
            sm.FindSupportMatch();
    	support_prob.reset(max_disparity / pi[i] /*+ 1*/); 
	    gen_support_prob(sm.support_left_map, sm.support_right_map, support_prob, max_disparity / pi[i]); 
			initial_prob.reset(max_disparity / pi[i + 1] /*+ 1*/);
			gen_initial_prob(disparity_left[i + 1], disparity_right[i + 1], initial_prob, max_disparity / pi[i + 1]);
			
			if (args > 7)
			  save_initial_cnt(initial_prob, file_name[4]);

			initial_prob.normalize();
			// small_given_large matrix.
			prob_matrix.reset(max_disparity / pi[i + 1] /*+ 1*/,
					              max_disparity / pi[i] /*+ 1*/);
			gen_small_given_large(prob_matrix, gmm, i);	
      
			gen_large_given_small(initial_prob, prob_matrix, support_prob);
			// generate disparity interval.
      // printf("XXXXXXXXXXXXXXX %d\n", i);
      // fflush(stdout);
			interval.reset(prob_matrix.height, 2);
			// need experiments.	
			gen_interval_mid(prob_matrix, interval, 0.001 * pi[i] /*0.001*/);
			if (args > 7)
				save_interval(interval, file_name[4]);
    }
     // if (is_highest_layer) {
      // (@xuejiaobai) have no idea but the results are different.
			Array3<unsigned char> copy_rgb_left, copy_rgb_right;
			copy_rgb_left.copy(rgb_left[i]);
			copy_rgb_right.copy(rgb_right[i]);
		  // compute_first_matching_cost(rgb_left[i], rgb_right[i],
      //    cost_left, cost_right, disparity_left[i + 1], interval, max_disparity / pi[i]);
      
			build_tree_with_interval(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
          forest_left[i], forest_right[i],
					disparity_left[i + 1], disparity_right[i + 1], interval,
					0.95, /*true,*/ !is_lowest_layer, false);
	
		  compute_first_matching_cost(copy_rgb_left, copy_rgb_right, cost_left, cost_right, forest_left[i], forest_right[i], max_disparity / pi[i]);	
			disparity_computation(forest_left[i], forest_right[i],
          cost_left, cost_right, disparity_left[i], disparity_right[i],
					disparity_left[i + 1], disparity_right[i + 1], interval);
			
			/*
			compute_first_matching_cost(rgb_left[i], rgb_right[i],
					cost_left, cost_right, max_disparity / pi[i]);
			build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
			 		forest_left[i], forest_right[i]);
			disparity_computation(forest_left[i], forest_right[i],
			 		cost_left, cost_right, disparity_left[i], disparity_right[i]);
		  */
		/*	
		  } else {
      // Not the highest layer. BUILD forest from here!
      compute_first_matching_cost(rgb_left[i], rgb_right[i],
          cost_left, cost_right, disparity_left[i + 1], interval, max_disparity / pi[i]);
      build_tree_with_interval(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
          forest_left[i], forest_right[i], 
          disparity_left[i + 1], disparity_right[i + 1], interval, 
          0.9 //  the threshold
          );
      disparity_computation(forest_left[i], forest_right[i],
          cost_left, cost_right, disparity_left[i], disparity_right[i],
				  disparity_left[i + 1], disparity_right[i + 1], interval);
    }*/
		
		find_stable_pixels(disparity_left[i], disparity_right[i],
           occlusion_left[i], occlusion_right[i]);

    refinement(i, max_disparity / pi[i]);
  }

    save_image(file_name[2], disparity_left[0], scale);
    save_image(file_name[3], disparity_right[0], scale);
    timer.check("all   ");
    //for (int i = 0; i < level; ++i)
       // draw_tree(0);
  return 0;
}
