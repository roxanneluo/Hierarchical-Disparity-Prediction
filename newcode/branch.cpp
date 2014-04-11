#ifndef CODE_FOR_SEPCIFIC_TREE_this_is_for_MST
#define CODE_FOR_SEPCIFIC_TREE_this_is_for_MST

#include <algorithm>

void MergeSet::init(int x) {
    n = x;
    for (int i = 0; i <= n; ++i) f[i] = i;
}

bool MergeSet::merge(int a, int b) {
    if (find(a) != find(b)) { f[find(a)] = find(b);
        return true;
    } else
    return false;
}


void BigObject::build_tree() {
    std::sort(edges + 1, edges + m + 1, smaller_edge);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i)
        if (mset.merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
        }
}

void BigObject::compute_cost_on_tree( double sigma ) {

    updateTable(sigma);

    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            backup[i][j] = cost[i][j];

    for (int i = n; i >= 1; --i) {
        int p = order[i];
        for (int j = 0; j < nodes[p].degree; ++j) {
            int q = nodes[p].next_node[j];
            if (nodes[q].ord < nodes[p].ord) continue; 
            double w = table[nodes[q].up_weight];
            double value_p = backup[nodes[p].x][nodes[p].y];
            double value_q = backup[nodes[q].x][nodes[q].y];
            value_p += w * value_q;
            backup[nodes[p].x][nodes[p].y] = value_p;
        }
    }

    cost[nodes[order[1]].x][nodes[order[1]].y] = backup[nodes[order[1]].x][nodes[order[1]].y];
    for (int i = 1; i <= n; ++i) {
        int p = order[i];
        for (int j = 0; j < nodes[p].degree; ++j) {
            int q =  nodes[p].next_node[j];
            if (nodes[q].ord < nodes[p].ord) continue; 
            double w = table[nodes[q].up_weight];
            double value_q_current = backup[nodes[q].x][nodes[q].y];
            double value_p = cost[nodes[p].x][nodes[p].y];
            cost[nodes[q].x][nodes[q].y] = w * (value_p - w* value_q_current) + value_q_current;
        }
    }
} // compute cost on tree

// In the following functions, 
// I put left and right together to save the ij iterations.
// this is inside the 0-maxdisparity iteration, so I need to save it.
void initDisparity() {
    for (int i = 0; i < right.H; ++i)
        for (int j = 0; j < right.W; ++j) {
            //left.disparity[i][j] = 0;
            left.best_cost[i][j] =  1e100;
            //right.disparity[i][j] = 0; 
            right.best_cost[i][j] = 1e100;
        }
}

void updateDisparity(int d) {
    for (int i = 0; i < left.H; ++i)
        for (int j = 0; j < left.W; ++j) {
            if (left.cost[i][j] < left.best_cost[i][j]) {
                left.best_cost[i][j] = left.cost[i][j];
                left.disparity[i][j] = d;
            }
            if (right.cost[i][j] < right.best_cost[i][j]) {
                right.best_cost[i][j] = right.cost[i][j];
                right.disparity[i][j] = d;
            }
        }
}

void updateMatchingCost(int disp) {
    // If (i,j) is an unstable pixel, cost is 0 for all disparities.
    // Otherwise update it.
    int H = left.H, W = right.W;
    for (int i = 0; i < H; ++i) 
        for (int j = 0; j < W; ++j) {
            if (left.stable[i][j] >= 0) 
                left.cost[i][j] = misc::abs(disp - left.stable[i][j]);
            else left.cost[i][j] = 0;

            if (right.stable[i][j] >= 0) 
                right.cost[i][j] = misc::abs(disp - right.stable[i][j]);
            else right.cost[i][j] = 0;
        }
}


void findStablePixels() {
    for (int i = 0; i < left.H; ++i)
        for (int j = 0; j < right.W; ++j) {
            int d = left.disparity[i][j];
            left.stable[i][j] = left.disparity[i][j]; // if stable[i][j] = -1, then it is not stable
            if (j - d < 0 || d == 0 || misc::abs(right.disparity[i][j - d] - d) >= 1) 
                left.stable[i][j] = -1;
            d = right.disparity[i][j];
            right.stable[i][j] = right.disparity[i][j];
            if (j + d >= left.W || d == 0 || misc::abs(left.disparity[i][j + d] - d) >= 1) 
                right.stable[i][j] = -1;
        }
}
            
#endif
