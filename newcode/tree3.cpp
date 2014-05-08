#ifndef BASIC_CODE_FOR_DPF_ST_afih1p289rionnjk
#define BASIC_CODE_FOR_DPF_ST_afih1p289rionnjk

// this code is for dpf + ST

#include <algorithm>
// TimeKeeper	timer;


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

void BigObject::build_tree(double threshold) {

    std::sort(edges + 1, edges + m + 1, smaller_edge); 
    //std::random_shuffle(edges + 1, edges + m + 1);
    mset.init(n); ts = 0;
    for (int i = 1; i <= m; ++i) {
        int a = edges[i].a, b = edges[i].b;
        Interval t1 = itv[mset.find(a)];
        Interval t2 = itv[mset.find(b)];
        Interval t3 = t1.cap(t2);
        Interval t4 = t1.cup(t2);
        double tmp = t3.length();
        tmp /= t4.length(); 
        if ( tmp < threshold ) continue;

        if (mset.find(a) != mset.find(b)) 
            if (mset.good(a, b, edges[i].weight)) {
                trees[++ts] = edges[i]; 
                mset.merge(a, b, edges[i].weight);
            }
    }
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
            itv[node_number(i, j)].l = dpf::interval[disp[i / 2][j / 2]][0];
            itv[node_number(i, j)].r = dpf::interval[disp[i / 2][j / 2]][1];
        }
}

void BigObject::buildForest(double threshold) {
    collect_edges();
    build_tree(threshold);
    prepare_visit();
    compute_gradient();
}

void BigObject::steroMatch(BigObject &ref, int sign) {
    // sign is a thing for the first cost.
    for (int i = 1; i <= numOT; ++i) {
        Interval treeInterval = itv[mset.find(order[oneTree[i][0]])];
        int low = oneTree[i][0], high = oneTree[i][1];
        for (int d = treeInterval.l; d <= treeInterval.r; ++d) {
            computeFirstCost(d * sign, ref, low, high); // sign is used here
            compute_cost_on_tree(low, high);
            updateDisparity(d, low, high);
        }
    }
}

void BigObject::shrinkPicture(BigObject & obj) {
    obj.H = H / 2;
    obj.W = W / 2;
    for (int i = 0; i < H; ++i) 
    for (int k = 0; k < 3; ++k)
        rgb[i][W][k] = rgb[i][W - 1][k];
    for (int i = 0; i < W; ++i) 
    for (int k = 0; k < 3; ++k)
        rgb[H][i][k] = rgb[H - 1][i][k];
    for (int k = 0; k <3; ++k) rgb[H][W][k] = rgb[H-1][W-1][k];
    
    for (int i = 0; i < obj.H; ++i)
    for (int j = 0; j < obj.W; ++j)
    for (int k = 0; k < 3; ++k) {
        int x = i * 2, y = j * 2;
        obj.rgb[i][j][k] = (rgb[x][y][k]+rgb[x+1][y][k]+rgb[x][y+1][k]+rgb[x+1][y+1][k]) / 4;
    }
}
#endif
