#include "../gridcode.h"
#include "../forests1.h"
#include "../arithmetics.h"
#include "../iomanager.h"
#include "../timekeeper.h"
#include "../refinement.h"
//#include "imageresult.h"

#include "../filter.h"
#include "../filename.h"


Grid<unsigned char> support;
int *disp;
int max_disparity = 16;
int scale = 16;

char file_name[50];
char dataset[] = "tsukuba"; ///cones, teddy, tsukuba(16,16), venus(20,8)

void get_disp() {
    for (int i = 0; i < max_disparity+1; ++i)
        disp[i] = 0;
    int W = support.width, H = support.height;
    int d;
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            d = support[i][j]/scale;
            if (d != 0) {
                if (d > max_disparity) printf("error: %d\n", d);
                else ++disp[d];
            } //else printf("zero\n");
        }
}

void save_disp() {
    //"support_disp_cnt.txt"
    FILE *file = fopen(get_file_name(file_name, "Prob_Gen_", dataset, "__support_disp_cnt.txt"), "w");
    for (int i = 0; i < max_disparity+1; ++i)
        fprintf(file, "%d\n", disp[i]);
    fclose(file);
}

int main(int args, char ** argv) {
    //"left_supportmap_teddy.pgm"
    try {
        load_image_gray(get_file_name(file_name, "left_supportmap_", dataset, ".pgm"), support);
    } catch (...) {
        puts("Error loading file");
        return 0;
    }

    disp = new int [max_disparity+1];
    get_disp();
    save_disp();

    delete [] disp;
    return 0;
}
