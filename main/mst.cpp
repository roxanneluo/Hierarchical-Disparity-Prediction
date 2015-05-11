#include "settings.hpp" // the global variables, the constants, the array size 
#include "misc.hpp" // misc. 
#include "iomanager.hpp" // read and write pictures.
#include "tree_mst.hpp" // the declaration of 'BigObject'
#include "extra.hpp"
#include "ctmf.h"
#include "timekeeper.hpp"

ImageLayer left_layer, right_layer;
MSTBigObject left, right;

TimeKeeper timer;

// consider merging trunk and branch pal
int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left_layer.rgb, left_layer.H, left_layer.W);
    load_image(file_name[1], right_layer.rgb, right_layer.H, right_layer.W);
    left.init(left_layer); right.init(right_layer);
    timer.reset();

    //misc::median_filter_rgb(right.rgb, right.H, right.W, 1);
    if (use_lab) {
      left_layer.computeLab();
      //misc::median_filter_rgb(left.lab, left.H, left.W, 1);
    }
    left.buildTree(use_lab);	
    left_layer.computeGradient();
    right_layer.computeGradient();
    //right.buildTree(use_lab);
    // next part : compute disparity
    left.initDisparity();
    updateTable(255 * 0.1);
    //timer.check("build forest");
    left.stereoMatch(right, 1, max_disparity, use_lab);    
    //timer.check("stereoMatch");
//  next part : refinement 
    misc::median_filter(left.disparity, left.H, left.W);
    //misc::median_filter(right.disparity, right.H, right.W); 
    /*
    initDisparity(left, right);
    findStablePixels(left, right);
    updateTable(255 * 0.05);
    for (int d = 0; d <= max_disparity; ++d) {
        updateMatchingCost(d, left, right);
        left.compute_cost_on_tree();
        right.compute_cost_on_tree();
        updateDisparity(d, left, right);
    }
    misc::median_filter(left.disparity, left.H, left.W);
    misc::median_filter(right.disparity, right.H, right.W); 
    */

timer.check("all");
	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    //save_image(file_name[3], right.disparity, right.H, right.W, scale);
    return 0;
}
