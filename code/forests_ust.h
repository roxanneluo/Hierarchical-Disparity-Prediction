/* implement the wilson's uniform random tree */


#ifndef FORESTS_CODE_H_OCT_2013
#define FORESTS_CODE_H_OCT_2013
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>

#include "gridcode.h"
#include "mylib.h"

const float sigma_const = 255 * 0.1;

class Edge {
public :
    int a, b; // Node ID
    int weight;
    bool connect(int x, int y) {
        return (x == a && y == b) || (x == b && y == a);
    }
};

bool operator < (const Edge &d, const Edge &e) {
    if (d.weight < e.weight) return true;
    if (d.weight > e.weight) return false;
    if (d.a < e.a) return true;
    if (d.a > e.a) return false;
    return d.b < e.b;
}
//    bool operator > (const Edge &e) { return e < *this;  }

bool operator == (const Edge &d, Edge &e) {
    //if (e.a > e.b)  {int temp = e.a; e.a = e.b; e.b = temp;}
    return (d.weight == e.weight) && (d.a == e.a) && (d.b == e.b);
}


class MergeSet {
public :
    int n;
    int *f;
    int find(int x) {
        if (x != f[x]) f[x] = find(f[x]);
        return f[x];
    }
    void init(int x) {
        n = x;
        f = (int*) malloc((n + 2) * sizeof(int));
        for (int i = 0; i <= n; ++i) f[i] = i;
    }
    bool merge(int a, int b) {
        if (find(a) != find(b)) {
            if (rand() & 1) f[find(a)] = find(b);
            else f[find(b)] = find(a);
            return true;
        } else return false;
    }
};

typedef vector<int> VI;

class Graph {

    MergeSet mset; // for buiding mst
    Edge* edges; // all candidate edges  1-based

public :
    Graph() {
        edges = NULL;
        trees = NULL;
        listing = NULL; // the linked list of the graph; linked_list[i][j] = j-th edge of node i, [i][0] = number of edges;
        randord = NULL;
        wilson = NULL; //check if the node is visited
        last = NULL;
        blocks = NULL; // mark if this area has at least one point
    }
    //Graph(const Graph& g) {}
    Edge* trees; // collected tree edges 1-based
    int n, m; // number of nodes and edges 1-based
    int ts; // number of tree edges
    int H, W; // graph info
    int node_number(int x, int y) {
        return x * W + y + 1;
    }
    void node_location(int p, int &x, int &y) {
        --p;
        x = p / W;
        y = p % W;
    }

