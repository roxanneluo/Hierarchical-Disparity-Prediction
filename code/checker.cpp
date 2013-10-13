#include "mylib.h"
#include "iomanager.h"

Grid<int> out;
Array3<int> ref;
int pad1 = 10, pad2 = 10;
// the area that is not compared 
// pad1 is on top and bottom,
// pad2 is on left and right

int main(int args, char ** argv) {
    // argv[1] is the output image
    // argv[2] is the standard image
    // argv[3] is the maximum error allowed
    if (args <= 3) { puts("Insufficient parameters"); return 0;}
    int tolerance = atoi(argv[3]);

    load_image_gray(argv[1], out);
    load_image(argv[2], ref);

    int h = out.height, w = out.width;
    if (h != ref.height || w != ref.width)
        { puts("Not even same size"); return 0;}

    int total = 0, correct = 0;
    for (int i = pad1; i < h - pad1; ++i) 
    for (int j = pad2; j < w - pad2; ++j) {
        if (ref[0][i][j] != ref[1][i][j] || ref[1][i][j] != ref[2][i][j])
            { puts("standard disparity read error"); return 0; }
        if (ref[0][i][j] == 0) continue;
        ++total;
        if (mylib::ABS(ref[0][i][j] - out[i][j]) <= tolerance)
            ++correct;
    }

    double result = correct * 100;
    printf("correct rate = %.3f%%\n", result / total);
    return 0;
}
