#ifndef __DP_ST__
#define __DP_ST__

#include "tree_st.hpp"
#include "dp_big_object.hpp"

class DPSTBigObject: public DPBigObject {
 public:
  DPSTBigObject():DPBigObject(new STMergeSet) {}
  DPSTBigObject(ImageLayer & image_layer)
    :DPBigObject(new STMergeSet, image_layer) {}

  void build_tree(double threshold);
};


void DPSTBigObject::build_tree(double threshold) {

    //std::sort(edges + 1, edges + m + 1, smaller_edge); 
    extra::sort(edges, 1, m);
    // std::random_shuffle(edges + 1, edges + m + 1);
    STMergeSet * stmset = (STMergeSet *) mset;
    stmset->init(n); ts = 0;
    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        Interval t1 = itv[stmset->find(a)];
        Interval t2 = itv[stmset->find(b)];
        Interval t3 = t1.cap(t2);
        Interval t4 = t1.cup(t2);
        double tmp = t3.length();
        tmp /= t4.length(); 
        if ( tmp < threshold ) continue;
        if (stmset->find(a) != stmset->find(b)) 
            if (stmset->good(a, b, edges[i].weight)) {
                trees[++ts] = edges[i]; 
                stmset->merge(a, b, edges[i].weight);
            }
    }
}

#endif
