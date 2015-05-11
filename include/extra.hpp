#ifndef EXTRA_FEATURES_LEEEEEROY_JENKINS_IH98343uhioj2b3ofu
#define EXTRA_FEATURES_LEEEEEROY_JENKINS_IH98343uhioj2b3ofu

#include "tree_common.hpp"

namespace extra {

Edge swapping[NODES * 2];
const int MAXV = 256;
int count[MAXV]; // assuming the values are in 0..255

void sort(Edge* a, int begin, int end) {
    // sort a[begin, end] similar to that sort in std
    for (int i = 0; i < MAXV; ++i) count[i] = 0;
    for (int i = begin; i <= end; ++i) count[a[i].weight] ++;
    for (int i = 1; i < MAXV; ++i) count[i] += count[i - 1];
    for (int i = end; i >= begin; --i)
        swapping[begin - 1 + count[a[i].weight]--] = a[i];
    for (int i = begin; i <= end; ++i)
        a[i] = swapping[i];
}

}

#endif
