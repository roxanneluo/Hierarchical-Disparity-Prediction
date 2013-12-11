#ifndef FORESTS_CODE_H_OCT_2013
#define FORESTS_CODE_H_OCT_2013
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <vector>
#include "gridcode.h"
#include "mylib.h"
#include "filter.h"
#define MAX_TOLERANCE 7

const float sigma_const = 255 * 0.1;

class Edge {
public :
    int a, b; // Node ID
    int weight;

    Edge() {}
    Edge(const int& aa, const int& bb, const int& ww) {
      a = aa;
      b = bb;
      weight = ww;
    }

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
public :
    int find(int x) {
        if (x != f[x]) f[x] = find(f[x]);
        return f[x];
    }
    void init(int x) {
        n = x; f = (int*) malloc((n + 2) * sizeof(int));
        for (int i = 0; i <= n; ++i) f[i] = i;
    }
    bool merge(int a, int b) {
        if (find(a) != find(b)) {
            if (rand() & 1) f[find(a)] = find(b); else f[find(b)] = find(a);
            return true;
        } else return false;
    }
};

class Interval {
public :
    int l, r;
    Interval() : l(0), r(-1)  {}
    Interval(int a, int b) : l(a), r(b) {}
    Interval cap(Interval b) {
        return Interval(mylib::max(l, b.l), mylib::min(r, b.r));
    }
		Interval uu(Interval b) {
			  return Interval(mylib::min(l, b.l), mylib::max(r, b.r));
		}
    int length() { return r - l + 1; } // Error: Should be r - l - 1;
};

