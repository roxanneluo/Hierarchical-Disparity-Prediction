#include "gridcode.h"
#include "forests2.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "imageresult.h"

#include "filter.h"


// Compute matching cost only for the highest layer.
template<class type>
void compute_first_matching_cost(Array3<type>& left, 
		                             Array3<type>& right,
																 Array3<double>& cost_left,
																 Array3<double>& cost_right,
																 const int& max_disparity) {
	Grid<float> left_gradient, right_gradient;
  compute_gradient(left_gradient, left);
	compute_gradient(right_gradient, right);
	compute_first_cost(cost_left, cost_right,
			               left, right,
										 left_gradient, right_gradient, max_disparity);
}

// Compute matching cost for other layers.
template<class type>
void compute_first_matching_cost(Array3<type>& left, 
		                             Array3<type>& right,
																 Array3<double>& cost_left,
																 Array3<double>& cost_right,
																 Grid<type>& pre_disparity_left,
																 const int& max_disparity) {
	Grid<float> left_gradient, right_gradient;
  compute_gradient(left_gradient, left);
	compute_gradient(right_gradient, right);
	compute_first_cost(cost_left, cost_right,
			               left, right,
										 left_gradient, right_gradient, 
										 pre_disparity_left,
										 max_disparity);
}

// Build trees only for the highest layer.
template<class type>
void build_tree(Array3<type>& left,
		            Array3<type>& right,
								Graph& left_graph,
								Graph& right_graph,
								Forest& forest_left,
								Forest& forest_right) {
	// Median filter.
	for (int i = 0; i < 3; ++i) {
	  median_filter(left[i]);
		median_filter(right[i]);
	}
  // Build graph.
	left_graph.collect_edges(left);
	right_graph.collect_edges(right);
	left_graph.build_MST();
	right_graph.build_MST();
	// left_graph.build_RandTree();
	// right_graph.build_RandTree();

	// Build forests.
  forest_left.init(left_graph);
	forest_right.init(right_graph);
	forest_left.order_of_visit();
	forest_right.order_of_visit();
}

// Build forests for other layers.
template<class type>
void build_tree(Array3<type>& left,
		            Array3<type>& right,
								Graph& left_graph,
								Graph& right_graph,
								Forest& forest_left,
								Forest& forest_right,
								Grid<type>& occ_left,
								Grid<type>& occ_right) {
	// Median filter.
	for (int i = 0; i < 3; ++i) {
	  median_filter(left[i]);
		median_filter(right[i]);
	}
  // Build graph.
	left_graph.collect_edges_edgeaware(left, occ_left);
	right_graph.collect_edges_edgeaware(right, occ_right);
	left_graph.build_MST();
	right_graph.build_MST();
	// left_graph.build_RandTree(true);
	// right_graph.build_RandTree(true);

	// Build forests.
  forest_left.init(left_graph);
	forest_right.init(right_graph);
	forest_left.order_of_visit();
	forest_right.order_of_visit();
}

template<class type>
void disparity_computation(Forest& forest_left,
		                       Forest& forest_right,
													 Array3<double>& cost_left,
													 Array3<double>& cost_right,
													 Grid<type>& disparity_left,
													 Grid<type>& disparity_right) {
  // Aggregate matching cost on tree.	
	forest_left.compute_cost_on_tree(cost_left);
	forest_right.compute_cost_on_tree(cost_right);
	
	// Compute disparity.
	compute_disparity(cost_left, disparity_left);
	compute_disparity(cost_right, disparity_right);
	
	// Median filter.
	median_filter(disparity_left);
	median_filter(disparity_right);
}

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
	// Grid<float> left_gradient, right_gradient;
	Array3<double> cost_left, cost_right;
 	compute_first_matching_cost(left, right, cost_left, cost_right,
	     max_disparity);
	
	build_tree(left, right, left_graph, right_graph, forest_left, forest_right);

	disparity_computation(forest_left, forest_right, cost_left, cost_right,
	 		disparity_left, disparity_right);
	save_image("123.pgm", disparity_left, 4);
}

