#include "settings.hpp" // the global variables, the constants, the array size 
#include "misc.hpp" // misc. 
#include "iomanager.hpp" // read and write pictures.

#include "prediction.hpp" // the prediction model

#include "image_layer.hpp"
#include "tree_dp_st.hpp" // the declaration of 'BigObject'
#include "extra.hpp"
#include "tree_dp_st.cpp"

#include "timekeeper.hpp"

TimeKeeper timer;

const int OBJ_NUM = 2;

ImageLayer left_pyramid[levels], right_pyramid[levels];
BigObject left[OBJ_NUM], right[OBJ_NUM];

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

int main(int args, char ** argv) {
    misc::process_args(args, argv);
    load_image(file_name[0], left_pyramid[0].rgb, left_pyramid[0].H, left_pyramid[0].W);
    load_image(file_name[1], right_pyramid[0].rgb, right_pyramid[0].H, right_pyramid[0].W);
    if (left_pyramid[0].H != right_pyramid[0].H || left_pyramid[0].W != right_pyramid[0].W) {
        printf("The size of two pictures differ!! quit....");
        return 0;
    }
    if (left_pyramid[0].H >= MAX_HEIGHT || right_pyramid[0].W >= MAX_WIDTH) {
        printf("Input : %d x %d, program : %d %d\n", left_pyramid[0].H, left[0].W, MAX_HEIGHT, MAX_WIDTH);
        puts("Image too big. change settings.hpp and re-compile.");
        return 0;
    }

    if (use_lab) {
      left_pyramid[0].computeLab();
      right_pyramid[0].computeLab();
    }
    for (int i = 0; i + 1 < levels; ++i) {
      left_pyramid[i+1].shrinkPicture(left_pyramid[i+1].rgb, left_pyramid[i].rgb, left_pyramid[i].H, left_pyramid[i].W);
      right_pyramid[i+1].shrinkPicture(right_pyramid[i+1].rgb, right_pyramid[i].rgb, right_pyramid[i].H, right_pyramid[i].W);
      if (use_lab) {
        left_pyramid[i+1].shrinkPicture(left_pyramid[i+1].lab, left_pyramid[i].lab, left_pyramid[i].H, left_pyramid[i].W);
        right_pyramid[i+1].shrinkPicture(right_pyramid[i+1].lab, right_pyramid[i].lab, right_pyramid[i].H, right_pyramid[i].W);
      }
      save_image_rgb(shrinkname[i+1][0], left_pyramid[i+1].rgb, 
          left_pyramid[i+1].H, left_pyramid[i+1].W);
    }

timer.reset();
    for (int lvl = levels - 1; lvl >= 0; -- lvl) {
        int idx = lvl % OBJ_NUM;
        left[idx].init(left_pyramid[lvl]);
        right[idx].init(right_pyramid[lvl]);
        if (lvl == levels - 1) {
            left[idx].noPrediction(max_disparity / (1 << lvl));
        } else {
            DP::getSupportProb(left[idx].rgb, right[idx].rgb, 
                                left[idx].H, left[idx].W, max_disparity / (1 << lvl));
            DP::getProbMatrix(lvl, max_disparity / (1 << (lvl + 1)), max_disparity / (1 << lvl), dataset);
            DP::getInterval(0.001 * (1 << lvl)/*, tot_threshold*/);
            left[idx].readPrediction(left[(lvl + 1)%OBJ_NUM].disparity);
            // left[idx].intersectInterval(left[(lvl + 1) % OBJ_NUM]);
        } 
        // Now use the INTERVAL to find the new disparities.
        left_pyramid[lvl].computeGradient();
        right_pyramid[lvl].computeGradient();
        left[idx].buildForest(0.95, use_lab);
        left[idx].initDisparity();
        updateTable(255 * 0.1);
        left[idx].steroMatch(right[idx], 1, use_lab);
        misc::median_filter(left[idx].disparity, left[idx].H, left[idx].W, 3);
        save_image(layername[lvl][0], left[idx].disparity, left[idx].H, left[idx].W, scale * (1 << lvl));
    } // end of layer iteration.

timer.check("all");

    save_image(file_name[2], left[0].disparity, left[0].H, left[0].W, scale);

    return 0;
}
