#ifndef FORESTS_declarationssasfasdnlaadsni
#define FORESTS_declarationssasfasdnlaadsni

/**************************************************

This code is the schemas.
Only declarations and single-line functions in this file.

functions are implemented in tree.cpp

**************************************************/


#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>

#include "settings.h"
#include "misc.cpp" 
#include "image_layer.h"

#define MAX_TOLERANCE 7

class Edge {
public :
    int a, b; // Node ID
    int weight;
};

bool smaller_edge(const Edge & a, const Edge & b) 
    { return a.weight < b.weight; }

class MergeSet {
public :
    int n;  int f[NODES];
    int siz[NODES], max_weight[NODES];
    int find(int x); 
    void init(int x);
    void merge(int a, int b, int w);
    bool good(int a, int b, int w);
};

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
    int n ; //number of node
    PicturePtr rgb;     // can be 8-bit unsigned
    FloPicturePtr lab; 
    BytArray disparity; 
    Interval itv[NODES];


    BigObject() { rgb = NULL; lab = NULL; }
    BigObject(ImageLayer & image_layer) { init(image_layer); }
    void init(ImageLayer& image_layer);
    void initDisparity();
    void buildForest(double threshold, bool lab);
    void steroMatch(BigObject &ref, int sign, bool lab);
    void refinement();

    void readPrediction(BytArray disp);
    void noPrediction(int max_disp);
    void intersectInterval(const BigObject & small, double iou);


    inline int node_number(int x, int y) const { return x * W + y + 1; }
};




#endif
