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

    bool operator > (Edge &e) {
        //if (e.a > e.b)  {int temp = e.a; e.a = e.b; e.b = temp;}

        if (weight > e.weight) return true;
        else if (weight < e.weight) return false;

        if (a > e.a) return true;
        else if (a < e.a) return false;

        if (b > e.b) return true;
        else return false;
    }

    bool operator == (Edge &e) {
        //if (e.a > e.b)  {int temp = e.a; e.a = e.b; e.b = temp;}
        return (weight == e.weight) && (a == e.a) && (b == e.b);
    }
};
bool smaller_edge(const Edge & a, const Edge &b) {return a.weight < b.weight;}


class MergeSet {
    int n;  int *f;
    int *siz; int *max_weight;
public :
    int find(int x) {
        if (x != f[x]) f[x] = find(f[x]);
        return f[x];
    }
    void init(int x) {
        n = x; 
        f = (int*) malloc((n + 2) * sizeof(int));
        siz = (int*) malloc((n + 2) * sizeof(int));
        max_weight = (int*) malloc((n + 2) * sizeof(int));
        for (int i = 0; i <= n; ++i) {
            f[i] = i;
            siz[i] = 1;
            max_weight[i] = 0;
        }
    }
    void merge(int a, int b, int w) {
        siz[find(b)] += siz[find(a)];
        max_weight[find(b)] = mylib::max(max_weight[find(a)], max_weight[find(b)]);
        max_weight[find(b)] = mylib::max(w, max_weight[find(b)]);
        f[find(a)] = find(b); 
    }
    bool good(int a, int b, int w) {
        int x = find(a), y = find(b);
        return (w <= mylib::min(max_weight[x] + 1200.0 / siz[x], max_weight[y] + 1200.0 / siz[y]));
    }
};

class Graph {

    MergeSet mset; // for buiding mst
    Edge* edges; // all candidate edges  1-based

public :
		Graph() {}
		Graph(const Graph& g) {}
    Edge* trees; // collected tree edges 1-based
    int n, m; // number of nodes and edges
    int ts; // number of tree edges
    int H, W; // graph info
    int node_number(int x, int y) { return x * W + y + 1; }
    void node_location(int p, int &x, int &y) {--p; x = p / W; y = p % W; }

	template <class type>
    void collect_edges(Array3<type> & rgb) {
        H = rgb.height; W = rgb.width;
        n = H * W;
        m = 2 * H * W - H - W;
        edges = (Edge *) malloc((m + 2) * sizeof(Edge));
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
                                     rgb[2][i][j] - rgb[2][i+p][j+q]); // ERROR: max not min
            }
//        cout << k << ' ' << m << endl;
    }

    void build_RandTree() {
        //std::sort(edges + 1, edges + m + 1, smaller_edge); // this is not good.. a bit lazy
        std::sort(edges + 1, edges + m + 1, smaller_edge);
        mset.init(n); ts = 0;
        
        for (int i = 1; i <= m; ++i) {
            int a = edges[i].a, b = edges[i].b;
            if (mset.find(a) != mset.find(b)) 
                if (mset.good(a, b, edges[i].weight)) {
                    trees[++ts] = edges[i]; 
                    mset.merge(a, b, edges[i].weight);
//                cout << ts << endl;
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
    }
    void order_of_visit(int rootx, int rooty, int W) {
		order = (int * ) malloc((n + 2) * sizeof(int));
		for (int i = 1; i <= n; ++i) visited[i] = false;
		int num = 0;

        int root = rooty*W+rootx+1;
//            cout << root << endl;
        order[++num] = root;
        nodes[root].ord = num;
        nodes[root].up_weight = 0;
        visited[root] = true;
        for (int i = num; i <= num; ++i) { // this is a bfs
            int t = order[i];
            for (int j = 0; j < nodes[t].degree; ++j) {
                int p = nodes[t].next_node[j];
                if (!visited[p]) {
                    order[++num] = p;
                    nodes[p].ord = num;
                    nodes[p].up_weight = nodes[t].edge_weight[j];
                    visited[p] = true;
                }
            }
        } // end for bfs
	}
	void order_of_visit() {
		order = (int * ) malloc((n + 2) * sizeof(int));
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

    template <class type>
    void compute_support(Grid<type> &support, double sigma = 255 * 0.1) {
        update_table(sigma);
        backup.reset(1, support.height, support.width);

        support[nodes[order[1]].x][nodes[order[1]].y]=255;
        // support[nodes[order[1]].x][nodes[order[1]].y]=backup[0][nodes[order[1]].x][nodes[order[1]].y] = 255;

        for (int i = 1; i <= n; ++i) {
            int p = order[i];
            for (int j = 0; j < nodes[p].degree; ++j) {
                int q = nodes[p].next_node[j];
                if (nodes[q].ord < nodes[p].ord) continue;

                // support[nodes[q].x][nodes[q].y] = backup[0][nodes[q].x][nodes[q].y]
                //                                 = backup[0][nodes[p].x][nodes[p].y]*table[nodes[q].up_weight];
                support[nodes[q].x][nodes[q].y] = support[nodes[p].x][nodes[p].y]*table[nodes[q].up_weight];
            }
        }
    }

    template <class type>
    int findSupportSize(int starti, int startj, Grid<type> &support, int H, int W, double threshold = 0.8, double sigma = 255 * 0.1) {
        update_table(sigma);
        order_of_visit(startj, starti, W);
        support.reset(H,W);
        // update_table(sigma);

        support[nodes[order[1]].x][nodes[order[1]].y]=1;
        // support[nodes[order[1]].x][nodes[order[1]].y]=backup[0][nodes[order[1]].x][nodes[order[1]].y] = 255;
        int size = 0;
        for (int i = 1; i <= n; ++i) {
            int p = order[i];
            if (support[nodes[p].x][nodes[p].y] > threshold)
                    ++size;

            for (int j = 0; j < nodes[p].degree; ++j) {
                int q = nodes[p].next_node[j];
                if (nodes[q].ord < nodes[p].ord) continue;

                // support[nodes[q].x][nodes[q].y] = backup[0][nodes[q].x][nodes[q].y]
                //                                 = backup[0][nodes[p].x][nodes[p].y]*table[nodes[q].up_weight];
                support[nodes[q].x][nodes[q].y] = support[nodes[p].x][nodes[p].y]*table[nodes[q].up_weight];
               
            }
        }
        return size;
    }

    bool connected(int i1,int j1, int i2, int j2, Graph &g) {
        int n1 = g.node_number(i1,j1);
        int n2 = g.node_number(i2,j2);
        for (int i = 0; i < nodes[n1].degree; ++i) 
            if (nodes[n1].next_node[i] == n2)
                return true;
        return false;
    }
};

#endif
