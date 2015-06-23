#ifndef STEREO_STATISTICS
#define STEREO_STATISTICS
#include <memory>
#include <string>
#include "settings.hpp"
#include "misc.hpp"
using std::string;
namespace stat {
  template <typename Dtype>
  void compute_concur(int *concur, Dtype large_disp[][MAX_WIDTH], Dtype small_disp[][MAX_WIDTH],
    int H, int W, int max_disparity, int sec_max_disparity) {
    //!! cannot use memset directly for a dynamic array
    memset(concur, 0, (sec_max_disparity + 1) * (max_disparity+1) * sizeof(int));

    int ii, jj;
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j) {
      ii = i/2; jj = j/2;
      ++concur[small_disp[ii][jj] * (max_disparity + 1) + large_disp[i][j]];
    }
  }

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
  void process_args(int args, char ** argv, 
      string & gnd_left_disp_name,
      string & dataset_name,
      string & algo, 
      string & concur_folder, string & support_folder) {
    misc::process_args(args, argv);
    int arg_idx = 5;
    gnd_left_disp_name = argv[arg_idx++];
    arg_idx +=4;
    dataset_name = argv[arg_idx++];
    algo = argv[arg_idx++];
    concur_folder = argv[arg_idx++];
    support_folder = argv[arg_idx++];
  }
}

#endif
