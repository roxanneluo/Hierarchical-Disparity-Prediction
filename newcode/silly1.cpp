#include "gridcode.h"
#include "forests1.h"
#include "arithmetics.h"
#include "iomanager.h"

//#include "filter.h"

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
        load_image(file_name[0], rgb_left);
        load_image(file_name[1], rgb_right);
    } catch (...) {
        puts("Error loading file");
        return 0;
    }

    compute_gradient(left_gradient, rgb_left);
    compute_gradient(right_gradient, rgb_right);
    compute_first_cost(cost_left, cost_right, rgb_left, rgb_right,
                       left_gradient, right_gradient, max_disparity);

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
    //refinement(disparity_left, disparity_right);
    median_filter(disparity_left);
    median_filter(disparity_right);

    save_image(file_name[2], disparity_left, scale);
    save_image(file_name[3], disparity_right, scale);
    return 0;
}
