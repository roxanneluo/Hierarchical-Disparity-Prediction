#ifndef __TREE_COMMON__
#define __TREE_COMMON__
class Edge {
public :
    int a, b; // Node ID
    int weight;
};

bool smaller_edge(const Edge & a, const Edge & b) 
    { return a.weight < b.weight; }


class TreeNode {
public :
    int x, y, id ; // id = its index in an array
    int ord, up_weight; // the bfs order and the edge weight to parent after direct tree constructed
    int degree, next_node[4], edge_weight[4];
    TreeNode() : degree(0), ord(-1) {}
    TreeNode(int a, int b)
      : x(a) , y(b), degree(0), ord(-1) {}
    void add_edge(int node, int weight);
};

void TreeNode::add_edge(int node, int weight) {
    next_node[degree] = node;
    edge_weight[degree] = weight;
    ++degree;
}



double table[333];
void updateTable(double sigma) {
    table[0] = 1;
    for(int i = 1; i <= 255; i++)
        table[i] = table[i-1] * exp(-1.0 / sigma);
}

#endif
