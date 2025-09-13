/**
 * Author: Benjamin Qi, Oleksandr Kulkov, chilli
 * Date: 2020-01-12
 * License: CC0
 * Source: https://codeforces.com/blog/entry/53170, https://github.com/bqi343/USACO/blob/master/Implementations/content/graphs%20(12)/Trees%20(10)/HLD%20(10.3).h
 * Description: Decomposes a tree into vertex disjoint heavy paths and light
 * edges such that the path from any leaf to the root contains at most log(n)
 * light edges. Code does additive modifications and max queries, but can
 * support commutative segtree modifications/queries on paths and subtrees.
 * Takes as input the full adjacency list. in\_edges being true means that
 * values are stored in the edges, as opposed to the nodes. All values
 * initialized to the segtree default. Root must be 0.
 * Time: O((\log N)^2)
 * Status: stress-tested against old HLD
 */
#pragma once

#include "../data-structures/LazySegmentTree.h"

template<bool in_edges> struct HLD {
    int n;
    vt<vi> adj;
    vi par, root, depth, sz, pos;
    int time;
    SegTree tree;
    void ae(int u, int v) {
        adj[u].pb(v);
        adj[v].pb(u);
    }
    void dfs_sz(int u) {
        sz[u] = 1;
        for (int& v : adj[u]) {
            par[v] = u;
            depth[v] = depth[u] + 1;
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
        n = _n;
        adj.resize(n);
        par = root = depth = sz = pos = vi(n);
    }
    void gen(int r = 0) {
        par[r] = depth[r] = time = 0;
        dfs_sz(r);
        root[r] = r;
        dfs_hld(r);
        tree.init(n);
    }
    int lca(int u, int v) {
        while (root[u] != root[v]) {
            if (depth[root[u]] > depth[root[v]]) swap(u, v);
            v = par[root[v]];
        }
        return depth[u] < depth[v] ? u : v;
    }
    template <class Op>
    void process(int u, int v, Op op) {
        for (;; v = par[root[v]]) {
            if (pos[u] > pos[v]) swap(u, v);
            if (root[u] == root[v]) break;
            op(pos[root[v]], pos[v] + 1);
        }
        op(pos[u] + in_edges, pos[v] + 1);
    }
    void upd(int u, int v, ll upd) {
        process(u, v, [&] (int l, int r) { 
            tree.upd(l, r, upd); 
        });
    }
    ll query(int u, int v) {
        ll res = 0;
        process(u, v, [&] (int l, int r) { 
            res = res + tree.query(l, r); 
        });
        return res;
    }
};