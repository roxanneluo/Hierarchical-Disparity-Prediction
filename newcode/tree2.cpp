#ifndef BASIC_CODE_FOR_TREE_STsldkhfoqij23ionsd
#define BASIC_CODE_FOR_TREE_STsldkhfoqij23ionsd

// this is for simple segment tree

#include <algorithm>

double table[333];
void updateTable(double sigma) {
    table[0] = 1;
    for(int i = 1; i <= 255; i++)
        table[i] = table[i-1] * exp(-1.0 / sigma);
}

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


void TreeNode::add_edge(int node, int weight) {
    next_node[degree] = node;
    edge_weight[degree] = weight;
    ++degree;
}

void BigObject::compute_gradient () {
    double co[3] = { 0.299, 0.587, 0.114 };
    double plus = 0.5, minus = 0.5, gray;
    for (int x = 0; x < H; ++x) {
        for (int j = 0; j < 3; ++j)  {
            plus += co[j] * rgb[x][1][j]; // * rgb[j][x][1] not rgb[j][x][0]
            minus += co[j] * rgb[x][0][j]; // * rgb[j][x][0] not rgb[j][x][1]
        }
        gray = plus;
        gradient[x][0] = 127.5 + plus - minus;
        for (int i = 1; i <= W - 2; ++i) {
            plus = 0.5;
            for (int j = 0; j < 3; ++j)
                plus += co[j] * rgb[x][i + 1][j];
            gradient[x][i] = 0.5 * (plus - minus) + 127.5;
            minus = gray;
            gray = plus;
        }
        gradient[x][W - 1] = plus - minus + 127.5;
    }
}

const double max_gradient_color_difference= 2.0;
const double max_color_difference = 10.0;
const double weight_on_color = 0.11;
void BigObject::computeFirstCost (int d, BigObject & right) { 
	// only the cost for disparity d.
    for (int x = 0; x < H; ++x)
    for (int y = 0; y < W; ++y) {
        
        int where = y - d; if (where < 0) where = 0; if (where >= right.W) where = right.W - 1;
        double cost1 = 0;
        for (int c = 0; c < 3; ++c)
            cost1 += misc::abs(rgb[x][y][c] - right.rgb[x][where][c]);
        cost1 = misc::min(cost1 / 3.0, max_color_difference); // here is weired
        double cost_gradient = misc::abs(gradient[x][y] - right.gradient[x][where]);
        cost_gradient = misc::min(cost_gradient, max_gradient_color_difference);
        cost[x][y] = weight_on_color * cost1 + (1 - weight_on_color) * cost_gradient;
    }
}

void BigObject::collect_edges() {
    n = H * W;
    m = 2 * H * W - H - W;
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
                edges[k].weight = misc::max(edges[k].weight, misc::abs(rgb[i][j][c] - rgb[i+p][j+q][c]));
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
    int num = 0, root = 0;
    while (1) {
        bool found = false;
        for (int i = root + 1; i <= n; ++i) 
            if (nodes[i].ord == -1) 
                { root = i; found = true; break; }
        if (!found) break;
        order[++num] = root;
        nodes[root].ord = num;
        nodes[root].up_weight = 0;

        for (int i = num; i <= num; ++i) { // this is a bfs
            int t = order[i];
            for (int j = 0; j < nodes[t].degree; ++j) {
                int p = nodes[t].next_node[j];
                if (nodes[p].ord == -1) {
                    order[++num] = p;
                    nodes[p].ord = num;
                    nodes[p].up_weight = nodes[t].edge_weight[j];
                    visited[p] = true;
                }
            }
        } // end for bfs
    }// end for the while
}




void BigObject::build_tree() {

    //std::sort(edges + 1, edges + m + 1, smaller_edge);
    extra::sort(edges, 1, m);
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

void BigObject::compute_cost_on_tree( ) {

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
// In the following functions, 
// I put left and right together to save the ij iterations.
// this is inside the 0-maxdisparity iteration, so I need to save it.

void initDisparity(BigObject & left, BigObject & right) {
    for (int i = 0; i < right.H; ++i)
        for (int j = 0; j < right.W; ++j) {
            //left.disparity[i][j] = 0;
            left.best_cost[i][j] =  1e100;
            //right.disparity[i][j] = 0; 
            right.best_cost[i][j] = 1e100;
        }
}

void updateDisparity(int d, BigObject & left, BigObject & right) {
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

void updateMatchingCost(int disp, BigObject & left, BigObject & right) {
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


void findStablePixels(BigObject & left, BigObject & right) {
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
