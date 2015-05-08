#ifndef __BIG_OBJECT__
#define __BIG_OBJECT__

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>

#include "settings.hpp"
#include "misc.hpp" 
#include "image_layer.hpp"

#define MAX_TOLERANCE 7

class Edge {
public :
    int a, b; // Node ID
    int weight;
};

bool smaller_edge(const Edge & a, const Edge & b) 
    { return a.weight < b.weight; }


class TreeNode {
public :
	int x, y, id ; // id = its index in an array
	int ord, up_weight; // the bfs order and the edge weight to parent after direct tree constructed
	TreeNode() {}
	TreeNode(int a, int b) : x(a) , y(b) {}
    int degree, next_node[4], edge_weight[4];
    void add_edge(int node, int weight);
};

class Interval {
public :
    int l, r;
    Interval() : l(0), r(-1)  {}
    Interval(int a, int b) : l(a), r(b) {}
    Interval cap(Interval b) { return Interval(misc::max(l, b.l), misc::min(r, b.r)); }
    // problem in cup : the Intervals must NOT be DISJOINT !!
    Interval cup(Interval b)  { return Interval(misc::min(l, b.l), misc::max(r, b.r)); } 
    int length() { return r - l + 1;  }  //  r - l - 1;  or r - l ??
};

class MergeSet;

class BigObject {
  private :
    int m; // number of edges
    int ts; // number of tree edges
    inline void node_location(int p, int &x, int &y) {--p; x = p / W; y = p % W; }
	  MergeSet mset; // for buiding the tree
    Edge edges[NODES * 2]; // all candidate edges  1-based
    Edge trees[NODES]; // collected tree edges 1-based

    void collect_edges(); // collect all the edges.
    void collect_lab_edges(); // collect all the edges.
	  void prepare_visit();  // construct the bfs order for the forest
    void build_tree(double threshold); // build the tree given all the collected edges.

    void computeFirstCost (int d, BigObject & right, int low, int high) ;
    void computeFirstLabCost (int d, BigObject & right, int low, int high) ;
	  void compute_cost_on_tree(int low, int high);

    void updateDisparity(int d, int low, int high);
    void updateMatchingCost(int disp, int low, int high);


    TreeNode nodes[NODES];
    bool visited[NODES];
	  int order[NODES]; // the sequence of index, the visiting order of the tree
	  int orderX[NODES], orderY[NODES];
    int numOT, oneTree[NODES][2];


	  FloArrayPtr gradient;
    double best_cost[NODES], backup[NODES], cost[NODES];  
    int stable[NODES]; // can be 16-bit signed
   
public :
    int H, W; // graph size, height and width
    int n; // number of nodes
    PicturePtr rgb;     // can be 8-bit unsigned
    FloPicturePtr lab; 
    BytArray disparity; 
    Interval itv[NODES];

    BigObject() { rgb = NULL; lab = NULL; }
    BigObject(ImageLayer & image_layer) { init(image_layer); }
    void init(ImageLayer& image_layer);
    void initDisparity();
    void buildForest(double threshold, bool use_lab);
    void steroMatch(BigObject &ref, int sign, bool use_lab);
    void refinement();

    void readPrediction(BytArray disp);
    void noPrediction(int max_disp);

    inline int node_number(int x, int y) const { return x * W + y + 1; }
};



// implement the common BigObject function 

double table[333];
void updateTable(double sigma) {
    table[0] = 1;
    for(int i = 1; i <= 255; i++)
        table[i] = table[i-1] * exp(-1.0 / sigma);
}

void TreeNode::add_edge(int node, int weight) {
    next_node[degree] = node;
    edge_weight[degree] = weight;
    ++degree;
}

const double max_gradient_color_difference= 2.0;
const double max_color_difference = 10.0;
const double weight_on_color = 0.11;
void BigObject::computeFirstCost (int d, BigObject & right, int low, int high) { 
	// only the cost for disparity d.
	// printf("%d %d\n", low, high);
     for (int i = low; i <= high; ++i) {
        // int x = nodes[order[i]].x, y = nodes[order[i]].y;
        int x = orderX[i], y = orderY[i];
        int where = y - d; if (where < 0) where = 0; if (where >= right.W) where = right.W - 1;
        double cost1 = 0;
        for (int c = 0; c < 3; ++c)
            cost1 += misc::abs(rgb[x][y][c] - right.rgb[x][where][c]);
        cost1 = misc::min(cost1 / 3.0, max_color_difference); // here is weired
        double cost_gradient = misc::abs(gradient[x][y] - right.gradient[x][where]);
        cost_gradient = misc::min(cost_gradient, max_gradient_color_difference);
        cost[order[i]] = weight_on_color * cost1 + (1 - weight_on_color) * cost_gradient;
    }
}

