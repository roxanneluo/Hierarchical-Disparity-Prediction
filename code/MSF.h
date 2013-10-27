#include "gridcode.h"
#include "forests2.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "imageresult.h"

#include "filter.h"

template<class type>
void stereo_matching(Array3<type>& left,
		                 Array3<type>& right,
										 Graph& left_graph,
										 Graph& right_graph,
										 Forest& forest_left,
										 Forest& forest_right,
										 Grid<type>& disparity_left,
										 Grid<type>& disparity_right,
										 const int& max_disparity) {
	Grid<float> left_gradient, right_gradient;
	Array3<double> cost_left, cost_right;

  compute_gradient(left_gradient, left);
	compute_gradient(right_gradient, right);
	compute_first_cost(cost_left, cost_right,
			               left, right,
										 left_gradient, right_gradient, max_disparity);
	for (int i = 0; i < 3; ++i) {
	  median_filter(left[i]);
		median_filter(right[i]);
	}

	left_graph.collect_edges(left);
	right_graph.collect_edges(right);
	left_graph.build_MST();
	right_graph.build_MST();
	// left_graph.build_RandTree(true);
	// right_graph.build_RandTree(true);

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
	printf( "STEREO MATCHING\n");
	
}