    template<class type>
    void collect_edges_edgeaware(Array3<type>& rgb) {
        printf("before collect edges\n");
        fflush(stdout);
        H = rgb.height;
        W = rgb.width;
        if (edges == NULL)
            edges = (Edge *) malloc((m + 2) * sizeof(Edge));
        if (trees == NULL)
            trees = (Edge *) malloc((n + 2) * sizeof(Edge));
        int k = 0;
        int a, b, weight, weight_tmp;
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j) {
                a = b = -1;
                weight = weight_tmp = 256;
                for (int p = -1; p < 1; ++p)
                    for (int q = -1; q < 1; ++q) {
                        if (mylib::ABS(p + q) == 1) {
                            if (i + p < H && j + q < W && i + p >= 0 && j + q >= 0) {
                                weight_tmp = mylib::max3abs(
                                                 rgb[0][i][j] - rgb[0][i+p][j+q],
                                                 rgb[1][i][j] - rgb[1][i+p][j+q],
                                                 rgb[2][i][j] - rgb[2][i+p][j+q]);
                                if (weight_tmp < weight) {
                                    a = node_number(i, j);
                                    b = node_number(i+p, j+q);
                                    weight = weight_tmp;
                                }
                            }
                        }
                    }
                if (a != -1 && b != -1) {
                    ++k;
                    edges[k].a = a;
                    edges[k].b = b;
                    edges[k].weight = weight;
                }
            }
        m = k;
        printf("after collect edges\n");
        fflush(stdout);
    }

    template <class type>
    void collect_edges(Array3<type> & rgb) {
        H = rgb.height;
        W = rgb.width;
        n = H * W;
        m = 2 * H * W - H - W;
        if (edges == NULL)
            edges = (Edge *) malloc((m + 2) * sizeof(Edge));
        if (trees == NULL)
            trees = (Edge *) malloc((n + 2) * sizeof(Edge));
        int k = 0;
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j)
                for (int p = 0; p < 2; ++p)
                    for (int q = 0; q < 2; ++q) if (p + q == 1)
                            if (i + p < H && j + q < W) {
                                ++k;
                                edges[k].a = node_number(i, j);
                                edges[k].b = node_number(i+p, j+q);
                                edges[k].weight = mylib::max3abs(rgb[0][i][j] - rgb[0][i+p][j+q],
                                                                 rgb[1][i][j] - rgb[1][i+p][j+q],
                                                                 rgb[2][i][j] - rgb[2][i+p][j+q]);
                            }
        m = k;
        // printf("%d\n", m);
        // fflush(stdout);
    }
    template <class type>
    void collect_edges_edgeaware2 (Array3<type> & rgb) {
        H = rgb.height;
        W = rgb.width;
        n = H * W;
        m = 2 * H * W - H - W;
        if (edges == NULL)
            edges = (Edge *) malloc((m + 2) * sizeof(Edge));
        if (trees == NULL)
            trees = (Edge *) malloc((n + 2) * sizeof(Edge));
        int k = 0;
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j)
                for (int p = 0; p < 2; ++p)
                    for (int q = 0; q < 2; ++q) if (p + q == 1)
                            if (i + p < H && j + q < W) {
                                ++k;
                                edges[k].a = node_number(i, j);
                                edges[k].b = node_number(i+p, j+q);
                                edges[k].weight = mylib::max3abs(rgb[0][i][j] - rgb[0][i+p][j+q],
                                                                 rgb[1][i][j] - rgb[1][i+p][j+q],
                                                                 rgb[2][i][j] - rgb[2][i+p][j+q]);
                            }
        std::sort(edges + 1, edges + k + 1);
        m = int(0.7 * k);
    }

    template<class type>
    void collect_edges_edgeaware(Array3<type>& rgb, Grid<type>& occ) {
        printf("before collect edges\n");
        fflush(stdout);
        H = rgb.height;
        W = rgb.width;
        if (edges == NULL)
            edges = (Edge *) malloc((m + 2) * sizeof(Edge));
        if (trees == NULL)
            trees = (Edge *) malloc((n + 2) * sizeof(Edge));
        int k = 0;
        int a, b, weight, weight_tmp;
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j) {
                a = b = -1;
                weight = weight_tmp = 256;
                for (int p = -1; p < 1; ++p)
                    for (int q = -1; q < 1; ++q) {
                        if (mylib::ABS(p + q) == 1) {
                            if (i + p < H && j + q < W && i + p >= 0 && j + q >= 0) {
                                weight_tmp = mylib::max3abs(
                                                 rgb[0][i][j] - rgb[0][i+p][j+q],
                                                 rgb[1][i][j] - rgb[1][i+p][j+q],
                                                 rgb[2][i][j] - rgb[2][i+p][j+q]);
                                if (occ[i][j] == 0) {
                                    ++k;
                                    edges[k].a = node_number(i, j);
                                    edges[k].b = node_number(i + p, j + q);
                                    edges[k].weight = weight_tmp;
                                    continue;
                                }
                                if (weight_tmp < weight) {
                                    a = node_number(i, j);
                                    b = node_number(i+p, j+q);
                                    weight = weight_tmp;
                                }
                            }
                        }
                    }
                if (a != -1 && b != -1) {
                    ++k;
                    edges[k].a = a;
                    edges[k].b = b;
                    edges[k].weight = weight;
                }
            }
        m = k;
        printf("after collect edges %d\n", m);
        fflush(stdout);
    }
    int **listing; // the linked list of the graph; linked_list[i][j] = j-th edge of node i, [i][0] = number of edges;
    int * randord;
    bool * wilson; //check if the node is visited
    int * last ;
    bool * blocks ; // mark if this area has at least one point
    vector<int> path, through; // vector is used, for i really don't know how large path could be. compile with -O2 would be faster
    // path : the nodes walked, through : the edges walked, lerw : the loop erase random walk
    void build_MST() {
      std::sort(edges + 1, edges + m + 1);
      mset.init(n);
      ts = 0;
      for (int i = 1; i <= m; ++i) {
        if (mset.merge(edges[i].a, edges[i].b)) {
          trees[++ts] = edges[i];
        }
      }
    }
    void build_RandTree() {

        srand(time(NULL));
        // if you want to delete any edges, put your code in collect_edges()
        // here will assume that the graph is connected with the set of edges in edges[]
        if (listing == NULL) {
            listing = (int **) malloc((n + 2) * sizeof(int *));
            for (int i = 0; i < n + 2; ++i) {
                listing[i] = (int*) malloc(5 * sizeof(int));
            }
        }
        if (randord == NULL)
            randord = (int *) malloc((n + 2) * sizeof (int));
        for (int i = 1; i <= n; ++i) randord[i] = i;
        if (wilson == NULL)
            wilson = (bool *) malloc((n + 2) * sizeof (bool));
        for (int i = 0; i < n + 2; ++i) wilson [i] = 0;
        if (last == NULL)
            last = (int *) malloc((n + 2) * sizeof (int));
        if (blocks == NULL)
            blocks = (bool *) malloc((n + 2) * sizeof (bool));
        memset(blocks, 0, sizeof(bool) * (n + 2));
        mset.init(n);
        for (int i = 1; i <= m; ++i)
            mset.merge(edges[i].a, edges[i].b);
        for (int i = 0; i < n + 2; ++i) {
            listing[i][0] = 0;
        }
        for (int i = 1, t; i <= m; ++i) {
            int x = edges[i].a, y = edges[i].b;
            t = ++listing[x][0];
            listing[x][t] = i;
            t = ++listing[y][0];
            listing[y][t] = i;
        }

        printf("build_RandTree\n");
        fflush(stdout);
        // This part is very slow in the refinement. Probably it is
        // becase of the value of "m" is too small.
        random_shuffle(randord + 1, randord + 1 + n);
        ts = 0;
        for (int i = 1; i <= n; ++i) {
            int p = randord[i];
            if (wilson[p]) continue;
            int blk = mset.find(p);
            if (!blocks[blk]) {
                wilson[p] = 1;
                blocks[blk] = 1;
                continue;
            }
            path.clear();
            path.push_back(p);
            while (!wilson[p]) {
                int next = mylib::randint(1, listing[p][0]);
                int q = edges[listing[p][next]].a;
                if (q == p) q = edges[listing[p][next]].b;
                through.push_back(listing[p][next]);
                p = q;
                path.push_back(p);
            }
            for (size_t i = 0; i < path.size(); ++i)
                last[path[i]] = i;
            int le = path[0], rw;
            while (le != p) {
                wilson[le] = 1;
                //trees[++ts] = edges[through[le]];
                rw = path[last[le] + 1];
                for (int j = listing[le][0]; j > 0; --j) {
                    int e = listing[le][j];
                    if (edges[e].connect(le, rw))
                        trees[++ts] = edges[e];
                }
                le = rw;
            }
        }
        printf("tree --- %d %d\n", ts, n);
        fflush(stdout);
        /*
        sort(trees + 1, trees + 1 + ts);
        int x00 = unique(trees + 1, trees + 1 + ts) - trees;
        printf("%d %d\n", x00, ts);
        sort(edges + 1, edges + 1 + m);
        x00 = unique(edges + 1, edges + 1 + m) - edges;
        printf("%d %d\n", x00, m);
        */
    }
};

