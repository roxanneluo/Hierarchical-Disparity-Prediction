#include "mylib.h"
#include "iomanager.h"

/******************************************

This checker is used when running multiple tests.
The output is easier for other program to read.

******************************************/

Grid<int> out, ref, disparity_errormap;
int pad1 = 10, pad2 = 10;
char errormap[80];
// the area that is not compared
// pad1 is on top and bottom,
// pad2 is on left and right.

using namespace std;

int main(int args, char ** argv) {
    // argv[1] is the output image
    // argv[2] is the standard image
    // argv[3] is the maximum error allowed at the same time.
    // argv[4] is the scale of the disparity map. This should
    // be read from spec.txt
    if (args <= 3) {
        puts("Failed");
        return 0;
    }
    int tolerance = atoi(argv[3]);
    int scale = atoi(argv[4]);

    load_image_gray(argv[1], out);
    load_image_gray(argv[2], ref);

    int h = out.height, w = out.width;
    if (h != ref.height || w != ref.width)
    {
        puts("Failed");
        return 0;
    }

    // Update the errormap.
    disparity_errormap.reset(h, w);
    disparity_errormap.zero();
    int total = 0, correct = 0;
    for (int i = pad1; i < h - pad1; ++i)
        for (int j = pad2; j < w - pad2; ++j) {
            if (ref[i][j] == 0) continue;
            ++total;
            // the trancated value should be tolerance * scale
            if (mylib::ABS(ref[i][j] - out[i][j]) <= tolerance * scale) {
                ++correct;
                disparity_errormap[i][j] = 255;
            } else if (ref[i][j] - out[i][j] > 0) {
                disparity_errormap[i][j] = 0;
            } else {
                disparity_errormap[i][j] = 127;
            }
        }
    printf("%d %d\n", correct , total);
    strncpy(errormap, argv[1], strlen(argv[1]) - 4);
    strcat(errormap, "error.pgm");
    save_image(errormap, disparity_errormap);
    return 0;
}
