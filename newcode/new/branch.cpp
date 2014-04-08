#ifndef CODE_FOR_SEPCIFIC_TREE_this_is_for_MST
#define CODE_FOR_SEPCIFIC_TREE_this_is_for_MST

#include <algorithm>

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

void BigObject::build_tree() {
    //std::sort(edges + 1, edges + m + 1, smaller_edge); // this is not good.. a bit lazy
    srand(time(NULL));
    std::random_shuffle(edges + 1, edges + m + 1);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i)
        if (mset.merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
        }
}

FloArray backup, best_cost;
double table[259];

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

void BigObject::getDisparity(BigObject & ref, bool left) {
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            disparity[i][j] = 255;
            best_cost[i][j] = 103456789;
        }

    table[0] = 1;
    for(int i = 1; i <= 255; i++)
        table[i] = table[i-1] * exp(-1.0 / sigma_const);//weight table

    // assume the tree is constructed.

    for (int d = 0; d < max_disparity; ++d) {
        if (left) compute_first_cost(d, ref.rgb, ref.gradient);
            else compute_first_cost(-d, ref.rgb, ref.gradient);
        compute_cost_on_tree();
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j)
                if (cost[i][j] < best_cost[i][j]) {
                    best_cost[i][j] = cost[i][j];
                    disparity[i][j] = d;
                }
    }
}

#endif
