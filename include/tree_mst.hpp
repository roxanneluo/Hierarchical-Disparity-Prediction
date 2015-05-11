#ifndef FORESTS_CODE_H_OCT_2013
#define FORESTS_CODE_H_OCT_2013


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

#include "big_object.hpp"

class MSTBigObject: public BigObject{
 public:
  MSTBigObject():
    BigObject(new MergeSet){
  }
  MSTBigObject(ImageLayer & image_layer)
    :BigObject(new MergeSet, image_layer) {}
};

#endif
