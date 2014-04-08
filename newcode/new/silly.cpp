#include "settings.h"
#include "iomanager.h"
#include "arbol.h"
#include "trunk.cpp"
#include "branch.cpp"

//#include "filter.h"

char file_name[4][300] =
{"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm"};


BigObject left, right;

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

    load_image(file_name[0], left.rgb, left.H, left.W);
    load_image(file_name[1], right.rgb, right.H, right.W);

    left.compute_gradient();
    right.compute_gradient();
	// next part build the trees
	left.collect_edges();
	left.build_tree();
	left.prepare_visit();;
    left.compute_gradient();
	
	right.collect_edges();
	right.build_tree();
	right.prepare_visit();
    right.compute_gradient();
	
	//working 
	left.getDisparity(right, true);
	right.getDisparity(left, false);
	
	//save
    save_image(file_name[2], left.disparity, left.H, left.W, scale);
    save_image(file_name[3], right.disparity, right.H, right.W, scale);
    return 0;
}
