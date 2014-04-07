
int MergeSet::find() {
    if (x != f[x]) f[x] = find(f[x]);
    return f[x];
}

int MergeSet::init(int n) {
    for (int i = 0 ; i <= n; ++i) f[i] = i;
}


