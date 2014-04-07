#ifndef CODE_FOR_SEPCIFIC_TREE_this_is_for_MST
#define CODE_FOR_SEPCIFIC_TREE_this_is_for_MST

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

void BigObject::build_Tree() {
    //std::sort(edges + 1, edges + m + 1, smaller_edge); // this is not good.. a bit lazy
    srand(time(NULL));
    std::random_shuffle(edges + 1, edges + m + 1);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i)
        if (mset.merge(edges[i].a, edges[i].b)) {
            trees[++ts] = edges[i];
        }
}

void compute_cost_on_tree(double sigma = 255 * 0.1 ) {

    table[0] = 1;
    for(int i = 1; i <= 255; i++)
        table[i] = table[i-1] * exp(-1.0 / sigma);//weight table

    copy(cost);
    for (int i = n; i >= 1; --i) {
        int p = order[i];
        for (int j = 0; j < nodes[p].degree; ++j) {
            int q = nodes[p].next_node[j];
            if (nodes[q].ord < nodes[p].ord) continue; // parent node has smaller order number. q is the child.
//               cout << i <<  ' ' << j << ' ' <<  endl;
            // double w = exp(-1.0 * nodes[q].up_weight / sigma_const);
            double w = table[nodes[q].up_weight];
            for (int d = 0; d < cost.array; ++d) {
//                    cout << d << ' ' << nodes[p].x << ' ' << nodes[p].y << endl;
                double value_p = backup[d][nodes[p].x][nodes[p].y];
                double value_q = backup[d][nodes[q].x][nodes[q].y];
                value_p += w * value_q;
                backup[d][nodes[p].x][nodes[p].y] = value_p;
            }
        }
    }
    for (int d = 0; d < cost.array; ++d) {
        cost[d][nodes[order[1]].x][nodes[order[1]].y] = backup[d][nodes[order[1]].x][nodes[order[1]].y];
    } // ERROR: forgot this part.
    for (int i = 1; i <= n; ++i) {
        int p = order[i];
        for (int j = 0; j < nodes[p].degree; ++j) {
            int q =  nodes[p].next_node[j];
            if (nodes[q].ord < nodes[p].ord) continue; // ERROR: Not > but <. q is child
            // double w = exp(-1.0 * nodes[q].up_weight / sigma_const);
            double w = table[nodes[q].up_weight];
            for (int d = 0; d < cost.array; ++d) {
                double value_q_current = backup[d][nodes[q].x][nodes[q].y];
                double value_p = cost[d][nodes[p].x][nodes[p].y];
                cost[d][nodes[q].x][nodes[q].y] = w * (value_p - w* value_q_current) + value_q_current;
            }
        }
    }
} // compute cost on tree


#endif
