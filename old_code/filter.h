#ifndef FILTER_H_CODE_OCT_2013
#define FILTER_H_CODE_OCT_2013

#include "ctmf.h"
#include "gridcode.h"


Grid<unsigned char> tmp;

//template <class type>

void median_filter(Grid<unsigned char>& a, int radius = 2) {
    tmp.copy(a);
    int w = a.width, h = a.height;
    ctmf(tmp[0], a[0], w, h, w, w, radius, 1, w * h);
}

#endif
