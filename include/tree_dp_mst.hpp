#ifndef FORESTS_declarationssasfasdnlaadsni
#define FORESTS_declarationssasfasdnlaadsni

/**************************************************

This code is the schemas.
Only declarations and single-line functions in this file.

functions are implemented in tree.cpp

**************************************************/

#include "dp_big_object.hpp"

class DPMSTBigObject: public DPBigObject {
 public:
  DPMSTBigObject()
    :DPBigObject(new MergeSet) {}
  DPMSTBigObject(ImageLayer &image_layer)
    :DPBigObject(new MergeSet, image_layer) {}

  void build_tree(double threshold);
};

void DPMSTBigObject::build_tree(double threshold) {

    //std::sort(edges + 1, edges + m + 1, smaller_edge); 
    extra::sort(edges, 1, m);
    //std::random_shuffle(edges + 1, edges + m + 1);
    mset->init(n); ts = 0;
    for (int i = 1; i <= m; ++i) {
        Interval t1 = itv[mset->find(edges[i].a)];
        Interval t2 = itv[mset->find(edges[i].b)];
        Interval t3 = t1.cap(t2);
        Interval t4 = t1.cup(t2);
        double tmp = t3.length();
        tmp /= t4.length(); 
        if ( tmp < threshold ) continue;
        if (mset->merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
            itv[mset->find(edges[i].a)] = t4; 
        }
    }
}
#endif