void BigObject::computeFirstLabCost (int d, BigObject & right, int low, int high) { 
  //printf("Comp, trueuting first cost\n");
	// only the cost for disparity d.
    for (int i = low; i <= high; ++i) {
        // int x = nodes[order[i]].x, y = nodes[order[i]].y;
        int x = orderX[i], y = orderY[i];
        int where = y - d; if (where < 0) where = 0; if (where >= right.W) where = right.W - 1;
        float cost1 = 0, diff;
        for (int c = 0; c < 3; ++c) {
            diff = lab[x][y][c] - right.lab[x][where][c];
            cost1 += diff * diff;
        }
        cost1 = sqrt(cost1);
        cost1 = misc::min(cost1, max_color_difference); // here is weired
        double cost_gradient = misc::abs(gradient[x][y] - right.gradient[x][where]);
        cost_gradient = misc::min(cost_gradient, max_gradient_color_difference);
        cost[order[i]] = weight_on_color * cost1 + (1 - weight_on_color) * cost_gradient;
    }
}

void BigObject::collect_edges() {
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

void BigObject::prepare_visit() {
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


void BigObject::compute_cost_on_tree(int low, int high) {
    // order[low .. high] is a connected tree.

    for (int u = low; u <= high; ++u) 
        backup[order[u]] = cost[order[u]];

    for (int i = high; i >= low; --i) {
        int p = order[i];
        for (int j = 0; j < nodes[p].degree; ++j) {
            int q = nodes[p].next_node[j];
            if (nodes[q].ord > nodes[p].ord) 
                backup[p] += backup[q] * table[nodes[q].up_weight];
        }
    }

    cost[order[low]] = backup[order[low]];
    for (int i = low; i <= high; ++i) {
        int p = order[i];
        for (int j = 0; j < nodes[p].degree; ++j) {
            int q =  nodes[p].next_node[j];
            if (nodes[q].ord > nodes[p].ord) {
                double w = table[nodes[q].up_weight];
                double value_q_current = backup[q];
                double value_p = cost[p];
                cost[q] = w * (value_p - w* value_q_current) + value_q_current;
            }
        }
    }
} // compute cost on a little tree


void BigObject::updateDisparity(int d, int low, int high) {
    for (int u = low; u <= high; ++u) {
        // int i = nodes[order[u]].x;
        // int j = nodes[order[u]].y;
        int t = order[u];
        if (cost[t] < best_cost[t]) {
            best_cost[t] = cost[t];
            // disparity[/*nodes[t].x*/orderX[u]][/*nodes[t].y*/orderY[u]] = d;
            disparity[orderX[u]][orderY[u]] = d;
        }
    }
}

void BigObject::updateMatchingCost(int disp, int low, int high) {
    // If (i,j) is an unstable pixel, cost is 0 for all disparities.
    // Otherwise update it.
    for (int u = low; u <= high; ++u) {
        // int i = nodes[order[u]].x;
        // int j = nodes[order[u]].y;
        int t = order[u];
        if (stable[t] >= 0) 
           cost[t] = misc::abs(disp - stable[t]);
        else cost[t] = 0;
    }
}


void BigObject::initDisparity() {
    for (int i = 0; i <= n; ++i) 
        best_cost[i] = 1e30;
}

void BigObject::noPrediction(int max_disp) {
    for (int i = H * W; i >= 0; --i) {
        itv[i].l = 0;
        itv[i].r = max_disp - 1;
    }
}

void BigObject::readPrediction(BytArray disp) {
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            itv[node_number(i, j)].l = DP::interval[disp[i / 2][j / 2]][0];
            itv[node_number(i, j)].r = DP::interval[disp[i / 2][j / 2]][1];
        }
}

void BigObject::buildForest(double threshold, bool use_lab = true) {
    if (use_lab)
      collect_lab_edges();
    else
      collect_edges();
    build_tree(threshold);
    prepare_visit();
}

void BigObject::steroMatch(BigObject &ref, int sign, bool use_lab = true) {
    // sign is a thing for the first cost.
    for (int i = 1; i <= numOT; ++i) {
        Interval treeInterval = itv[mset.find(order[oneTree[i][0]])];
        int low = oneTree[i][0], high = oneTree[i][1];
        // printf("treeInterval %d %d\n", treeInterval.l, treeInterval.r);
        for (int d = treeInterval.l; d <= treeInterval.r; ++d) {
            // timer.check("first cost"); 
            if (use_lab)
              computeFirstLabCost(d * sign, ref, low, high); // sign is used here
            else 
              computeFirstCost(d * sign, ref, low, high); // sign is used here
            compute_cost_on_tree(low, high);
            // timer.check("cost on tree");
            updateDisparity(d, low, high);
        }
    }
}

void BigObject::collect_lab_edges() {
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

void BigObject::init(ImageLayer & image_layer) {
  H = image_layer.H;
  W = image_layer.W;
  rgb = image_layer.rgb;
  lab = image_layer.lab;
  gradient = image_layer.gradient;
}
#endif
