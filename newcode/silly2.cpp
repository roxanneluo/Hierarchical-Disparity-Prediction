// This is the simple ST

#include "settings.h" // the global variables, the constants, the array size 
#include "misc.cpp" // misc. 
#include "iomanager.h" // read and write pictures.
#include "arbol2.h" // the declaration of 'BigObject'
#include "tree2.cpp"

#include "timekeeper.h"

BigObject left, right;


// consider merging trunk and branch pal
TimeKeeper timer;
int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left.rgb, left.H, left.W);
    load_image(file_name[1], right.rgb, right.H, right.W);
  timer.reset();
	// next part build the trees
	for (int i = 0; i < left.H; ++i) {
	  for (int j = 0; j < left.W; ++j) {
		  for (int c = 0; c < 3; ++c) {
			  left.rgb_[c][i][j] = left.rgb[i][j][c];
				right.rgb_[c][i][j] = right.rgb[i][j][c];
			}
		}
	}
	for (int c = 0; c < 3; ++c)
	  misc::median_filter(left.rgb_[c], left.H, left.W, 1);
  for (int i = 0; i < left.H; ++i) {
	  for (int j = 0; j < left.W; ++j) {
		  for (int c = 0; c < 3; ++c) {
			  left.rgb[i][j][c] = left.rgb_[c][i][j];
				right.rgb[i][j][c] = right.rgb_[c][i][j];
			}
		}
	}
	left.collect_edges();
	left.build_tree();
	left.prepare_visit();;
	
	right.collect_edges();
	right.build_tree();
	right.prepare_visit();
  
	load_image(file_name[0], left.rgb, left.H, left.W);
  load_image(file_name[1], right.rgb, right.H, right.W); 
  
  left.compute_gradient();
  right.compute_gradient();
	
    // next part : compute disparity
    initDisparity(left, right);
    updateTable(255 * 0.1);
    for (int d = 0; d <= max_disparity; ++d) {
        left.computeFirstCost(d, right);
        right.computeFirstCost(-d, left); // improvement can be done here
        left.compute_cost_on_tree();
        right.compute_cost_on_tree();
        updateDisparity(d, left, right);
    }

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
