#include "gridcode.h"
#include "forests.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"

#include "filter.h"


char file_name[2][30] = {"left.ppm", "right.ppm"};

Array3<unsigned char> rgb_left, rgb_right; // int -> unsigned char
Grid<unsigned char> disparity_left, disparity_left_ctmf, disparity_right, disparity_right_ctmf; // int -> unsigned char
Grid<float> left_gradient, right_gradient;
Array3<double> cost_left, cost_right;
Graph left_graph, right_graph;
Forest forest_left, forest_right;
int max_disparity = 16;
int scale = 16;

TimeKeeper timer;

Grid<unsigned char> occlusion_left, occlusion_right;

template <class type>
void refinement(Grid<type>& d_left, Grid<type>& d_right) 
{
	find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
	update_matching_cost(cost_left, cost_right, d_left, d_right, occlusion_left, occlusion_right);
	forest_left.compute_cost_on_tree(cost_left, 255 * 0.05);
	forest_right.compute_cost_on_tree(cost_right, 255 * 0.05);
timer.check("refinement   ");
	compute_disparity(cost_left, d_left);
	compute_disparity(cost_right, d_right);
}


int main(int args, char ** arg) {
	if (args >= 2) {
		strcpy(file_name[0], arg[1]);
		strcpy(file_name[1], arg[2]);
    }
    if (args >= 3) {
		max_disparity = atoi(arg[3]);
		scale = 256 / max_disparity;
    } 
    if (args >= 4) {
        scale = atoi(arg[4]);
    }
timer.reset();
	load_image(file_name[0], rgb_left);
	load_image(file_name[1], rgb_right);
//	const int h = rgb_left.height;
//	const int w = rgb_left.width;

    
timer.check("load         ");
    compute_gradient(left_gradient, rgb_left);
	compute_gradient(right_gradient, rgb_right);
timer.check("gradient     ");
	compute_first_cost(cost_left, cost_right, rgb_left, rgb_right, left_gradient, right_gradient, max_disparity); // ERROR: use exact max_disparity
    
timer.check("initial cost ");
    for (int i = 0; i < 3; ++i){
        median_filter(rgb_left[i]);
        median_filter(rgb_right[i]);
    }

timer.check("ctmf for     ");
    left_graph.collect_edges(rgb_left);
	right_graph.collect_edges(rgb_right);
	left_graph.build_MST();
	right_graph.build_MST();
	forest_left.init(left_graph);
	forest_right.init(right_graph);
	forest_left.order_of_visit();
	forest_right.order_of_visit();
timer.check("MST          ");
	forest_left.compute_cost_on_tree(cost_left);
	forest_right.compute_cost_on_tree(cost_right);
timer.check("update a-cost");
	compute_disparity(cost_left, disparity_left);
	compute_disparity(cost_right, disparity_right);
    median_filter(disparity_left);
    median_filter(disparity_right);
timer.check("unref-dispar ");
    refinement(disparity_left, disparity_right);
    median_filter(disparity_left);
    median_filter(disparity_right);
    
	//disparity_left_ctmf.reset(h, w);
	//disparity_right_ctmf.reset(h, w);
	// disparity_left_ctmf.copy(disparity_left);
	// disparity_right_ctmf.copy(disparity_right);
	// ctmf(disparity_right[0], disparity_right_ctmf[0], w, h, w, w, 2, 1, w * h);
timer.check("get disparity");
	save_image("leftdisparity_ctmf_refined_morefil2.pgm", disparity_left, scale);
	//save_image("leftdisparity_ctmf.pgm", disparity_left_ctmf, scale);
	save_image("rightdisparity_ctmf.pgm", disparity_right, scale);
	//save_image("rightdisparity_ctmf.pgm", disparity_right_ctmf, scale);
timer.get_total_time();
    return 0;
}
