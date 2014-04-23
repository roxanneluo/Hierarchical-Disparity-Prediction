#include "settings.h" // the global variables, the constants, the array size 
#include "misc.cpp" // misc. 
#include "iomanager.h" // read and write pictures.
#include "arbol.h" // the declaration of 'BigObject'
#include "trunk.cpp" // the part that doesn't usually change
#include "branch.cpp" // the part that differs in every method.


int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left.rgb, left.H, left.W);
    load_image(file_name[1], right.rgb, right.H, right.W);

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
    initDisparity();
    for (int d = 0; d < max_disparity; ++d) {
        left.computeFirstCost(d, right);
        right.computeFirstCost(-d, left);
        left.compute_cost_on_tree(255 * 0.1);
        right.compute_cost_on_tree(255 * 0.1);
        updateDisparity(d);
    }
    // next part : refinement 

    misc::median_filter(left.disparity, left.H, left.W);
    misc::median_filter(right.disparity, right.H, right.W); 
    initDisparity();
    findStablePixels();
    for (int d = 0; d < max_disparity; ++d) {
        updateMatchingCost(d);
        left.compute_cost_on_tree(255 * 0.05);
        right.compute_cost_on_tree(255 * 0.05);
        updateDisparity(d);
    }
    misc::median_filter(left.disparity, left.H, left.W);
    misc::median_filter(right.disparity, right.H, right.W); 

	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    save_image(file_name[3], right.disparity, right.H, right.W, scale);
    return 0;
}
