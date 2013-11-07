#include "gridcode.h"
#include "forests1.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
//#include "imageresult.h"

#include "filter.h"

int max_disparity = 60;
int scale = 1;
char file_name[6][300] =
    {"silly_left_4.pgm", "silly_left_2.pgm"};
char save_file_name[50];

char * get_file_name(const char* head, int small_scale, int large_scale, const char * tail) {
    char num[4];

    strcpy(save_file_name, head);
    strcat(save_file_name, "_");
    itoa(small_scale, num, 10);
    strcat(save_file_name, num);
    strcat(save_file_name, "_");
    itoa(large_scale, num, 10);
    strcat(save_file_name, num);
    strcat(save_file_name, tail);
    return save_file_name;
}

void find_difference(int ** dif,
                      Grid<unsigned char> &small,
                      Grid<unsigned char> &large,
                      int small_scale, int large_scale) {
    int W = small.width, H = small.height;
    int layer_scale = small_scale/large_scale;

    //printf("w:%d, %d, h:%d, %d\n",W, large_left.width, H, large_left.height);
    for (int i = 0; i < max_disparity/small_scale+1; ++i)
        for (int j = 0; j < max_disparity/large_scale+1; ++j)
            dif[i][j] = 0;

    int s, l;
   // printf("%d,%d\n", H, W);
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            for (int ii = 0; ii < layer_scale; ++ii)
            for (int jj = 0; jj < layer_scale; ++jj) {
               s = small[i][j];
               l = large[layer_scale*i+ii][layer_scale*j+jj];
               //printf("<i:%d, j:%d>\n",layer_scale*i+ii,layer_scale*j+jj );
               //if (s <= max_disparity/small_scale && l <= max_disparity/large_scale){
                ++ dif[s][l];
               //} else printf("error: <%d, %d>\n", s, l);
            }
        }
    }

}

void save_dif(int **dif, char *dif_file_name, int small_scale, int large_scale) {
    FILE *file = fopen(dif_file_name, "w");
    if (file != NULL) {
        for (int i = 0 ; i < max_disparity/small_scale+1; ++i) {
            for (int j = 0; j < max_disparity/large_scale+1; ++j) {
                fprintf(file, "%d ", dif[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}
int large_scale = 2, small_scale = 4;
int **dif_MST;
Grid<unsigned char> large, small;
int main(int args, char ** argv) {
    //load groud truth, rgb,
    //MST on rgb
    // scale rgb
    // ground truth on small rgb
    //count difference with ground truth
    // count difference with larger scale MST
    try {
        load_image_gray(file_name[0], small);
        load_image_gray(file_name[1], large);
    } catch (...) {
        puts("Error loading file");
        return 0;
    }


    dif_MST = new int* [max_disparity/small_scale+1];
    for (int i = 0; i < max_disparity/small_scale+1; ++i) {
        dif_MST[i] = new int[max_disparity/large_scale+1];
    }

    find_difference(dif_MST, small, large, small_scale, large_scale);

    save_dif(dif_MST, get_file_name("dif_MST", small_scale, large_scale, ".txt"), small_scale, large_scale);

    for(int i = 0; i < max_disparity/small_scale+1; ++i) {
        delete [] dif_MST[i];
    }
    delete [] dif_MST;

    return 0;
}
