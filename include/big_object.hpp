#ifndef __BIG_OBJECT__
#define __BIG_OBJECT__
#include "settings.hpp"
#include "misc.hpp" 
#include "tree_common.hpp"
#include "merge_set.hpp"
#include "extra.hpp"
#include "image_layer.hpp"
#include "timekeeper.hpp"

class BigObject {
  protected :
    int m; // number of edges
    int ts; // number of tree edges
    MergeSet *mset; // for buiding the tree
    Edge edges[NODES * 2]; // all candidate edges  1-based
    Edge trees[NODES]; // collected tree edges 1-based

    TreeNode nodes[NODES];
    bool visited[NODES];
	  int order[NODES]; // the sequence of index, the visiting order of the tree
	  int orderX[NODES], orderY[NODES];

	  FloArrayPtr gradient;  
    double best_cost[NODES], cost[NODES], backup[NODES]; // no idea 
    int stable[NODES]; // can be 16-bit signed

    virtual void prepare_visit();  // construct the bfs order for the forest 
    virtual void collect_edges(); // collect all the edges. 
    virtual void collect_lab_edges(); // collect all the edges.
    virtual void build_tree(); // build the tree given all the collected edges. 
    virtual void compute_cost_on_tree(int low = 1, int high = -1);
    virtual void update_disparity(int d, int low = 1, int high = -1);

    inline void node_location(int p, int &x, int &y) const  
      {--p; x = p / W; y = p % W; }
    inline int node_number(int x, int y) const { return x * W + y + 1; }
  public :
    int H, W; // graph size, height and width
    int n; // number of nodes
    PicturePtr rgb;     // can be 8-bit unsigned
    FloPicturePtr lab; 
    BytArray disparity; 

    BigObject(MergeSet * merge_set)
      :mset(merge_set) {rgb = NULL; lab = NULL;}
    BigObject(MergeSet *merge_set, ImageLayer & image_layer)
    :mset(merge_set), H(image_layer.H), W(image_layer.W)
    { init(image_layer); }

    void init(ImageLayer& image_layer);
    void initDisparity();
    void buildTree(bool use_lab = false);
    void updateMatchingCost(int disp, int low = 1, int high = -1);
    void copyLeftCostToRight (int d, BigObject & left, int low = 1, int high = -1);
    void computeFirstCost (int d, BigObject & right, int low = 1, int high = -1) ;
    void computeFirstLabCost (int d, BigObject & right, int low = 1, int high = -1) ;
    void stereoMatch(BigObject &ref, int sign, int max_disparity, 
        bool use_lab = false); // 


    friend void findStablePixels(BigObject & left, BigObject & right);
    friend void updateMatchingCost(int disp, BigObject & left, BigObject & right);

    inline void treeNodeLocation(int u, int &x, int &y) const {
      int p = order[u];
      node_location(p, x, y);
    }
};

void BigObject::init(ImageLayer & image_layer) {
  rgb = image_layer.rgb;
  lab = image_layer.lab;
  H = image_layer.H;
  W = image_layer.W;
  n = H*W;
  gradient = image_layer.gradient;
}

const double max_gradient_color_difference= 2.0;
const double max_color_difference = 10.0; // st and mst all set it to 7 in the original code. but we set it to 10
const double weight_on_color = 0.11;
void BigObject::computeFirstCost (int d, BigObject & right,
    int low, int high) { 
    if (high < 0) high = n;
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
    if (high < 0) high = n;
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

void BigObject::prepare_visit() {
    //next part would reorganize the tree
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
    }// end for finding a root
}
// In the following functions, 
// I put left and right together to save the ij iterations.
// this is inside the 0-maxdisparity iteration, so I need to save it.

void BigObject::initDisparity() {
    for (int i = 1; i <= n; ++i) 
        best_cost[i] = 1e30;
}

// winner takes all
void BigObject::update_disparity(int d, int low, int high) {
    if (high < 0) high = n;
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
    if (high < 0) high = n;
    for (int u = low; u <= high; ++u) {
        // int i = nodes[order[u]].x;
        // int j = nodes[order[u]].y;
        int t = order[u];
        if (stable[t] >= 0) 
           cost[t] = misc::abs(disp - stable[t]);
        else cost[t] = 0;
    }
}

