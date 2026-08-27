/**
 * Author: Benjamin Qi, Oleksandr Kulkov, chilli
 * Date: 2020-01-12
 * License: CC0
 * Source: https://codeforces.com/blog/entry/53170, https://github.com/bqi343/USACO/blob/master/Implementations/content/graphs%20(12)/Trees%20(10)/HLD%20(10.3).h
 * Description: Decomposes a tree into vertex disjoint heavy paths and light
 * edges such that the path from any leaf to the root contains at most log(n)
 * light edges. \texttt{process(u, v, op)} calls \texttt{op(l, r)} on
 * $O(\log N)$ half-open ranges of positions covering the u--v path; pair it
 * with any range structure indexed by \texttt{pos}. Subtree of u is
 * \texttt{[pos[u], pos[u] + sz[u])}. in\_edges true stores values on edges
 * (the range for a path skips the LCA). Takes the full adjacency list;
 * root must be 0.
 * Time: O((\log N)^2)
 * Status: stress-tested against old HLD
 */
#pragma once

template<bool in_edges> struct HLD {
    int n, time;
    vt<vi> adj;
    vi par, root, sz, pos;
    HLD(vt<vi> &adj) : n(size(adj)), time(0), adj(adj), par(n), root(n), sz(n), pos(n) {
        dfs_sz(0);
        dfs_hld(0);
    }
    void dfs_sz(int u) {
        sz[u] = 1;
        for (int& v : adj[u]) {
            par[v] = u;
            adj[v].erase(find(all(adj[v]), u));
            dfs_sz(v);
            sz[u] += sz[v];
            if (sz[v] > sz[adj[u][0]]) swap(v, adj[u][0]);
        }
    }
    void dfs_hld(int u) {
        pos[u] = time++;
        for (int& v : adj[u]) {
            root[v] = (v == adj[u][0] ? root[u] : v);
            dfs_hld(v);
        }
    }
    void init(int _n) {
        n = _n, time = 0;
        adj.resize(n);
        par = root = sz = pos = vi(n);
    }
    template <class Op>
    void process(int u, int v, Op op) {
        for (; ; v = par[root[v]]) {
            if (pos[u] > pos[v]) swap(u, v);
            if (root[u] == root[v]) break;
            op(pos[root[v]], pos[v] + 1);
        }
        op(pos[u] + in_edges, pos[v] + 1); // u is lca
    }
};