#ifndef STEREO_STATISTICS
#define STEREO_STATISTICS
#include <memory>
#include "../newcode/misc.cpp"
namespace stat {
  void compute_concur(int **concur, BytArray large_disp, BytArray small_disp,
    int H, int W, int max_disparity, int sec_max_disparity) {
    //!! cannot use memset directly for a dynamic array
    for (int i = 0; i < sec_max_disparity + 1; ++i)
      memset(concur[i], 0, (max_disparity+1) * sizeof(int));

    int ii, jj;
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j) {
      ii = i/2; jj = j/2;
      ++concur[small_disp[ii][jj]][large_disp[i][j]];
    }
  }
}
#endif
