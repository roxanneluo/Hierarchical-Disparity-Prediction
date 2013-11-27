#ifndef _PROBABILITY_
#define _PROBABILITY_
#include <cstring>
#include "../gridcode.h"

template<class type>
void cnt_small_large(int ** dif,
                      Grid<type> &disparity_left_small,Grid<type> &disparity_right_small,
                      Grid<type> &disparity_left_large, Grid<type> &disparity_right_large,
                      int max_disparity_small, int max_disparity_large, int scale = 1) {
    int W = disparity_left_small.width, H = disparity_left_small.height;
    int largeW = disparity_left_large.width, largeH = disparity_right_large.height;
    for (int i = 0; i < max_disparity_small+1; ++i)
        memset(dif[i], 0, (max_disparity_large+1)*sizeof(int));

    int layer_scale = largeW/W;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            for (int ii = 0; ii < layer_scale; ++ii)
            for (int jj = 0; jj < layer_scale; ++jj) {
                //if (disparity_left_small[i][j] <= max_disparity/layer_scale && disparity_left_largelayer_scale*i+ii][layer_scale*j+jj]/scale <= max_disparity)
                    ++dif[disparity_left_small[i][j]][disparity_left_large[layer_scale*i+ii][layer_scale*j+jj]/scale];
                //else printf("error left <%d, %d, %d, %d>\n", i, j, disparity_left_small[i][j], disparity_left_large[2*i+ii][2*j+jj]/scale);
                //if (disparity_right_small[i][j] <= max_disparity/2 && disparity_right_large[2*i+ii][2*j+jj]/scale <= max_disparity)
                    ++dif[disparity_right_small[i][j]][disparity_right_large[layer_scale*i+ii][layer_scale*j+jj]/scale];
                //else printf("error right <%d, %d, %d, %d>\n", i, j, disparity_right_small[i][j], disparity_right_large[2*i+ii][2*j+jj]/scale);
                //++dif[disparity_right_small[i][j]][disparity_right_large[2*i+ii][2*j+jj]/scale];
            }
        }
    }

}

void save_small_large(int **dif, char *dif_file_name, int disparity_small, int disparity_large) {
    FILE *file = fopen(dif_file_name, "w");
    if (file != NULL) {
        for (int i = 0 ; i < disparity_small+1; ++i) {
            for (int j = 0; j < disparity_large+1; ++j) {
                fprintf(file, "%d ", dif[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}

template <class type>
void cnt_disp(float *pd, Grid<type> &support, int scale = 1) {
    int W = support.width, H = support.height;
    int d;
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            d = support[i][j]/scale;
            /*if (d != 0) {
                if (d > max_disparity) printf("error: %d\n", d);
                else ++disp[d];
            } *///else printf("zero\n");
            ++pd[d];
        }
}

template <class type>
void gen_pd(float *pd, Grid<type> &support_left, Grid<type> &support_right, int max_disparity, int scale = 1) {
    //for (int i = 0; i < max_disparity+1; ++i)
      //  pd[i] = 0;
    memset(pd, 0, (max_disparity+1)*sizeof(float));

    cnt_disp(pd, support_left, scale);
    cnt_disp(pd, support_right, scale);
	 	int size = support_left.width*support_left.height;
    for (int i = 0; i < max_disparity+1; ++i) {
        pd[i] /= 2*size;
    }
}

void save_pd(const char * file_name, float * pd, int disparity) {
    FILE *file = fopen(file_name, "w");
    for (int i = 0; i < disparity+1; ++i)
        fprintf(file, "%f\n", pd[i]);
    fclose(file);
}

#endif // _PROBABILITY_
