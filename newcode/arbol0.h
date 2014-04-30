#ifndef FORESTS_CODE_H_OCT_2013
#define FORESTS_CODE_H_OCT_2013


/**************************************************

This code is the schemas.
Only declarations and single-line functions in this file.

functions are implemented in trunk.cpp and branch.cpp

If you want a new kind of tree, 
create a new branch.cpp and change the silly.cpp includings.

**************************************************/


#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>

#include "settings.h"
#include "misc.cpp" 

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
    int find(int x); 
    void init(int x);
    bool merge(int a, int b);
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


class BigObject {
public :
    int n, m; // number of nodes and edges
    int ts; // number of tree edges
    int H, W; // graph size, height and width
    inline int node_number(int x, int y) { return x * W + y + 1; }
    inline void node_location(int p, int &x, int &y) {--p; x = p / W; y = p % W; }
	MergeSet mset; // for buiding the tree
    Edge edges[NODES * 2]; // all candidate edges  1-based
    Edge trees[NODES]; // collected tree edges 1-based
    // next three are in trunck.cpp
    void collect_edges(); // collect all the edges.
	void prepare_visit();  // construct the bfs order for the forest
    // next three are in branch.cpp
    void build_tree(); // build the tree given all the collected edges.

	void compute_cost_on_tree();
    void compute_gradient();

    TreeNode nodes[NODES];
    bool visited[NODES];
	int order[NODES]; // the sequence of index, the visiting order of the tree

    void computeFirstCost (int d, BigObject & right) ;

    void copyLeftCostToRight (int d, BigObject & left);

    void getDisparity(BigObject & ref, bool left);

	FloArray cost, gradient, backup, best_cost; // no idea 
	BytArray disparity; 
    Picture rgb;     // can be 8-bit unsigned
    IntArray stable; // can be 16-bit signed
};

#endif
