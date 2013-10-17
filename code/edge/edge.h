#ifndef __EDGE__
#define __EDGE__

#include <cmath>
#include "../gridcode.h"
#include "../mylib.h"

double smooth(double x, double beta) {
    if (x<=1) return x;
    return 1+log2(mylib::max(x-1, 0.001))/beta;
}

template <class type>
double sigma(Array3<type> &rgb, int i, int j, int w, int beta) {
    int W = rgb.width, H = rgb.height;
    int t = mylib::min(i,w), b = mylib::min(H-1-i, w);
    int l = mylib::min(j, w), r = mylib::min(W-1-j,w);
    double square = 0, sum = 0, ans = 0;
    int size = (l+r+1)*(t+b+1);
    int color;
    for (int d = 0; d < 3; ++d) {
        square = sum = 0;
        for (int ii = i-t; ii <= i+b; ++ii)
        for (int jj = j-l; jj <= j+r; ++jj) {
            color = rgb[d][ii][jj];
            sum+=color;
            square += color*color;
        }
        ans += (square+sum*sum/size)/size;
    }
    return smooth(sqrt(mylib::max(ans,0.0)/3), beta);
}
#endif // __EDGE__
