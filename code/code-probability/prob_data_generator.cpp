#include <vector>
#include <cstdlib>
#include <algorithm>
#include "../gridcode.h"
#include "../arithmetics.h"
#include "../scale.h"
#include "../iomanager.h"
#include "../timekeeper.h"
#include "../refinement.h"
//#include "imageresult.h"
#include "../MSF.h"
#include "../filter.h"
#include "prob.h"
#include "../filename.h"

using namespace std;


const int level = 3;
int contract = 2;
int max_disparity = 60;
int scale = 4;
int pi[level];
vector< Array3<unsigned char> > rgb_left(level), rgb_right(level); // int -> unsigned char
vector< Grid<unsigned char> > disparity_left(level), disparity_right(level); // int -> unsigned char

vector<Graph> graph_left(level), graph_right(level);
vector<Forest> forest_left(level), forest_right(level);


vector< Grid<unsigned char> > occlusion_left(level), occlusion_right(level);

// for image result
/*Forest left_support_forest, right_support_forest;
Grid<unsigned char> left_support_map[level], right_support_map[level];
Grid<unsigned char> left_tree_img, right_tree_img;
*/

char prefix[300] = "MST_";
char file_name[2][300] =
    {"left.ppm", "right.ppm"};
char save_file_name[50];

template <class type>
void refinement(Grid<type>& d_left, Grid<type>& d_right,
		Grid<type>& occ_left, Grid<type>& occ_right,
		Forest &forest_left, Forest &forest_right) {
    Array3<double> cost_left, cost_right;
    cost_left.reset(max_disparity, d_left.height, d_left.width);
    cost_right.reset(max_disparity, d_right.height, d_right.width);
    // find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
    update_matching_cost(cost_left, cost_right, d_left, d_right,
      occ_left, occ_right);

    forest_left.compute_cost_on_tree(cost_left, 255 * 0.05);
    forest_right.compute_cost_on_tree(cost_right, 255 * 0.05);
    compute_disparity(cost_left, d_left);
    compute_disparity(cost_right, d_right);
    //save_image("lefterror.pgm", occ_left);
    //save_image("righterror.pgm", occ_right);
}

void find_disparity() {
    for (int i = 0; i < level - 1; ++i) {
        scale_image(rgb_left[i], rgb_left[i + 1]);
        scale_image(rgb_right[i], rgb_right[i + 1]);
    }


	for (int i = level - 1; i >= 0; --i) {
		bool is_highest_layer = (i == level - 1) ? true : false;

        Array3<double> cost_left, cost_right;
        if (is_highest_layer) {
              compute_first_matching_cost(rgb_left[i], rgb_right[i],
                      cost_left, cost_right, max_disparity / pi[i]);
                build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
                      forest_left[i], forest_right[i]);
        } else {
            compute_first_matching_cost(rgb_left[i], rgb_right[i],
                    cost_left, cost_right, disparity_left[i + 1], max_disparity / pi[i]);
            build_tree(rgb_left[i], rgb_right[i], graph_left[i], graph_right[i],
                  forest_left[i], forest_right[i], occlusion_left[i + 1], occlusion_right[i + 1]);
        }
        disparity_computation(forest_left[i], forest_right[i],
                cost_left, cost_right, disparity_left[i], disparity_right[i]);

        find_stable_pixels(disparity_left[i], disparity_right[i],
             occlusion_left[i], occlusion_right[i]);
	}

	refinement(disparity_left[0], disparity_right[0], occlusion_left[0], occlusion_right[0],
                   forest_left[0], forest_right[0]);
    median_filter(disparity_left[0]);
    median_filter(disparity_right[0]);
}

float *pd;
int **cnt;
/**
 *  pr[di]
 */
void gen_pd(int index) {
    int disparity = max_disparity/pi[index];
    gen_pd(pd, disparity_left[index], disparity_right[index], disparity);
    char pd_prefix[100];
    strcpy(pd_prefix, prefix);
    strcat(pd_prefix, "_pd");
    save_pd(get_file_name(save_file_name, pd_prefix, index, ".txt"), pd, disparity);
}

void cnt_small_large(int small_i, int large_i) {
    cnt_small_large(cnt, disparity_left[small_i], disparity_left[small_i],
                        disparity_left[large_i], disparity_right[large_i],
                        max_disparity/pi[small_i], max_disparity/pi[large_i]);

    char cnt_prefix[100];
    strcpy(cnt_prefix, prefix);
    strcat(cnt_prefix, "_cnt");
    save_small_large(cnt, get_file_name(save_file_name, cnt_prefix, small_i, large_i,".txt"),
                        max_disparity/pi[small_i], max_disparity/pi[large_i]);
}

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
        strcpy(prefix, argv[5]);
        int len = strlen(prefix);
        prefix[len-8] = '\0';
        strcat(prefix, "_MST");
    }

    try {
        load_image(file_name[0], rgb_left[0]);
        load_image(file_name[1], rgb_right[0]);
    } catch (...) {
        puts("Error loading file");
        return 0;
    }

	pi[0] = 1;
	for (int i = 1; i < level; ++i) {
	  pi[i] = contract * pi[i - 1];
	}

    find_disparity();
    pd = new float[max_disparity+1];
    for (int i = 0; i < level; ++i) {
        gen_pd(i);
    }
    cnt = new int*[max_disparity/pi[1]+1];
    for (int i = 0; i < max_disparity/pi[1]+1; ++i)
        cnt[i] = new int[max_disparity+1];

    for (int i = 0; i < level; ++i)
    for (int j = i+1; j < level; ++j) {
        cnt_small_large(j,i);
    }

    delete [] pd;
    for (int i = 0; i < max_disparity/pi[1]+1; ++i)
        delete [] cnt[i];
    delete [] cnt;

    return 0;
}
