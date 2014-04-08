#ifndef BASIC_CODE_FOR_GENERAL_FOREST
#define BASIC_CODE_FOR_GENERAL_FOREST

int MergeSet::find(int x) {
    if (f[x] != x) 
        f[x] = find(f[x]);
    return f[x];
}

void TreeNode::add_edge(int node, int weight) {
    next_node[degree] = node;
    edge_weight[degree] = weight;
    ++degree;
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
            edges[k].weight = mylib::max3abs(rgb[i][j][0] - rgb[i+p][j+q][0],
                                 rgb[i][j][1] - rgb[i+p][j+q][1],
                                 rgb[i][j][2] - rgb[i+p][j+q][2]); 
        }
}

void BigObject::prepare_visit() {
    //next part would reorganize the tree
    for (int i = 1; i <= n; ++i) {
        nodes[i].id = i;
        nodes[i].degree = 0;
    }
    for (int i = 1; i <= g.ts; ++i) {
        int aa = trees[i].a, bb = trees[i].b, zz = trees[i].weight;
        g.node_location(aa, nodes[aa].x, nodes[aa].y);
        g.node_location(bb, nodes[bb].x, nodes[bb].y);
        nodes[aa].add_edge(bb, zz);
        nodes[bb].add_edge(aa, zz);
    }
    // next part would get the BFS order
    for (int i = 1; i <= n; ++i) 
        nodes[i].ord = -1;
    int num = 0;
    while (1) {
        int root = -1;
        for (int i = 1, j = mylib::randint(1, n); i <= n; ++i) {
            if (nodes[i].ord == -1) { root = i; break; }
            if (++j > n) j = 1;
        }
        if (root == -1) break;
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

#endif
