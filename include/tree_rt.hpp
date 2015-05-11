#ifndef __TREE_RT__
#define __TREE_RT__

#include "big_object.hpp"


class RTBigObject: public BigObject {
 public:
  RTBigObject()
    :BigObject(new MergeSet) {}
  RTBigObject(ImageLayer &image_layer)
    :BigObject(new MergeSet, image_layer) {}
  void build_tree();
};

void RTBigObject::build_tree() {
	  srand(time(NULL));
    // std::sort(edges + 1, edges + m + 1, smaller_edge);
    std::random_shuffle(edges + 1, edges + m + 1);
    mset->init(n); ts = 0;
    for (int i = 1; i <= m; ++i)
        if (mset->merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
        }
}

#endif
