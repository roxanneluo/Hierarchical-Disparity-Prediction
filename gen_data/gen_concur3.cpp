// This code is for dpf with ST

// !!! be care with the includes !!!!!!1
// TODO sample Pr(D_i)

#include <string>
#include "../newcode/settings.h" // the global variables, the constants, the array size 
#include "../newcode/misc.cpp" // misc. 
#include "../newcode/iomanager.h" // read and write pictures.

#include "../newcode/prediction.h" // the prediction model

#include "../newcode/arbol3.h" // the declaration of 'BigObject'
#include "../newcode/extra.cpp"
#include "../newcode/tree3.cpp"
#include "statistics.h"

#include "../newcode/timekeeper.h"
using std::string;
using std::to_string;

const int levels = 3;

TimeKeeper timer;

BigObject left[levels], right[levels];
Picture input_gnd;
BytArray gnds[levels];

const char layername[LEVELS][2][100] = { 
    { "nl0.pgm", "nr0.pgm"}, 
    { "nl1.pgm", "nr1.pgm"}, 
    { "nl2.pgm", "nr2.pgm"}
   // { "nl2.pgm", "nr2.pgm"},
   // { "nl3.pgm", "nr3.pgm"}
};

const char shrinkname[LEVELS][2][100] = {
    { "skL0.ppm", "skR0.ppm" },
    { "skL1.ppm", "skR1.ppm" },
    { "skL2.ppm", "skR2.ppm" }
    // { "skL2.ppm", "skR2.ppm" },
    // { "skL3.ppm", "skR3.ppm" }
};

string algo, dataset, concur_folder, support_folder;
/*
 * argv
 * 1: left_input_image, 2: right_input_image
 * 3: max disparity, 4: scale
 * 5: ground truth left disp
 * 6: concur_folder, 7: support_folder
 * 8: data set name
 * 9: algo 
 *
 * output: folder/concur_dataset_algo_l,l-1.txt
 * output: folder/support_dataset_algo_l.txt
 */
int main(int args, char ** argv) {
    misc::process_args(args, argv);
    concur_folder = argv[6];
    support_folder = argv[7];
    dataset = argv[8];
    algo = argv[9];
    string output_pre = concur_folder + "/concur_" + dataset + "_" + algo + "_";
    string support_pre = support_folder + "/support_" + dataset + "_" + algo + "_";

    load_image(file_name[0], left[0].rgb, left[0].H, left[0].W);
    load_image(file_name[1], right[0].rgb, right[0].H, right[0].W);
    int gnd_h, gnd_w;
    load_image(file_name[2], input_gnd, gnd_h, gnd_w, scale);
    if (!(left[0].H == right[0].H && left[0].W == right[0].W 
          && gnd_h == left[0].H && gnd_w == left[0].W)) {
        printf("The size of two pictures differ!! quit....");
        return 0;
    }
    if (left[0].H >= MAX_HEIGHT || right[0].W >= MAX_WIDTH) {
        printf("Input : %d x %d, program : %d %d\n", left[0].H, left[0].W, MAX_HEIGHT, MAX_WIDTH);
        puts("Image too big. change settings.h and re-compile.");
        return 0;
    }

    for (int i = 0; i + 1 < levels; ++i) {
        left[i].shrinkPicture(left[i + 1]);
        right[i].shrinkPicture(right[i + 1]);
        //save_image_rgb(("median_rgb_"+dataset+"_"+algo+"_"+to_string(i+1)+".ppm").c_str(),
        //    left[i+1].rgb, left[i+1].H, left[i+1].W);
        //TODO: scale
    }

    rgb2gray(gnds[0], input_gnd, gnd_h, gnd_w);
    for (int i = 1; i < levels-1; ++i)
        shrinkGray(gnds[i], gnds[i-1], gnd_h >> i, gnd_w >> i);
    /*
    for (int i = 0; i < levels-1; ++i) {
        string filename = "median_GND_"+dataset + "_" + algo + "_" + to_string(i) + ".pgm";
        save_image(filename.c_str(), gnds[i], gnd_h >> i, gnd_w >>i, scale << i);
    }
    */

printf("Printing support\n");
    for (int lvl = 0; lvl < levels-1; ++lvl) {
        double* support_prob = dpf::getSupportProb(left[lvl].rgb, right[lvl].rgb, 
                            left[lvl].H, left[lvl].W, max_disparity / (1 << lvl));
        save_array((support_pre + to_string(lvl) + ".txt").c_str(), 
            support_prob, (max_disparity >> lvl) + 1);
    }

printf("Computing disp maps\n");
    for (int lvl = levels - 1; lvl >= 1; -- lvl) {
        left[lvl].noPrediction(max_disparity / (1 << lvl));
        // Now use the INTERVAL to find the new disparities.
        left[lvl].buildForest(0.95);
        right[lvl].compute_gradient();
        left[lvl].initDisparity();
        updateTable(255 * 0.1);
        left[lvl].steroMatch(right[lvl], 1);
        misc::median_filter(left[lvl].disparity, left[lvl].H, left[lvl].W, 3);
//        save_image(("median_" + string(layername[lvl][0])).c_str(), 
//            left[lvl].disparity, left[lvl].H, left[lvl].W, scale * (1 << lvl));
    } // end of layer iteration.

printf("Saving concur\n");
    int sec_max_disparity = max_disparity / 2;
    int ** concur;
    concur = new int*[sec_max_disparity + 1];
    for (int i = 0; i < sec_max_disparity + 1; ++i) 
      concur[i] = new int[max_disparity + 1];

    for (int lvl = 0; lvl < levels-1; ++lvl) {
      string output_filename = output_pre
        + to_string(lvl+1) + "," + to_string(lvl) + ".txt";
      int layer_max_disp = max_disparity >> lvl;
      int layer_sec_max_disp = sec_max_disparity >> lvl;
      stat::compute_concur(concur, gnds[lvl], left[lvl+1].disparity,
        left[lvl].H, left[lvl].W, layer_max_disp, layer_sec_max_disp);
      save_matrix(output_filename.c_str(), concur, 
          layer_sec_max_disp+1, layer_max_disp+1);
      printf("saved %s\n", output_filename.c_str());
    }

    for (int i = 0; i < sec_max_disparity + 1; ++i)
      delete [] concur[i];
    delete [] concur;

    return 0;
}
