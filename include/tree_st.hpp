#ifndef __ST__
#define __ST__ 

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
#include "merge_set.hpp"
#include "big_object.hpp"



/*******************************************
 * STMergeSet
 *******************************************/
class STMergeSet: public MergeSet {
public :
    int siz[NODES], max_weight[NODES];
    bool good(int a, int b, int w);
    virtual void init(int x);
    virtual void merge(int a, int b, int w);
};


void STMergeSet::init(int x) {
    n = x; 
    for (int i = 0; i <= n; ++i) {
        f[i] = i;
        siz[i] = 1;
        max_weight[i] = 0;
    }
}

void STMergeSet::merge(int a, int b, int w) {
    siz[find(b)] += siz[find(a)];
    max_weight[find(b)] = misc::max(max_weight[find(a)], max_weight[find(b)]);
    max_weight[find(b)] = misc::max(w, max_weight[find(b)]);
    f[find(a)] = find(b); 
}

bool STMergeSet::good(int a, int b, int w) {
    int x = find(a), y = find(b);
    return (w <= misc::min(max_weight[x] + 1200.0 / siz[x], max_weight[y] + 1200.0 / siz[y]));
}

/*******************************************
 * STBigObject
 *******************************************/
class STBigObject: public BigObject {
 public:
  STBigObject()
    :BigObject(new STMergeSet) {}
  STBigObject(ImageLayer &image_layer)
    :BigObject(new STMergeSet, image_layer) {}

  void build_tree();
};

void STBigObject::build_tree() {

    //std::sort(edges + 1, edges + m + 1, smaller_edge);
    extra::sort(edges, 1, m);
    // std::random_shuffle(edges + 1, edges + m + 1);
    
    STMergeSet * stmset = (STMergeSet *) mset;
    stmset->init(n); ts = 0;
    
    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        if (stmset->find(a) != stmset->find(b)) 
            if (stmset->good(a, b, edges[i].weight)) {
                trees[++ts] = edges[i]; 
                stmset->merge(a, b, edges[i].weight);
            }
    }

    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        if (stmset->find(a) != stmset->find(b))  {
            trees[++ts] = edges[i]; 
            stmset->merge(a, b, edges[i].weight);
        }
    }
}
#endif
