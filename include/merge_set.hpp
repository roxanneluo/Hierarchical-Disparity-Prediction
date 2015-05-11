#ifndef __MERGE_SET__
#define __MERGE_SET__
class MergeSet {
public :
    int n;  int f[NODES];
    int find(int x); 
    void init(int x);
    bool merge(int a, int b);
};

int MergeSet::find(int x) {
    if (f[x] != x) 
        f[x] = find(f[x]);
    return f[x];
}

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

#endif
