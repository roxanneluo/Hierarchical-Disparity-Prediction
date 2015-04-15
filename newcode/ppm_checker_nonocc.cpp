#include <cassert>
#include "settings.h"
#include "iomanager.h"
#include "misc.cpp"

/******************************************

This checker is used when running multiple tests.
The output is easier for other program to read.

******************************************/

IntArray out;
Picture ref_left, ref_right;
Picture err;
int h, w;
// the area that is not compared
// pad1 is on top and bottom,
// pad2 is on left and right.
// black is occluded
// green is farther than ref
// red is closer than ref

int main(int args, char ** argv) {
    // argv[1] is the output image
    // argv[2] is the standard left image
    // argv[3] is the standard right image
    // argv[4] is the maximum error allowed.
    // argv[5] is the scale of the disparity map. This should be read from spec.txt
    // argv[6] is the filename of the errmap
    if (args < 6) {
        puts("Failed. Need more specifications.");
        return 0;
    }
    int tolerance = atoi(argv[4]);
    int scale = atoi(argv[5]);
    bool compute_err = args >= 7;

    load_image_gray(argv[1], out, h, w);
    int h0, w0, h1, w1;
    load_image(argv[2], ref_left, h0, w0);
    load_image(argv[3], ref_right, h1, w1);

    if (h != h0 || h != h1 || w != w0 || w != w1) {
        puts("Failed. Pictures have different size.");
        return 0;
    }

    int total = 0, correct = 0;
    for (int i = 0; i < h; ++i)
    for (int j = 0; j < w; ++j)  {
        assert(ref_left[i][j][1] == ref_left[i][j][0] && ref_left[i][j][2] == ref_left[i][j][0]);
        assert(ref_right[i][j][1] == ref_right[i][j][0] && ref_right[i][j][2] == ref_right[i][j][0]);
        int d = ref_left[i][j][0] / scale;
        if (j - d >= 0 && ref_left[i][j][0] == ref_right[i][j - d][0]) {
            ++total;
            for (int c = 0; c < 3; ++c) err[i][j][c] = out[i][j];
            if (misc::abs(out[i][j] - ref_left[i][j][0]) < tolerance * scale) {
                ++correct;
            } else {  
              int channel = out[i][j] < ref_left[i][j][0]? 1:0;
              for (int c = 0; c < 3; ++c)  
                if (c != channel) err[i][j][c] = 0;
            }
        } else {
            for (int c = 0; c < 3; ++c) err[i][j][c] = 0;
        }
    }
    printf("%d %d\n", correct , total);
    
    if (compute_err) 
      save_image_rgb(argv[6], err, h, w);
    return 0;
}
