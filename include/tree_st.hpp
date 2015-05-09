#ifndef FORESTS_CODE_H_OCT_2013
#define FORESTS_CODE_H_OCT_2013

// this is for simple Segment tree


/**************************************************

This code is the schemas.
Only declarations and single-line functions in this file.

functions are implemented in trunk.cpp and branch.cpp

If you want a new kind of tree, 
create a new branch.cpp and change the silly.cpp includings.

**************************************************/


#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>

#include "settings.hpp"
#include "misc.hpp" 
#include "tree_common.hpp"

class MergeSet {
public :
    int n;  int f[NODES];
    int siz[NODES], max_weight[NODES];
    int find(int x); 
    void init(int x);
    void merge(int a, int b, int w);
    bool good(int a, int b, int w);
};

#include "original_big_object.hpp"
#endif
