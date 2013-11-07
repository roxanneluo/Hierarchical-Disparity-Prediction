#include "gridcode.h"
#include "forests1.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
//#include "imageresult.h"

#include "filter.h"


char file_name[6][300] =
    {"left.ppm", "right.ppm", "displeft.pgm", "dispright.pgm", "silly_left.pgm", "silly_right.pgm"};
char save_file_name[50];

Array3<unsigned char> rgb_left, rgb_right; // int -> unsigned char
Array3<unsigned char> rgb_left_small, rgb_right_small;
Grid<unsigned char> disparity_left, disparity_right; // int -> unsigned char
Grid<unsigned char> disparity_left_small, disparity_right_small; // int -> unsigned char
Grid<unsigned char> gnd_left, gnd_right;
Grid<unsigned char> disparity_left_ctmf, disparity_right_ctmf;
Grid<float> left_gradient, right_gradient;
Array3<double> cost_left, cost_right;
Graph left_graph, right_graph;
Forest forest_left, forest_right;
int max_disparity = 60;
int scale = 4;
int **dif_gnd, **dif_MST;
int layer_scale = 2;

Grid<unsigned char> occlusion_left, occlusion_right;

// for image result
/*Forest left_support_forest, right_support_forest;
Grid<unsigned char> left_support_map, right_support_map;
Grid<unsigned char> left_tree_img, right_tree_img;*/

template <class type>
void refinement(Grid<type>& d_left, Grid<type>& d_right) {
  // find stable pixels by using left-right consisty check
  find_stable_pixels(d_left, d_right, occlusion_left, occlusion_right);
  update_matching_cost(cost_left, cost_right, d_left, d_right,
      occlusion_left, occlusion_right);

  forest_left.compute_cost_on_tree(cost_left, 255 * 0.05);
  forest_right.compute_cost_on_tree(cost_right, 255 * 0.05);
  compute_disparity(cost_left, d_left);
  compute_disparity(cost_right, d_right);
}

char * get_file_name(const char* head, int layer_scale, const char * tail) {
    char num[4];
    itoa(layer_scale, num, 10);
    strcpy(save_file_name, head);
    strcat(save_file_name, "_");
    strcat(save_file_name, num);
    strcat(save_file_name, tail);
    return save_file_name;
}

void find_disparity(Grid<unsigned char> &disparity_left, Grid<unsigned char> &disparity_right,
                    Array3<unsigned char> &rgb_left, Array3<unsigned char> &rgb_right, int max_disparity){

//int main(int args, char ** argv) {

//	const int h = rgb_left.height;
//	const int w = rgb_left.width;

  compute_gradient(left_gradient, rgb_left);
  compute_gradient(right_gradient, rgb_right);
  compute_first_cost(cost_left, cost_right, rgb_left, rgb_right,
      left_gradient, right_gradient, max_disparity);
  for (int i = 0; i < 3; ++i){
    median_filter(rgb_left[i]);
    median_filter(rgb_right[i]);
  }

  left_graph.collect_edges(rgb_left);
  right_graph.collect_edges(rgb_right);
  left_graph.build_MST();
  right_graph.build_MST();

  /*
  int times = 3;
  int x = 252, y = 167;
  draw_tree_and_RGBimage(left_tree_img, rgb_left,
      left_graph.trees, left_graph.n, times);
  draw_tree_and_RGBimage(right_tree_img, rgb_right,
      right_graph.trees, right_graph.n, times);
  left_support_forest.init(left_graph);
  right_support_forest.init(right_graph);
  draw_support_map(left_support_map, left_support_forest,
      left_graph, x, y, times)
  draw_support_map(right_support_map, right_support_forest,
      right_graph, x, y, times);
  */

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
  refinement(disparity_left, disparity_right);
  median_filter(disparity_left);
  median_filter(disparity_right);

  save_image(get_file_name("silly_left", layer_scale, ".pgm"), disparity_left, scale);
  save_image(get_file_name("silly_right", layer_scale, ".pgm"), disparity_right, scale);
  // save_image("lefttreeimage.pgm", left_tree_img);
  // save_image("righttreeimage.pgm", right_tree_img);
  // save_image("leftsupportmap.pgm", left_support_map);
  // save_image("rightsupportmap.pgm", right_support_map);
//  return 0;
}