class TreeNode {
public :
    int x, y, id ; // id = its index in an array
    int ord, up_weight; // the bfs order and the edge weight to parent after direct tree constructed
    TreeNode() {}
    TreeNode(int a, int b) : x(a) , y(b) {}
    int degree, next_node[4], edge_weight[4];
    void add_edge(int node, int weight) {
        next_node[degree] = node;
        edge_weight[degree] = weight;
        ++degree;
    }
};

class Forest {

    // static const double sigma_const = 255 * 0.1; // is used in calculation of match cost

    int n; // number of tree nodes
    TreeNode * nodes;
    bool * visited;
    int * order; // the sequence of index, the visiting order of the tree
    Array3<double> backup; // used in calculate cost on tree
    double table[256]; // weight table
public :
    Forest() {}
    Forest(const Forest& f) {}
    void init(Graph & g) {
        n = g.n;
//        cout << "n=" << n << endl;
        nodes = (TreeNode *) malloc((n + 2) * sizeof(TreeNode));
        visited = (bool *) malloc((n + 2) * sizeof(bool));
        for (int i = 1; i <= n; ++i) {
            nodes[i].id = i;
            nodes[i].degree = 0;
        }
        for (int i = 1; i <= g.ts; ++i) {
            int aa = g.trees[i].a, bb = g.trees[i].b, zz = g.trees[i].weight;
            g.node_location(aa, nodes[aa].x, nodes[aa].y);
            g.node_location(bb, nodes[bb].x, nodes[bb].y);
            nodes[aa].add_edge(bb, zz);
            nodes[bb].add_edge(aa, zz);
        }
//        for (int i = 1; i <= n; ++i) printf("%d ", nodes[i].degree);
    }

    void order_of_visit() {
        order = (int * ) malloc((n + 2) * sizeof(int));
        for (int i = 1; i <= n; ++i)
            nodes[i].ord = -1;
        int num = 0;
        while (1) {
            int root = -1;
            for (int i = 1; i <= n; ++i)
                if (nodes[i].ord == -1) {
                    root = i;
                    break;
                }
            if (root == -1) break;
            //printf("%d\n", root); fflush(stdout);
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
    void update_table(double sigma ) {
        table[0] = 1;
        for(int i = 1; i <= 255; i++)
            table[i] = table[i-1] * exp(-1.0 / sigma);//weight table
    }
    void compute_cost_on_tree(Array3<double> & cost, double sigma = 255 * 0.1 ) {
        update_table(sigma);
        backup.copy(cost);
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


};

#endif
