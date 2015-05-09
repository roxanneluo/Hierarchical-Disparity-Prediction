#ifndef BASIC_CODE_FOR_TREE_MSTsimple_hfjklh4ionv09
#define BASIC_CODE_FOR_TREE_MSTsimple_hfjklh4ionv09

#include <algorithm>

// This is for a simple random tree.
int MergeSet::find(int x) {
    if (f[x] != x) 
        f[x] = find(f[x]);
    return f[x];
}


void MergeSet::init(int x) {
    n = x;
    for (int i = 0; i <= n; ++i) f[i] = i;
}

bool MergeSet::merge(int a, int b) {
    if (find(a) != find(b)) { 
        f[find(a)] = find(b);
        return true;
    } else
    return false;
}


void BigObject::build_tree() {
	  srand(time(NULL));
    // std::sort(edges + 1, edges + m + 1, smaller_edge);
    std::random_shuffle(edges + 1, edges + m + 1);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i)
        if (mset.merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
        }
}

#endif