void find_difference(int ** dif,
                      Grid<unsigned char> &disparity_left_small,Grid<unsigned char> &disparity_right_small,
                      Grid<unsigned char> &large_left, Grid<unsigned char> &large_right,
                      int layer_scale, int scale) {
    int W = disparity_left_small.width, H = disparity_left_small.height;
    //printf("w:%d, %d, h:%d, %d\n",W, large_left.width, H, large_left.height);
    for (int i = 0; i < max_disparity/layer_scale+1; ++i)
        for (int j = 0; j < max_disparity+1; ++j)
            dif[i][j] = 0;

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            for (int ii = 0; ii < layer_scale; ++ii)
            for (int jj = 0; jj < layer_scale; ++jj) {
                //if (disparity_left_small[i][j] <= max_disparity/layer_scale && large_leftlayer_scale*i+ii][layer_scale*j+jj]/scale <= max_disparity)
                    ++dif[disparity_left_small[i][j]][large_left[layer_scale*i+ii][layer_scale*j+jj]/scale];
                //else printf("error left <%d, %d, %d, %d>\n", i, j, disparity_left_small[i][j], large_left[2*i+ii][2*j+jj]/scale);
                //if (disparity_right_small[i][j] <= max_disparity/2 && large_right[2*i+ii][2*j+jj]/scale <= max_disparity)
                    ++dif[disparity_right_small[i][j]][large_right[layer_scale*i+ii][layer_scale*j+jj]/scale];
                //else printf("error right <%d, %d, %d, %d>\n", i, j, disparity_right_small[i][j], large_right[2*i+ii][2*j+jj]/scale);
                //++dif[disparity_right_small[i][j]][large_right[2*i+ii][2*j+jj]/scale];
            }
        }
    }

}
void build_small_rgb(Array3<unsigned char> &rgb_small, Array3<unsigned char> &rgb, int layer_scale) {
    int W = rgb.width, H = rgb.height;
    rgb_small.reset(3, H/layer_scale, W/layer_scale);

    int sum, size = layer_scale*layer_scale;
    for (int d = 0; d < 3; ++d)
        for (int i = 0; i < H/layer_scale; ++i) {
            for (int j = 0; j < W/layer_scale; ++j) {
                sum = 0;
                for (int ii = layer_scale*i; ii < layer_scale*i+layer_scale; ++ii)
                    for (int jj = layer_scale*j; jj < layer_scale*j+layer_scale; ++jj)
                        sum += rgb[d][ii][jj];
                rgb_small[d][i][j] = sum/size;
            }
        }
}
void save_dif(int **dif, char *dif_file_name, int layer_scale) {
    FILE *file = fopen(dif_file_name, "w");
    if (file != NULL) {
        for (int i = 0 ; i < max_disparity/layer_scale+1; ++i) {
            for (int j = 0; j < max_disparity+1; ++j) {
                fprintf(file, "%d ", dif[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
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
        strcpy(file_name[2], argv[5]);
        strcpy(file_name[3], argv[6]);
    }
    //load groud truth, rgb,
    //MST on rgb
    // scale rgb
    // ground truth on small rgb
    //count difference with ground truth
    // count difference with larger scale MST
    try {
        load_image(file_name[0], rgb_left);
        load_image(file_name[1], rgb_right);
        load_image_gray(file_name[2], gnd_left);
        load_image_gray(file_name[3], gnd_right);
    } catch (...) {
        puts("Error loading file");
        return 0;
    }

    find_disparity(disparity_left, disparity_right, rgb_left, rgb_right, max_disparity);

    build_small_rgb(rgb_left_small, rgb_left,layer_scale);
    build_small_rgb(rgb_right_small, rgb_right, layer_scale);

    find_disparity(disparity_left_small, disparity_right_small, rgb_left_small, rgb_right_small, max_disparity/layer_scale);
    /*for (int i = 0; i < rgb_left.height/2; ++i) {
        for (int j = 0; j < rgb_left.width/2; ++j) {
            if (disparity_left_small[i][j] >  max_disparity/2)
                printf("error left <%d, %d, %d>\n", i , j, disparity_left_small[i][j]);
            if (disparity_right_small[i][j] > max_disparity/2)
                printf("error right <%d, %d, %d>\n", i , j, disparity_right_small[i][j]);
        }
    }*/

    dif_gnd = new int* [max_disparity/layer_scale+1];
    dif_MST = new int* [max_disparity/layer_scale+1];
    for (int i = 0; i < max_disparity/layer_scale+1; ++i) {
        dif_gnd[i] = new int[max_disparity+1];
        dif_MST[i] = new int[max_disparity+1];
    }

    find_difference(dif_gnd, disparity_left_small, disparity_right_small, gnd_left, gnd_right, layer_scale, scale);
    find_difference(dif_MST, disparity_left_small, disparity_right_small, disparity_left, disparity_right,layer_scale, 1);

    save_dif(dif_gnd, get_file_name("dif_gnd", layer_scale, ".txt"), layer_scale);
    save_dif(dif_MST, get_file_name("dif_MST", layer_scale, ".txt"), layer_scale);

    for(int i = 0; i < max_disparity/layer_scale+1; ++i) {
        delete [] dif_gnd[i];
        delete [] dif_MST[i];
    }
    delete [] dif_gnd;
    delete [] dif_MST;

    return 0;
}
