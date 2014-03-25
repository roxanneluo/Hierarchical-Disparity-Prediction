#ifndef __BLIND__
#define __BLIND__
#include "algorithm"
#include "gridcode.h"
class Point {
public:
    int x, y;

    bool operator < (const Point &p) const {
        if (x < p.x) return true;
        else if (x > p.x) return false;

        return y > p.y;
    }
};

template <class type>
void find_blind(Grid<type> &blind, Grid<type> &disp, bool left) {
    int H = blind.height, W = blind.width, n = H*W;

    Point *points = new Point[W];
    //points = (Point *) malloc((W+2)*sizeof(Point));
    /*Point **points = new Point *[W];
    for (int i = 0; i < W; ++i)
        points[i] = new Point;*/
    int r, d, x,y;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            if (left) {
                points[j].x = j-disp[i][j];// //
                points[j].y = j+disp[i][j];
            } else {
                points[j].x = j+disp[i][j]; //
                points[j].y = -j+disp[i][j];
                //printf("init x:%d,%d; y:%d,%d,%d\n", points[j].x, j+disp[i][j], points[j].y, -j + disp[i][j], sizeof(points[j]));
                //printf("init x:%d,%d; y:%d,%d,%d\n", points[j]->x, j+disp[i][j], points[j]->y, -j + disp[i][j], sizeof(points[j]));
            }
        }
        std::sort(points, points+W);
        int j = 0;
        while (j < W) {
            x = points[j].x;
            y = points[j++].y;
            //printf("after %d\n", y);
            if (left) {
                r = (x+y)/2;
                d = (y-x)/2;
            }
            else {
                r = (x-y)/2;
                d = (x+y)/2;
            }
            blind[i][r] = disp[i][r];
            while (j < W && points[j].x == x) ++j;
        }
        //for (int j = 0; j < W; ++j)
        //  if (!v[j]) printf("%d,", j);
        //printf("\n");
    }

    delete [] points;
}

template <class type>
void draw_blind_area(Grid<type> &blind, Grid<type> &gray, bool left) {
    blind.reset(gray.height, gray.width);
    blind.white();
    find_blind(blind, gray, left);
}



#endif

