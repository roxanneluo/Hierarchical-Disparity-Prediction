#ifndef BASIC_CODE_FOR_DPFkjniluh3u98jf9p8
#define BASIC_CODE_FOR_DPFkjniluh3u98jf9p8

// this code is for dpf + MST

#include <algorithm>
// TimeKeeper	timer;

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

void BigObject::build_tree(double threshold) {

    //std::sort(edges + 1, edges + m + 1, smaller_edge); 
    extra::sort(edges, 1, m);
    //std::random_shuffle(edges + 1, edges + m + 1);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i) {
        Interval t1 = itv[mset.find(edges[i].a)];
        Interval t2 = itv[mset.find(edges[i].b)];
        Interval t3 = t1.cap(t2);
        Interval t4 = t1.cup(t2);
        double tmp = t3.length();
        tmp /= t4.length(); 
        if ( tmp < threshold ) continue;
        if (mset.merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
            itv[mset.find(edges[i].a)] = t4; 
        }
    }
}


/*
void findStablePixels(BigObject &left, BigObject & right) {
    for (int i = 0; i < left.H; ++i)
        for (int j = 0; j < right.W; ++j) {
            int d = left.disparity[i][j];
            left.stable[left.node_number(i, j)] = left.disparity[i][j]; // if stable[i][j] = -1, then it is not stable
            if (j - d < 0 || d == 0 || misc::abs(right.disparity[i][j - d] - d) >= 1) 
                left.stable[left.node_number(i, j)] = -1;
            d = right.disparity[i][j];
            right.stable[right.node_number(i, j)] = right.disparity[i][j];
            if (j + d >= left.W || d == 0 || misc::abs(left.disparity[i][j + d] - d) >= 1) 
                right.stable[right.node_number(i, j)] = -1;
        }
}
*/

#endif
