// !!! be care with the includes !!!!!!

#include <string>
#include "settings.hpp" // the global variables, the constants, the array size 
#include "misc.hpp" // misc. 
#include "iomanager.hpp" // read and write pictures.

#include "prediction.hpp" // the prediction model

#include "image_layer.hpp"
#include "tree_dp_mst.hpp" // the declaration of 'BigObject'
#include "extra.hpp"

#include "timekeeper.hpp"
#include "statistics.hpp"


using std::string;
using std::to_string;

TimeKeeper timer;

const int OBJ_NUM = 2;

BytArray gnds[levels];
int gnd_h[levels], gnd_w[levels];
ImageLayer left_pyramid[levels], right_pyramid[levels];
DPMSTBigObject left[OBJ_NUM], right[OBJ_NUM];

const char layername[LEVELS][2][100] = { 
    { "nl0.pgm", "nr0.pgm"}, 
    { "nl1.pgm", "nr1.pgm"}, 
    { "nl2.pgm", "nr2.pgm"},
    { "nl3.pgm", "nr3.pgm"}
};

const char shrinkname[LEVELS][2][100] = {
    { "skL0.ppm", "skR0.ppm" },
    { "skL1.ppm", "skR1.ppm" },
    { "skL2.ppm", "skR2.ppm" },
    { "skL3.ppm", "skR3.ppm" }
};

string algo, dataset_name, concur_folder, support_folder, gnd_left_disp_name;

int concur[MAX_WIDTH * MAX_WIDTH];


/*
 * argv
 * 1: left_input_image, 2: right_input_image
 * 3: max disparity, 4: scale
 * 5: gnd left disp
 * 6: fullsize/halfsize
 * 7: use_lab
 * 8: data set name
 * 9: algo 
 * 10: concur_folder, 11: support_folder
 *
 * output: concur_folder/concur_dataset_algo_l,l-1.txt
 * output: support_folder/support_dataset_algo_l.txt
 */
int main(int args, char ** argv) {
    stat::process_args(args, argv,
        gnd_left_disp_name, dataset_name, algo,
        concur_folder, support_folder);
    string output_pre = concur_folder + "/concur_" + dataset_name + "_" + algo + "_";
    string support_pre = support_folder + "/support_" + dataset_name + "_" + algo + "_";

  printf("Loading images\n");
    load_image(file_name[0], left_pyramid[0].rgb, 
        left_pyramid[0].H, left_pyramid[0].W);
    load_image(file_name[1], right_pyramid[0].rgb, 
        right_pyramid[0].H, right_pyramid[0].W);
  printf("Loading Gray %s\n", file_name[2]);
    load_image_gray(gnd_left_disp_name.c_str()
        , gnds[0], gnd_h[0], gnd_w[0], scale); // !!!! NOTE: load with scale !!!!
    if (left_pyramid[0].H != right_pyramid[0].H 
        || left_pyramid[0].W != right_pyramid[0].W) {
        printf("The size of two pictures differ!! quit....");
        return 0;
    }
    if (left_pyramid[0].H >= MAX_HEIGHT 
        || right_pyramid[0].W >= MAX_WIDTH) {
        printf("Input : %d x %d, program : %d %d\n", 
            left_pyramid[0].H, left[0].W, MAX_HEIGHT, MAX_WIDTH);
        puts("Image too big. change settings.hpp and re-compile.");
        return 0;
    }

    if (use_lab) {
      printf("Compute Lab\n");
      left_pyramid[0].computeLab();
      right_pyramid[0].computeLab();
    }

  printf("Shrinking pictures\n");
    for (int i = 0; i + 1 < levels; ++i) {
      left_pyramid[i+1].shrinkPicture(left_pyramid[i+1].rgb, 
          left_pyramid[i].rgb, left_pyramid[i].H, left_pyramid[i].W);
      right_pyramid[i+1].shrinkPicture(right_pyramid[i+1].rgb, 
          right_pyramid[i].rgb, right_pyramid[i].H, right_pyramid[i].W);
      if (use_lab) {
        left_pyramid[i+1].shrinkPicture(left_pyramid[i+1].lab, 
            left_pyramid[i].lab, left_pyramid[i].H, left_pyramid[i].W);
        right_pyramid[i+1].shrinkPicture(right_pyramid[i+1].lab, 
            right_pyramid[i].lab, right_pyramid[i].H, right_pyramid[i].W);
      }
    }

  printf("Shrink grays\n");
    //save_image(shrinkname[0][0], gnds[0], gnd_h[0], gnd_w[0], scale);
    for (int i = 1; i < levels-1; ++i) {
      ImageLayer::shrinkDisp(gnds[i], gnd_h[i], gnd_w[i],
          gnds[i-1], gnd_h[i-1], gnd_w[i-1]);
      //save_image(shrinkname[i][0], gnds[i], gnd_h[i], gnd_w[i], scale << i);
    }

  printf("Compute Support\n");
    double * support;
    for (int lvl = 0; lvl < levels-1; ++lvl) {
        support = DP::getSupportProb(left_pyramid[lvl].rgb, right_pyramid[lvl].rgb, 
            left_pyramid[lvl].H, left_pyramid[lvl].W, max_disparity / (1 << lvl));
        save_array((support_pre + to_string(lvl) + ".txt").c_str(), 
            support, (max_disparity >> lvl) + 1);
    }

  printf("Compute Disparity\n");
    for (int lvl = levels - 1; lvl >= 1; -- lvl) {
        int idx = lvl % OBJ_NUM;
        left[idx].init(left_pyramid[lvl]);
        right[idx].init(right_pyramid[lvl]);
        left[idx].noPrediction(max_disparity / (1 << lvl));
        // Now use the INTERVAL to find the new disparities.
        left_pyramid[lvl].computeGradient();
        right_pyramid[lvl].computeGradient();
        left[idx].buildForest(0.95, use_lab);
        left[idx].initDisparity();
        updateTable(255 * 0.1);
        left[idx].stereoMatch(right[idx], 1, use_lab);
        misc::median_filter(left[idx].disparity, left[idx].H, left[idx].W, 3);
        //save_image(layername[lvl][0], left[idx].disparity, left[idx].H, left[idx].W, scale * (1 << lvl));
        
        // print concur
        string output_filename = output_pre
          + to_string(lvl) + "," + to_string(lvl - 1) + ".txt";
        int layer_max_disp = max_disparity >> (lvl - 1);
        int layer_sec_max_disp = max_disparity >> lvl;
        stat::compute_concur(concur, gnds[lvl - 1], left[idx].disparity,
          gnd_h[lvl-1], gnd_w[lvl-1], layer_max_disp, layer_sec_max_disp); // NOTE: large H and large W!!
        save_matrix(output_filename.c_str(), concur, 
            layer_sec_max_disp+1, layer_max_disp+1);
        printf("saved %s\n", output_filename.c_str());
    } // end of layer iteration.

timer.check("all");

   // save_image(file_name[2], left[0].disparity, left[0].H, left[0].W, scale);

    return 0;
}
