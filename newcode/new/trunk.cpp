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

void BigObject::compute_first_cost (int d, Picture rgb_r, FloArray gradient_r) { 
	// only the cost for disparity d.
	// assume left and right rgb image has the exact same size;
    double max_gradient_color_difference= 2.0;
    double max_color_difference = 10.0;
    double weight_on_color = 0.11;
    for (int x = 0; x < H; ++x)
        for (int y = 0; y < W; ++y) {
            double cost1 = 0;
            for (int c = 0; c < 3; ++c)
                cost1 += abs(rgb[x][y][c] - rgb_r[x][misc::max(y - d,0)][c]);
            cost1 = misc::min(cost1 / 3.0, max_color_difference); // here is weired
            double cost_gradient = misc::abs(gradient[x][y] - gradient_r[x][misc::max(0, y - d)]);
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
                edges[k].weight = misc::max(edges[k].weight, abs(rgb[i][j][c] - rgb[i+p][j+q][c]));
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

#endif
