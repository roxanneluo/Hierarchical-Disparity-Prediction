/* This code is for silly_MSF_ST.cpp
    it uses forests4.h
*/
#include "gridcode.h"
#include "forests4.h"
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
template<class type>
void compute_first_matching_cost(Array3<type>& left,
                                 Array3<type>& right,
                                 Array3<double>& cost_left,
                                 Array3<double>& cost_right,
                                 Grid<type>& pre_disparity_left,
                                 Grid<int>& interval,
                                 const int& max_disparity) {
    Grid<float> left_gradient, right_gradient;
    compute_gradient(left_gradient, left);
    compute_gradient(right_gradient, right);
    compute_first_cost(cost_left, cost_right,
                       left, right,
                       left_gradient, right_gradient,
                       pre_disparity_left,
                       interval,
                       max_disparity);
}

template<class type>
void compute_first_matching_cost(Array3<type>& left,
                                 Array3<type>& right,
                                 Array3<double>& cost_left,
                                 Array3<double>& cost_right,
                                 Forest& forest_left,
                                 Forest& forest_right,
                                 const int& max_disparity) {
    Grid<float> left_gradient, right_gradient;
    compute_gradient(left_gradient, left);
    compute_gradient(right_gradient, right);
    forest_left.compute_first_cost_on_tree_with_interval(cost_left,
            left, right, left_gradient, right_gradient, max_disparity, true);
    forest_right.compute_first_cost_on_tree_with_interval(cost_right,
            left, right, left_gradient, right_gradient, max_disparity, false);
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
        median_filter(left[i], 1);
        median_filter(right[i], 1);
    }
    // Build graph.
    left_graph.collect_edges(left);
    right_graph.collect_edges(right);
    left_graph.build_tree();
    right_graph.build_tree();
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
void build_tree_with_interval(Array3<type>& left,
                              Array3<type>& right,
                              Graph& left_graph,
                              Graph& right_graph,
                              Forest& forest_left,
                              Forest& forest_right,
                              Grid<type>& disp_left,
                              Grid<type>& disp_right,
                              Grid<int>& interval,
                              double threshold,
                              bool build_MST = false,
                              bool use_median_filter = false) {

    // Median filter.
    if (use_median_filter)
        for (int i = 0; i < 3; ++i) {
            median_filter(left[i], 1);
            median_filter(right[i], 1);
        }
    // Build graph.
    // printf("XXXXXXXXXXXXXX\n");
    left_graph.collect_edges_with_interval(left, disp_left, interval);
    right_graph.collect_edges_with_interval(right, disp_right, interval);
    left_graph.build_tree_with_interval(threshold);
    right_graph.build_tree_with_interval(threshold);

    // left_graph.collect_edges(left);
    // right_graph.collect_edges(right);
    // left_graph.build_MST();
    // right_graph.build_MST();

    // Build forests.
    forest_left.init(left_graph);
    forest_right.init(right_graph);

    forest_left.order_of_visit();
    forest_right.order_of_visit();
}

// in disparity_computation, the highest level should use a different method.
// But i didnt .  Still problems in this part.
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
void disparity_computation(Forest& forest_left,
                           Forest& forest_right,
                           Array3<double>& cost_left,
                           Array3<double>& cost_right,
                           Grid<type>& disparity_left,
                           Grid<type>& disparity_right,
                           Grid<type>& pre_disp_left,
                           Grid<type>& pre_disp_right,
                           Grid<int>& interval) {
    // Aggregate matching cost on tree.
    // forest_left.compute_cost_on_tree(cost_left);
    // forest_right.compute_cost_on_tree(cost_right);
    forest_left.compute_cost_on_tree_with_interval(cost_left);
    forest_right.compute_cost_on_tree_with_interval(cost_right);

    // Compute disparity.
    // compute_disparity(cost_left, disparity_left, pre_disp_left, interval);
    // compute_disparity(cost_right, disparity_right, pre_disp_right, interval);

    forest_left.compute_disparity_on_tree_with_interval(cost_left, disparity_left);
    forest_right.compute_disparity_on_tree_with_interval(cost_right, disparity_right);

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

