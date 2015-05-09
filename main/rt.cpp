#include "settings.hpp" // the global variables, the constants, the array size 
#include "misc.hpp" // misc. 
#include "iomanager.hpp" // read and write pictures.
#include "tree_mst_rt.hpp" // the declaration of 'BigObject'
#include "extra.hpp"
#include "tree_rt.cpp"
#include "ctmf.h"
#include "timekeeper.hpp"

BigObject left, right;

TimeKeeper timer;

// consider merging trunk and branch pal
int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left.rgb, left.H, left.W);
    load_image(file_name[1], right.rgb, right.H, right.W);
    timer.reset();

    misc::median_filter_rgb(left.rgb, left.H, left.W, 1);
    //misc::median_filter_rgb(right.rgb, right.H, right.W, 1);
	// next part build the trees
	left.collect_edges();
	left.build_tree();
	left.prepare_visit();;
	
	//right.collect_edges();
	//right.build_tree();
	//right.prepare_visit();
    
    load_image(file_name[0], left.rgb, left.H, left.W);
    //load_image(file_name[1], right.rgb, right.H, right.W);

    left.compute_gradient();
	right.compute_gradient();
	
    // next part : compute disparity
    initDisparity(left, right);
    updateTable(255 * 0.1);
// timer.check("build forest");
    for (int d = 0; d <= max_disparity; ++d) {
        left.computeFirstCost(d, right);
        // right.copyLeftCostToRight(d, left);
        // right.computeFirstCost(-d, left); // improvement can be done here
// timer.check("first cost");
        left.compute_cost_on_tree();
        // right.compute_cost_on_tree();
// timer.check("cost on tree");
        updateDisparity(d, left, right);
    }
    misc::median_filter(left.disparity, left.H, left.W);

timer.check("all");
	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    //save_image(file_name[3], right.disparity, right.H, right.W, scale);
    return 0;
}
