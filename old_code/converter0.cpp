#include "iomanager.h"

Array3<int> hehe;
Grid<unsigned char> gg;

int main(int args, char ** argv) {
    load_image(argv[1], hehe);
    int h = hehe.height, w = hehe.width;
    gg.reset(h, w);
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            if (!(hehe[0][i][j] == hehe[1][i][j] && hehe[1][i][j] == hehe[2][i][j]))
                printf("%d %d %d\n" , hehe[0][i][j], hehe[1][i][j], hehe[2][i][j]);
            gg[i][j] = hehe[0][i][j];
        }
    save_image(argv[2], gg);
    //printf("%d %d %d %d %d\n", hehe[0][0][0], hehe[1][0][0], hehe[2][0][0], hehe[0][0][1], hehe[1][0][1]);
    //printf("%d %d %d %d\n", hehe[2][h-1][w-2], hehe[0][h - 1][w - 1],  hehe[1][h - 1][w - 1],  hehe[2][h - 1][w - 1]);
}
