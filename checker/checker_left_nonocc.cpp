#include <cstring>
#include <cassert>
#include "settings.hpp"
#include "iomanager.hpp"
#include "misc.hpp"

/******************************************

This checker is used when running multiple tests.
The output is easier for other program to read.

******************************************/

IntArray out;
IntArray gnd_left, gnd_right;
Picture err;
int h, w;
// the area that is not compared
// pad1 is on top and bottom,
// pad2 is on left and right.
// black is occluded
// green is farther than gnd
// red is closer than gnd

int main(int args, char ** argv) {
    // argv[1] is the output image
    // argv[2] is the standard left image
    // argv[3] is the Tolerence.
    // argv[4] is the scale of the disparity map. This should be read from spec.txt
    // argv[5] is the filename of the errmap
    // argv[6] is true if err pixels are all red and is 0 if err pixels are red
    // when closer than gnd, and green when farther than gnd
    if (args < 6) {
        puts("Failed. Need more specifications.");
        return 0;
    }
    int cnt = 2;
    int tolerance = atoi(argv[++cnt]);
    int scale = atoi(argv[++cnt]);
    bool compute_err = args >= 6;
    char * err_filename = compute_err? argv[++cnt]:NULL; 
    bool all_red = args > 6 && atoi(argv[++cnt]);

    load_image_gray(argv[1], out, h, w);
    int h0, w0;
    load_image_gray(argv[2], gnd_left, h0, w0);

    if (!(h == h0 && w == w0)) {
        puts("Failed. Pictures have different size.");
        return 0;
    }

    int total = 0, correct = 0;
    for (int i = 0; i < h; ++i)
    for (int j = 0; j < w; ++j)  {
        int d = gnd_left[i][j] / scale;
        if (d > 0) {
            ++total;
            for (int c = 0; c < 3; ++c) err[i][j][c] = out[i][j];
            if (misc::abs(out[i][j] - gnd_left[i][j]) < tolerance * scale) {
                ++correct;
            } else {  
              int channel = (out[i][j] > gnd_left[i][j] || all_red)? 0:1;
              for (int c = 0; c < 3; ++c)  
                if (c != channel) err[i][j][c] = 0;
            }
        } else {
            for (int c = 0; c < 3; ++c) err[i][j][c] = 0;
        }
    }
    printf("%d %d\n", correct , total);
    
    if (compute_err) 
      save_image_rgb(err_filename, err, h, w);
    return 0;
}