class Graph {

public :
    Edge* edges; // all candidate edges  1-based
    MergeSet mset; // for buiding mst
    Graph() {}
    Graph(const Graph& g) {}
    Edge* trees; // collected tree edges 1-based
    int n, m; // number of nodes and edges
    int boundary_m;
    int ts; // number of tree edges
    int H, W; // graph info
    int node_number(int x, int y) { return x * W + y + 1; }
    void node_location(int p, int &x, int &y) {--p; x = p / W; y = p % W; }
    Interval* itv;
    
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
                                     rgb[2][i][j] - rgb[2][i+p][j+q]); 
            }
    }

    template <class type>
    void collect_edges_with_interval(Array3<type> & rgb, Grid<type>& disp, Grid<int>& interval) {
        H = rgb.height; W = rgb.width;
        n = H * W;
        m = 2 * H * W - H - W;
				edges = (Edge *) malloc((m + 2) * sizeof(Edge));
        trees = (Edge *) malloc((n + 2) * sizeof(Edge));
        
				itv = (Interval *) malloc((n + 2) * sizeof(Interval));
       
			  printf("%d %d\n", interval[disp[0][0]][0], interval[disp[0][0]][1]);	 
				for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
           itv[node_number(i, j)].l = interval[disp[mylib::min(i / 2, disp.height - 1)][mylib::min(j / 2, disp.width - 1)]][0];
           itv[node_number(i, j)].r = interval[disp[mylib::min(i / 2, disp.height - 1)][mylib::min(j / 2, disp.width - 1)]][1]; 
            // now this will be fine
        }
        int k = 0;
        for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            for (int p = 0; p < 2; ++p)
            for (int q = 0; q < 2; ++q) if (p + q == 1)
            if (i + p < H && j + q < W) {
                int XX = node_number(i, j), YY = node_number(i+p, j+q);
                if (itv[XX].cap(itv[YY]).length() <= 0) {
									m--;
							   	continue;
								}
                ++k;
                edges[k].a = XX; edges[k].b = YY;
                edges[k].weight = mylib::max3abs(rgb[0][i][j] - rgb[0][i+p][j+q],
                                     rgb[1][i][j] - rgb[1][i+p][j+q],
                                     rgb[2][i][j] - rgb[2][i+p][j+q]); 
            }
    }

    template <class type>
    void collect_edges_with_interval(Array3<type> & rgb, Grid<type>& disp, vector < Grid<int> >& interval) {
        H = rgb.height; W = rgb.width;
        n = H * W;
        m = 2 * H * W - H - W;
				edges = (Edge *) malloc((m + 2) * sizeof(Edge));
        trees = (Edge *) malloc((n + 2) * sizeof(Edge));
        
				itv = (Interval *) malloc((n + 2) * sizeof(Interval));
        int p;
			  // printf("%d %d\n", interval[1]disp[0][0]][0], interval[1][disp[0][0]][1]);	 
				for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
           int prei = mylib::min(i / 2, disp.height - 1);
           int prej = mylib::min(j / 2, disp.width - 1);

        if (prei < disp.height / 2) {
          p = prej < disp.width / 2 ? 1 : 2;
        } else {
          p = prej < disp.width / 2 ? 3 : 4;
        }
           itv[node_number(i, j)].l = interval[p][disp[prei][prej]][0];
           itv[node_number(i, j)].r = interval[p][disp[prei][prej]][1]; 
            // now this will be fine
        }
        int k = 0;
        for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            for (int p = 0; p < 2; ++p)
            for (int q = 0; q < 2; ++q) if (p + q == 1)
            if (i + p < H && j + q < W) {
                int XX = node_number(i, j), YY = node_number(i+p, j+q);
                if (itv[XX].cap(itv[YY]).length() <= 0) {
									m--;
							   	continue;
								}
                ++k;
                edges[k].a = XX; edges[k].b = YY;
                edges[k].weight = mylib::max3abs(rgb[0][i][j] - rgb[0][i+p][j+q],
                                     rgb[1][i][j] - rgb[1][i+p][j+q],
                                     rgb[2][i][j] - rgb[2][i+p][j+q]); 
            }
    }

    void build_tree_with_interval(double threshold,
				                          bool build_MST = false) {
        if (build_MST) {
					std::sort(edges + 1, edges + m + 1, smaller_edge); // m + 1 ?
				} else {
					std::random_shuffle(edges + 1, edges + m + 1);
				}
			  mset.init(n);
        ts = 0;
        for (int i = 1; i <= m; ++i) {
            Interval t1 = itv[mset.find(edges[i].a)];
            Interval t2 = itv[mset.find(edges[i].b)];
            Interval t3 = t1.cap(t2);
						Interval t4 = t1.uu(t2);
            double tmp = t3.length();
            // printf("XXX"); fflush(stdout);
            tmp /= t4.length(); // (t1.length() + t2.length());
            // printf("YYY"); fflush(stdout);
            // what this threshold thing ? IDK
            if ( tmp < threshold ) continue;
            if (mset.merge(edges[i].a, edges[i].b)) {
                trees[++ts] = edges[i];
                itv[mset.find(edges[i].a)] = t4; // t3;
            }
        }
    }

    void build_MST() {
      // printf("THIS IS FOREST2.H\n");
      std::sort(edges + 1, edges + m + 1, smaller_edge);
      mset.init(n);
      ts = 0;
      for (int i = 1; i <= m; ++i) {
        if (mset.merge(edges[i].a, edges[i].b)) {
          trees[++ts] = edges[i];
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
    Graph * graph;
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
        graph = & g;
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

  void compute_cost_on_tree_with_interval(Array3<double> & cost, double sigma = 255 * 0.1 ) {
    update_table(sigma);
    backup.copy(cost);
    for (int i = n; i >= 1; --i) {
      int p = order[i];
      for (int j = 0; j < nodes[p].degree; ++j) {
        int q = nodes[p].next_node[j];
        if (nodes[q].ord < nodes[p].ord) continue; // parent node has smaller order number. q is the child.
        double w = table[nodes[q].up_weight];

        // The possibility model used here :
        // int set_number_p = graph->mset.find(p);
				// int set_number_q = graph->mset.find(q);
				// printf("set: %d %d\n", set_number_p, set_number_q);
				// fflush(stdout);
				Interval bound = graph->itv[graph->mset.find(p)].cap(Interval(0, cost.array-1));        
        // bound.l = mylib::max(0, bound.l);
			  // bound.r = mylib::min(bound.r, cost.array - 1);	
        for (int d = bound.l; d <= bound.r; ++d) {
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
    /*!!!!!!
    I dont know if this part is still necessary.
    But the I shoud reconsider the boundaries.
    */
    for (int i = 1; i <= n; ++i) {
      int p = order[i];
      for (int j = 0; j < nodes[p].degree; ++j) {
        int q =  nodes[p].next_node[j];
        if (nodes[q].ord < nodes[p].ord) continue; // ERROR: Not > but <. q is child
        // double w = exp(-1.0 * nodes[q].up_weight / sigma_const);
        double w = table[nodes[q].up_weight];
        Interval bound = graph->itv[graph->mset.find(p)].cap(Interval(0, cost.array-1));        
        for (int d = bound.l; d <= bound.r; ++d) {
          double value_q_current = backup[d][nodes[q].x][nodes[q].y];
          double value_p = cost[d][nodes[p].x][nodes[p].y];
          cost[d][nodes[q].x][nodes[q].y] = w * (value_p - w* value_q_current) + value_q_current;
        }
      }
    }
  } // compute cost on tree
  template <class type>
	void compute_disparity_on_tree_with_interval(Array3<double>& cost,
			                                         Grid<type>& disparity) {
	  disparity.reset(cost.height, cost.width);
		for (int i = 1; i <= n; ++i) {
		  int p = order[i];
			Interval bound = graph->itv[graph->mset.find(p)].cap(Interval(0, cost.array - 1));
			int best_dis = mylib::min(cost.array - 1, bound.l);
			int xx = nodes[p].x, yy = nodes[p].y;
			double best = cost[best_dis][xx][yy];
			for (int d = bound.l + 1; d <= mylib::min(cost.array - 1, bound.r); ++d) {
			  if (cost[d][xx][yy] < best) {
				  best = cost[d][xx][yy];
					best_dis = d;
				}	
			}
			disparity[xx][yy] = best_dis;
		}
	}

	template <class type>
	void update_matching_cost_on_tree_with_interval(Array3<double>& cost,
			                                            Grid<type>& disparity,
																									Grid<type>& occlusion) {
	  int arr = cost.array;
		int h = cost.height;
		int w = cost.width;
		cost.zero();
		for (int i = 1; i <= n; ++i) {
		  int p = order[i];
			Interval bound = graph->itv[graph->mset.find(p)].cap(Interval(0, cost.array - 1));
			bound.l = min(bound.l, cost.array - 1);
			bound.r = min(bound.r, cost.array - 1);
			if (occlusion[nodes[p].x][nodes[p].y] == 0) {
			  for (int d = bound.l; d <= bound.r; ++d) {
				  cost[d][nodes[p].x][nodes[p].y] =
						  mylib::ABS(d - disparity[nodes[p].x][nodes[p].y]);
				}
			}
		}
	}

	template <class type>
	void compute_first_cost_on_tree_with_interval(Array3<double>& cost,
			                                          Array3<type>& rgb_left,
																								Array3<type>& rgb_right,
																								Grid<float>& gradient_left,
																								Grid<float>& gradient_right,
																								int max_disparity,
																								bool is_left = true) {
	  int H, W;
		double search_range = 0;
		cost.reset(max_disparity, H = rgb_left.height, W = rgb_right.width);
		double max_gradient_color_difference = 2.0;
		double max_color_difference = 11.0;
		double weight_on_color = 0.11;// 0.20; // 0.11;
		for (int i = 1; i <= n; ++i) {
		  int p = order[i];
			int px = nodes[p].x;
			int py = nodes[p].y;
			Interval bound = graph->itv[graph->mset.find(p)].cap(Interval(0, cost.array - 1));
			bound.l = min(bound.l, cost.array - 1);
			bound.r = min(bound.r, cost.array - 1);
			search_range += bound.r - bound.l + 1;
			for (int d = bound.l; d <= bound.r; ++d) {
				double first_cost = 0;
			  for (int c = 0; c < 3; ++c) {
					if (is_left)
						first_cost += mylib::ABS(rgb_left[c][px][py] -
			          rgb_right[c][px][mylib::max(0, py - d)]);
				  else
					  first_cost += mylib::ABS(rgb_right[c][px][py] -
						    rgb_left[c][px][mylib::min(rgb_left.width - 1, py + d)]);
				}
				first_cost = mylib::min(first_cost / 3, max_color_difference);
				double cost_gradient = is_left ?
			      (double) mylib::ABS(gradient_left[px][py] -
																gradient_right[px][mylib::max(0, py - d)]) :
						(double) mylib::ABS(gradient_right[px][py] -
								                gradient_left[px][mylib::min(gradient_left.width - 1, py + d)]);
				cost_gradient = mylib::min(cost_gradient, max_gradient_color_difference);
				cost[d][px][py] = weight_on_color * first_cost + (1 - weight_on_color) * cost_gradient;
			}	
		}
		printf("average search range: %.4lf\n", search_range / n / max_disparity);
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

      support[nodes[order[1]].x][nodes[order[1]].y]=backup[0][nodes[order[1]].x][nodes[order[1]].y] = 255;
        for (int i = 1; i <= n; ++i) {
            int p = order[i];
            for (int j = 0; j < nodes[p].degree; ++j) {
                int q = nodes[p].next_node[j];
                if (nodes[q].ord < nodes[p].ord) continue;

                support[nodes[q].x][nodes[q].y] = backup[0][nodes[q].x][nodes[q].y]
                                                = backup[0][nodes[p].x][nodes[p].y]*table[nodes[q].up_weight];
            }
        }
  }
};

#endif
