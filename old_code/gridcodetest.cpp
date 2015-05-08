#include <cstdio>
#include "gridcode.h"
int main() {
    Grid<unsigned char> grid[3];
    for (int i = 0; i < 3; ++i)
        grid[i].reset(i*100,i*200);
    Grid<unsigned char> notreset;
    return 0;
}
