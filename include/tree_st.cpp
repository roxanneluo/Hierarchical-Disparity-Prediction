#ifndef BASIC_CODE_FOR_TREE_STsldkhfoqij23ionsd
#define BASIC_CODE_FOR_TREE_STsldkhfoqij23ionsd

// this is for simple segment tree

#include <algorithm>

int MergeSet::find(int x) {
    if (f[x] != x) 
        f[x] = find(f[x]);
    return f[x];
}

void MergeSet::init(int x) {
    n = x; 
    for (int i = 0; i <= n; ++i) {
        f[i] = i;
        siz[i] = 1;
        max_weight[i] = 0;
    }
}

void MergeSet::merge(int a, int b, int w) {
    siz[find(b)] += siz[find(a)];
    max_weight[find(b)] = misc::max(max_weight[find(a)], max_weight[find(b)]);
    max_weight[find(b)] = misc::max(w, max_weight[find(b)]);
    f[find(a)] = find(b); 
}

bool MergeSet::good(int a, int b, int w) {
    int x = find(a), y = find(b);
    return (w <= misc::min(max_weight[x] + 1200.0 / siz[x], max_weight[y] + 1200.0 / siz[y]));
}


void BigObject::build_tree() {

    //std::sort(edges + 1, edges + m + 1, smaller_edge);
    extra::sort(edges, 1, m);
    // std::random_shuffle(edges + 1, edges + m + 1);
    
    mset.init(n); ts = 0;
    
    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        if (mset.find(a) != mset.find(b)) 
            if (mset.good(a, b, edges[i].weight)) {
                trees[++ts] = edges[i]; 
                mset.merge(a, b, edges[i].weight);
            }
    }

    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        if (mset.find(a) != mset.find(b))  {
            trees[++ts] = edges[i]; 
            mset.merge(a, b, edges[i].weight);
        }
    }
}
void initDisparity(BigObject &who) {
    for (int i = 0; i < who.H; ++i)
        for (int j = 0; j < who.W; ++j) {
            who.best_cost[i][j] =  1e100;
        }
}
void updateDisparity(int d, BigObject & who) {
    for (int i = 0; i < who.H; ++i)
        for (int j = 0; j < who.W; ++j) {
            if (who.cost[i][j] < who.best_cost[i][j]) {
                who.best_cost[i][j] = who.cost[i][j];
                who.disparity[i][j] = d;
            }
        }
}

#endif
