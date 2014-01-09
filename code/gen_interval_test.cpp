#include "gridcode.h"
#include "gen_interval.h"

#include <iostream>

using namespace std;


int small, large;
char file_name[2][300] = {"tsukuba","tsukuba_interval.txt"};

Array1<double> initial_cnt_matlab;
Array1<double> initial_cnt_c;

Grid<int> interval;

void read_initial_cnt_matlab (char* dataset, int H, int W) {
    char path[300] = "all_prob_gen/Prob_Gen_";
    strcat(path, dataset);
    strcat(path, "__cnt_1_0.txt");
    FILE* file = fopen(path, "r");
    double tmp;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            fscanf(file, "%lf", &tmp);
            initial_cnt_matlab[i] += tmp;
        }
        initial_cnt_matlab[i] /= 4;
    }
    // initial_cnt_matlab.normalize();
    fclose(file);
}

void read_initial_cnt_c (char* dataset, int H) {
    char path[300] = "all_prob_gen/Prob_Gen_";
    strcat(path, dataset);
    strcat(path, "__cnt_c++.txt");
    FILE* file = fopen(path, "r");
    double tmp;
    for (int i = 0; i < H; ++i) {
        fscanf(file, "%lf", &tmp);
        initial_cnt_c[i] = tmp;
    }
    fclose(file);
}
void read_interval (char* dataset, int H) {
    char path[300] = "all_gen_interval/";
    strcat(path, dataset);
    strcat(path, "_interval.txt");
    FILE* file = fopen(path, "r");
    int tmp1, tmp2;
    for (int i = 0; i < H; ++i) {
        fscanf(file, "%d%d", &tmp1, &tmp2);
        interval[i][0] = tmp1;
        interval[i][1] = tmp2;
    }
    fclose(file);
}
int main(int args, char* argv[]) {
    // gen_interval_test.out teddy 60
    strcpy(file_name[0], argv[1]);
    large = atoi(argv[2]);
    small = large / 2;

    large++;
    small++;

    initial_cnt_matlab.reset(small);
    initial_cnt_matlab.zero();
    initial_cnt_c.reset(small);
    initial_cnt_c.zero();

    interval.reset(small, 2);


    //	read_initial_cnt_matlab(file_name[0], small, large);
    read_initial_cnt_c(file_name[0], small);
    /*
    Array1<double> cmp = initial_cnt_matlab - initial_cnt_c;
    double err = 0;
    FILE* file = fopen(file_name[1], "w");
    for (int i = 0; i < cmp.length; ++i) {
      fprintf(file,
    			    "%6.0lf %6.0lf   %6.0lf\n",
    					initial_cnt_matlab[i],
    					initial_cnt_c[i],
    					cmp[i]);
      err += cmp[i] > 0 ? cmp[i] : -cmp[i];
    }
    fprintf(file, "ALL ERROR: %.4lf\n", err);
    fclose(file);
    */

    read_interval(file_name[0], small);

    //	FILE* file = fopen(file_name[1], "w");
    double search = 0;
    initial_cnt_c.normalize();
    for (int i = 0; i < interval.height; ++i) {
        search += (interval[i][1] - interval[i][0] + 1) * initial_cnt_c[i];
    }
    // fprintf(file,
    // 		    "Average Search Range ( %d ): %.4lf %.4lf\%\n",
    // 		    large - 1,
    //				search,
    //				search * 100 / (large - 1));
    //fclose(file);

    printf("%d %.6lf\n", large, search);

    return 0;
}