void BigObject::compute_cost_on_tree(int low, int high) {
    // order[low .. high] is a connected tree.

    if (high < 0) high = n;
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

void BigObject::copyLeftCostToRight(int d, BigObject & left, int low, int high) {
  if(high < 0) high = n;

  int x,y, where;
  for (int i = low; i <= high; ++i) {
    x = orderX[i], y = orderY[i];
    where = y+d;
    if (where < W) cost[order[i]] = left.cost[node_number(x,where)];
    else cost[order[i]] = left.cost[node_number(x, W-1)];
  }
}

/*
void BigObject::copyLeftCostToRight(int d, BigObject & left) {
  for (int x = 0; x < H; ++x) {
	  for (int y = 0; y < W; ++y) {
		  int where = y + d;
      if (where < W) cost[x][y] = left.cost[x][y]; // I think it should be left.cost[x][where]
			else cost[x][y] = cost[x][y - 1];
		}
	}
}
TODO
*/ 


void BigObject::collect_edges() {
    n = H * W;
    //m = 2 * H * W - H - W;
    int k = 0;
    for (int i = 0; i < H; ++i)
    for (int j = 0; j < W; ++j)
        for (int p = 0; p < 2; ++p)
        for (int q = 0; q < 2; ++q) if (p + q == 1)
        if (i + p < H && j + q < W) {
            ++k;
            edges[k].a = node_number(i, j);
            edges[k].b = node_number(i+p, j+q);
            edges[k].weight = 0;
            for (int c = 0; c < 3; ++c) 
                edges[k].weight = misc::max(
                    edges[k].weight, misc::abs(rgb[i][j][c] - rgb[i+p][j+q][c]));
        }
    m = k;
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
            edges[++k].a = xx;
            edges[k].b = yy;
            edges[k].weight = ww;
        }
    m = k;
}

void BigObject::build_tree() {
    //std::sort(edges + 1, edges + m + 1, smaller_edge);
    extra::sort(edges, 1, m);
    mset->init(n); ts = 0;
    for (int i = 1; i <= m; ++i)
        if (mset->merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
        }
}

void BigObject::buildTree(bool use_lab) {
    if (use_lab) collect_lab_edges();
    else collect_edges();
	  build_tree();
	  prepare_visit();;
}

void BigObject::stereoMatch(BigObject &ref, int sign, int max_disparity,
    bool use_lab) {
    // sign is a thing for the first cost.
    for (int d = 0; d <= max_disparity; ++d) {
        // timer.check("first cost"); 
        if (use_lab) computeFirstLabCost(d * sign, ref); // sign is used here
        else computeFirstCost(d * sign, ref); // sign is used here
        compute_cost_on_tree();
        // timer.check("cost on tree");
        update_disparity(d);
    }
}



/*******************
 * for both left and right
 */
void updateMatchingCost(int disp, BigObject & left, BigObject & right) {
    // If (i,j) is an unstable pixel, cost is 0 for all disparities.
    // Otherwise update it.
    int H = left.H, W = right.W;
    int idx;
    for (int i = 0; i < H; ++i) 
        for (int j = 0; j < W; ++j) {
            idx = left.node_number(i,j);
            if (left.stable[idx] >= 0) 
                left.cost[idx] = misc::abs(disp - left.stable[idx]);
            else left.cost[idx] = 0;

            if (right.stable[idx] >= 0) 
                right.cost[idx] = misc::abs(disp - right.stable[idx]);
            else right.cost[idx] = 0;
        }
}


void findStablePixels(BigObject & left, BigObject & right) {
    int idx;
    for (int i = 0; i < left.H; ++i)
        for (int j = 0; j < right.W; ++j) {
            idx = left.node_number(i,j);
            int d = left.disparity[i][j];
            left.stable[idx] = left.disparity[i][j]; // if stable[i][j] = -1, then it is not stable
            if (j - d < 0 || d == 0 || misc::abs(right.disparity[i][j - d] - d) >= 1) 
                left.stable[idx] = -1;
            d = right.disparity[i][j];
            right.stable[idx] = right.disparity[i][j];
            if (j + d >= left.W || d == 0 || misc::abs(left.disparity[i][j + d] - d) >= 1) 
                right.stable[idx] = -1;
        }
}
            

#endif
