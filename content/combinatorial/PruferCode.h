/**
 * Author: caterpillow
 * Date: 2026-08-27
 * License: CC0
 * Description: Pr\"ufer code of a labeled tree, 0-indexed. encode takes
 * the adjacency list ($n \ge 2$) and returns the $n-2$ codes; decode
 * returns the $n-1$ edges. Bijection between trees on $n$ vertices and
 * sequences in $[0, n)^{n-2}$.
 * Time: O(n)
 * Status: stress-tested
 */
#pragma once

vi prufer_encode(vt<vi>& adj) {
    int n = size(adj), ptr = 0;
    vi par(n, -1), deg(n), code(n - 2), st{n - 1};
    while (size(st)) { // parents towards root n-1
        int u = st.back(); st.pop_back();
        for (int v : adj[u]) if (v != par[u]) par[v] = u, st.pb(v);
    }
    F0R (i, n) deg[i] = size(adj[i]);
    while (deg[ptr] != 1) ptr++;
    int leaf = ptr;
    F0R (i, n - 2) {
        int nxt = code[i] = par[leaf];
        if (--deg[nxt] == 1 && nxt < ptr) leaf = nxt;
        else { while (deg[++ptr] != 1); leaf = ptr; }
    }
    return code;
}

vt<pi> prufer_decode(vi& code) {
    int n = size(code) + 2, ptr = 0;
    vi deg(n, 1); for (int v : code) deg[v]++;
    while (deg[ptr] != 1) ptr++;
    int leaf = ptr; vt<pi> edges;
    for (int v : code) {
        edges.pb({leaf, v});
        if (--deg[v] == 1 && v < ptr) leaf = v;
        else { while (deg[++ptr] != 1); leaf = ptr; }
    }
    edges.pb({leaf, n - 1});
    return edges;
}
