#ifndef BASIC_CODE_FOR_DPF_ST_afih1p289rionnjk
#define BASIC_CODE_FOR_DPF_ST_afih1p289rionnjk

// this code is for dpf + ST

#include <algorithm>
// TimeKeeper	timer;

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
void BigObject::build_tree(double threshold) {

    //std::sort(edges + 1, edges + m + 1, smaller_edge); 
    extra::sort(edges, 1, m);
    // std::random_shuffle(edges + 1, edges + m + 1);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        Interval t1 = itv[mset.find(a)];
        Interval t2 = itv[mset.find(b)];
        Interval t3 = t1.cap(t2);
        Interval t4 = t1.cup(t2);
        double tmp = t3.length();
        tmp /= t4.length(); 
        if ( tmp < threshold ) continue;
        if (mset.find(a) != mset.find(b)) 
            if (mset.good(a, b, edges[i].weight)) {
                trees[++ts] = edges[i]; 
                mset.merge(a, b, edges[i].weight);
            }
    }
}



/*
void BigObject::refinement() {
    for (int i = 1; i <= numOT; ++i) {
         Interval treeInterval = itv[mset.find(order[oneTree[i][0]])];
         int low = oneTree[i][0], high = oneTree[i][1];
         for (int d = treeInterval.l; d <= treeInterval.r; ++d) {
             updateMatchingCost(d, low, high);
             compute_cost_on_tree(low, high);
             updateDisparity(d, low, high);
         }
    }
}

// iou
void BigObject::intersectInterval(const BigObject & small, double iou = 0.5) {
  Interval intersect, uni,  small_itv;
  int index;
  int cnt = 0;
  for (int i = 0; i < H; ++i)
  for (int j = 0; j < W; ++j) {
    index = node_number(i,j);
    small_itv = small.itv[small.node_number(i/2, j/2)];
    small_itv.l *= 2; small_itv.r *= 2;
    intersect = itv[index].cap(small_itv);
    uni = itv[index].cup(small_itv);
    if (intersect.length() / (double) misc::min(itv[index].length(), small_itv.length()) >= iou)
      itv[index] = intersect;
    else {
      itv[index] = uni; 
      ++cnt;
      // printf("large itv[%d][%d] = [%d, %d]; small itv[%d][%d] = [%d,%d]\n",
         // i, j, itv[index].l, itv[index].r, 
         //  i/2, j/2, small_itv.l, small_itv.r);
    }
  }
  printf("%d/%d = %f\n", cnt, H * W, cnt/double(H*W));
}
*/

#endif
