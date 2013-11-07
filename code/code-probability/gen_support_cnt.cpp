#include "gridcode.h"
#include "forests1.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
//#include "imageresult.h"

#include "filter.h"


Grid<unsigned char> support;
int *disp;
int max_disparity = 60;
int scale = 4;
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
    FILE *file = fopen("support_disp_cnt.txt", "w");
    for (int i = 0; i < max_disparity+1; ++i)
        fprintf(file, "%d\n", disp[i]);
    fclose(file);
}

int main(int args, char ** argv) {
    try {
        load_image_gray("left_supportmap_teddy.pgm", support);
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
