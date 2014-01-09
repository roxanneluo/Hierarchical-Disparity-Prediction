#include "mylib.h"
#include "iomanager.h"

/******************************************

This checker is used when running multiple tests.
The output is easier for other program to read.

******************************************/

Grid<int> out, ref, ref_left, ref_right, occ;
Array3<unsigned char> disparity_errormap;
int pad1 = 10, pad2 = 10;
char errormap[80], occmap[80];
// the area that is not compared
// pad1 is on top and bottom,
// pad2 is on left and right.

using namespace std;

int main(int args, char ** argv) {
    // argv[1] is the output image
    // argv[2] is the standard left image
    // argv[3] is the standard right image
    // argv[4] is the maximum error allowed at the same time.
    // argv[5] is the scale of the disparity map. This should
    // argv[6] indicates left or right image.
    // be read from spec.txt
    if (args < 7) {
        puts("Failed");
        return 0;
    }
    int tolerance = atoi(argv[4]);
    int scale = atoi(argv[5]);

    load_image_gray(argv[1], out);
    load_image_gray(argv[2], ref_left);
    load_image_gray(argv[3], ref_right);

    int left_or_right = atoi(argv[6]); // 0: left 1: right

    int h = out.height, w = out.width;
    if (h != ref_left.height || w != ref_left.width)
    {
        puts("Failed");
        return 0;
    }
    // Update the errormap.
    occ.reset(h, w);
    occ.zero();
    disparity_errormap.reset(3, h, w);
    if (left_or_right == 0) {
        ref.copy(ref_left);
    } else {
        ref.copy(ref_right);
    }

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            if (left_or_right == 0) {
                int d = ref_left[i][j] / scale;
                if (j - d >= 0 && ref_left[i][j] == ref_right[i][j -d])
                    occ[i][j] = 255;
            } else {
                int d = ref_right[i][j] / scale;
                if (j + d < w && ref_left[i][j + d] == ref_right[i][j])
                    occ[i][j] = 255;
            }
        }

    int total = 0, correct = 0;
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            for (int k = 0; k < 3; ++k) 
                disparity_errormap[k][i][j] = out[i][j];
            if (occ[i][j] == 0) 
                continue;
            ++total;
            // the trancated value should be tolerance * scale
            if (mylib::ABS(ref[i][j] - out[i][j]) < tolerance * scale) {
                ++correct;
            } else {
                for (int k = 1; k < 3; ++k)
                    disparity_errormap[k][i][j] = 0;
                disparity_errormap[0][i][j] = 127;
            }
        }
    printf("%d %d\n", correct , total);
    strncpy(errormap, argv[1], strlen(argv[1]) - 4);
    strncpy(occmap, argv[1], strlen(argv[1]) - 4);
    strcat(errormap, "error.ppm");
    strcat(occmap, "non-occ.pgm");
    save_image_rgb(errormap, disparity_errormap);
    save_image(occmap, occ);
    return 0;
}
