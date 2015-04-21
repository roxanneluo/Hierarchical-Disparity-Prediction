#ifndef STEREO_STATISTICS
#define STEREO_STATISTICS
#include <memory>
#include "../newcode/settings.h"
#include "../newcode/misc.cpp"
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
}
#endif
