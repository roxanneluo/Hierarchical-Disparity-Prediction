// This code is for RT

#include "settings.h" // the global variables, the constants, the array size 
#include "misc.cpp" // misc. 
#include "iomanager.h" // read and write pictures.
#include "arbol0.h" // the declaration of 'BigObject'
#include "tree4.cpp"

#include "timekeeper.h"

BigObject left, right;

TimeKeeper timer;

// consider merging trunk and branch pal

int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left.rgb, left.H, left.W);
    load_image(file_name[1], right.rgb, right.H, right.W);
  timer.reset();
	// next part build the trees
	left.collect_edges();
	left.build_tree();
	left.prepare_visit();;
    left.compute_gradient();
	
	right.collect_edges();
	right.build_tree();
	right.prepare_visit();
    right.compute_gradient();
	
    // next part : compute disparity
    initDisparity(left, right);
    updateTable(255 * 0.1);
// timer.check("build forest");
    for (int d = 0; d <= max_disparity; ++d) {
        left.computeFirstCost(d, right);
        right.computeFirstCost(-d, left); // improvement can be done here
				// timer.check("first cost");
        left.compute_cost_on_tree();
        right.compute_cost_on_tree();
				// timer.check("cost on tree");
        updateDisparity(d, left, right);
		}
//    timer.check("steroMatch");
    // next part : refinement 
    misc::median_filter(left.disparity, left.H, left.W);
    misc::median_filter(right.disparity, right.H, right.W); 
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

timer.check("all");
	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    save_image(file_name[3], right.disparity, right.H, right.W, scale);


		return 0;
}