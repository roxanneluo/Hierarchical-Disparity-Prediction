#ifndef __DP_BIG_OBJECT__
#define __DP_BIG_OBJECT__

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cassert>

#include "settings.hpp"
#include "misc.hpp" 
#include "image_layer.hpp"
#include "tree_common.hpp"
#include "merge_set.hpp"
#include "big_object.hpp"
#include "timekeeper.hpp"

#define MAX_TOLERANCE 7
typedef const int (*OneTree)[2];

class Interval {
public :
    int l, r;
    Interval() : l(0), r(-1)  {}
    Interval(int a, int b) : l(a), r(b) {}
    Interval cap(Interval b) { 
      return Interval(misc::max(l, b.l), misc::min(r, b.r)); 
    }
    // problem in cup : the Intervals must NOT be DISJOINT !!
    Interval cup(Interval b)  { 
      return Interval(misc::min(l, b.l), misc::max(r, b.r));
    } 
    inline int length() { return r - l + 1;  }  //  r - l - 1;  or r - l ??
};

class DPBigObject: public BigObject {
  protected :
    int numOT, oneTree[NODES][2];
    Interval itv[NODES];


	  virtual void prepare_visit();  // construct the bfs order for the forest
    virtual void collect_edges(); // collect all the edges.
    virtual void collect_lab_edges();
    virtual void build_tree(double threshold) = 0; // build the tree given all the collected edges.

  public :
    DPBigObject(MergeSet *merge_set) : BigObject(merge_set) {}
    DPBigObject(MergeSet *merge_set, ImageLayer &image_layer)
      : BigObject(merge_set, image_layer) {}

    void buildForest(double threshold, bool use_lab = false);
    void stereoMatch(DPBigObject &ref, int sign, bool use_lab = false);
    void refinement();

    void readPrediction(BytArray disp);
    void noPrediction(int max_disp);

    OneTree treeStartEnd() const  {return oneTree;}
    inline int numOfTree() const  {return numOT;}
    inline const Interval& getTreeInterval(int u) const  {
      return itv[mset->find(order[u])];
    }

};

// implement the common DPBigObject function 

void DPBigObject::collect_edges() {
    n = H * W;
    int k = 0;
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j)
        for (int p = 0; p < 2; ++p)
        for (int q = 0; q < 2; ++q) if (p + q == 1)
        if (i + p < H && j + q < W) {
            int xx = node_number(i, j), yy = node_number(i+p, j+q), ww = 0;
            for (int c = 0; c < 3; ++c) 
                ww = misc::max(ww, misc::abs(rgb[i][j][c] - rgb[i+p][j+q][c]));
            if (itv[xx].cap(itv[yy]).length() > 0 || ww <= MAX_TOLERANCE) {
                edges[++k].a = xx;
                edges[k].b = yy;
                edges[k].weight = ww;
            }
        }
    m = k;
}

void DPBigObject::prepare_visit() {
    //next part would reorganize the tree
    numOT = 0;
    for (int i = 1; i <= n; ++i) {
        nodes[i].id = i;
        nodes[i].degree = 0;
    }
    for (int i = 1; i <= ts; ++i) {
        int aa = trees[i].a, bb = trees[i].b, zz = trees[i].weight;
        node_location(aa, nodes[aa].x, nodes[aa].y);
        node_location(bb, nodes[bb].x, nodes[bb].y);
        nodes[aa].add_edge(bb, zz);
        nodes[bb].add_edge(aa, zz);
    }
    // next part would get the BFS order
    for (int i = 1; i <= n; ++i) 
        nodes[i].ord = -1;
    int num = 0, root = 0, numx = 0, numy = 0;
    while (1) {
        bool found = false;
        for (int i = root + 1; i <= n; ++i) 
            if (nodes[i].ord == -1) 
                { root = i; found = true; break; }
        if (!found) break;
        order[++num] = root;
				orderX[++numx] = nodes[root].x;
				orderY[++numy] = nodes[root].y;	
        nodes[root].ord = num;
        nodes[root].up_weight = 0;
        oneTree[++numOT][0] = num; // 
        for (int i = num; i <= num; ++i) { // this is a bfs
            int t = order[i];
            for (int j = 0; j < nodes[t].degree; ++j) {
                int p = nodes[t].next_node[j];
                if (nodes[p].ord == -1) {
                    order[++num] = p;
										orderX[++numx] = nodes[p].x;
										orderY[++numy] = nodes[p].y;
                    nodes[p].ord = num;
                    nodes[p].up_weight = nodes[t].edge_weight[j];
                    visited[p] = true;
                }
            }
        } // end for bfs
        oneTree[numOT][1] = num;
    }// end for finding a root
}


void DPBigObject::noPrediction(int max_disp) {
    for (int i = H * W; i >= 0; --i) {
        itv[i].l = 0;
        itv[i].r = max_disp;
    }
}

void DPBigObject::readPrediction(BytArray disp) {
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            itv[node_number(i, j)].l = DP::interval[disp[i / 2][j / 2]][0];
            itv[node_number(i, j)].r = DP::interval[disp[i / 2][j / 2]][1];
        }
}

void DPBigObject::buildForest(double threshold, bool use_lab) {
    if (use_lab)
      collect_lab_edges();
    else
      collect_edges();
    build_tree(threshold);
    prepare_visit();
}

void DPBigObject::stereoMatch(DPBigObject &ref, int sign, bool use_lab) {
    // sign is a thing for the first cost.
    for (int i = 1; i <= numOT; ++i) {
        Interval treeInterval = itv[mset->find(order[oneTree[i][0]])];
        int low = oneTree[i][0], high = oneTree[i][1];
        // printf("treeInterval %d %d\n", treeInterval.l, treeInterval.r);
        for (int d = treeInterval.l; d <= treeInterval.r; ++d) {
            //timer.check("first cost"); 
            if (use_lab)
              computeFirstLabCost(d * sign, ref, low, high); // sign is used here
            else 
              computeFirstCost(d * sign, ref, low, high); // sign is used here
            compute_cost_on_tree(low, high);
            //timer.check("cost on tree");
            update_disparity(d, low, high);
        }
    }
}


void DPBigObject::refinement() {
    for (int i = 1; i <= numOT; ++i) {
         Interval treeInterval = itv[mset->find(order[oneTree[i][0]])];
         int low = oneTree[i][0], high = oneTree[i][1];
         for (int d = treeInterval.l; d <= treeInterval.r; ++d) {
             updateMatchingCost(d, low, high);
             compute_cost_on_tree(low, high);
             update_disparity(d, low, high);
         }
    }
}

void DPBigObject::collect_lab_edges() {
    n = H * W;
    int k = 0;
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j)
        for (int p = 0; p < 2; ++p)
        for (int q = 0; q < 2; ++q) if (p + q == 1)
        if (i + p < H && j + q < W) {
            int xx = node_number(i, j), yy = node_number(i+p, j+q);
            float  ww = 0,  diff;
            for (int c = 0; c < 3; ++c) {
                diff = lab[i][j][c] - lab[i+p][j+q][c];
                ww += diff * diff;
            }
            ww = sqrt(ww)/80*255;
            if (itv[xx].cap(itv[yy]).length() > 0 || ww <= MAX_TOLERANCE) {
                edges[++k].a = xx;
                edges[k].b = yy;
                edges[k].weight = ww;
            }
        }
    m = k;
}



#endif
