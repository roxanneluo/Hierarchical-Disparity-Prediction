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
    int siz[NODES], max_weight[NODES];
    int find(int x); 
    void init(int x);
    void merge(int a, int b, int w);
    bool good(int a, int b, int w);
};

#include "big_object.hpp"
#endif
