#ifndef FORESTS_declarationssasfasdnlaadsni
#define FORESTS_declarationssasfasdnlaadsni

/**************************************************

This code is the schemas.
Only declarations and single-line functions in this file.

functions are implemented in tree.cpp

**************************************************/



class MergeSet {
public :
    int n;  int f[NODES];
    int find(int x); 
    void init(int x);
    bool merge(int a, int b);
};

#include "big_object.hpp"
#endif
