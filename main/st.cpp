// This is the simple ST

#include "settings.hpp" // the global variables, the constants, the array size 
#include "misc.hpp" // misc. 
#include "iomanager.hpp" // read and write pictures.
#include "tree_st.hpp" // the declaration of 'BigObject'
#include "extra.hpp"
#include "tree_st.cpp"

#include "timekeeper.hpp"

BigObject left, right;


// consider merging trunk and branch pal
TimeKeeper timer;
int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left.rgb, left.H, left.W);
    load_image(file_name[1], right.rgb, right.H, right.W);
  timer.reset();
	// next part build the trees
    misc::median_filter_rgb(left.rgb, left.H, left.W, 1);
    //misc::median_filter_rgb(right.rgb, right.H, right.W, 1);
    left.collect_edges();
    left.build_tree();
    left.prepare_visit();
    
    load_image(file_name[0], left.rgb, left.H, left.W);
    //load_image(file_name[1], right.rgb, right.H, right.W); 
    left.compute_gradient();
    right.compute_gradient();
	
    // next part : compute disparity
    initDisparity(left);
    updateTable(255 * 0.1);
    for (int d = 0; d <= max_disparity; ++d) {
        left.computeFirstCost(d, right);
        left.compute_cost_on_tree();
        updateDisparity(d, left);
    }

    misc::median_filter(left.disparity, left.H, left.W, 3);
    //misc::median_filter(right.disparity, right.H, right.W); 
    
    timer.check("all");
	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    //save_image(file_name[3], right.disparity, right.H, right.W, scale);
    return 0;
}
