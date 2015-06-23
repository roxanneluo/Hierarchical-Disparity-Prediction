// This is the simple ST

#include "settings.hpp" // the global variables, the constants, the array size 
#include "misc.hpp" // misc. 
#include "iomanager.hpp" // read and write pictures.
#include "tree_st.hpp" // the declaration of 'BigObject'
#include "extra.hpp"
#include "ctmf.h"
#include "timekeeper.hpp"

ImageLayer left_layer, right_layer;
ImageLayer left_median;
STBigObject left, right;

TimeKeeper timer;

int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left_layer.rgb, left_layer.H, left_layer.W);
    load_image(file_name[1], right_layer.rgb, right_layer.H, right_layer.W);
  timer.reset();

    left_median.H = left_layer.H, left_median.W = left_layer.W;
    misc::median_filter(left_layer.rgb, left_median.rgb, 
        left_median.H, left_median.W, 1);
    if (use_lab) {
      left_layer.computeLab();
      right_layer.computeLab();
      left_median.computeLab();
      // misc::median_filter_rgb(left.lab, left.H, left.W, 1);
    }

    left.init(left_median);
    left.buildTree(use_lab);	

    left.init(left_layer); right.init(right_layer);
    left_layer.computeGradient();
    right_layer.computeGradient();
    // next part : compute disparity
    left.initDisparity();
    updateTable(255 * 0.1);
// timer.check("build forest");
    left.stereoMatch(right, 1, max_disparity, use_lab);    
    misc::median_filter(left.disparity, left.H, left.W, 3); //TODO: why radius = 2, not 3 as dp

timer.check("all");
	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    //save_image(file_name[3], right.disparity, right.H, right.W, scale);
    return 0;
}
