/**
 * Author: idk
 * Date: 2025-09-01
 * License: CC0
 * Source: i forgot
 * Description: Fast incremental bipartite matching. Zero-indexed.
 * Usage: \texttt{operator[]} for the pair of right node i, \texttt{n} is the size of the rhs, \texttt{add(v)} to add adjacency list of node on lhs 
 * Time: O(\sqrt{V}E)
 * Status: tested i thinks
 */
#pragma once

struct Matching : vi {
    vt<vi> adj;
    vi rank, low, pos, vis, seen;
    int k{0};
    // n = size of rhs
    Matching(int n) :vi(n, -1), rank(n) {}
    bool add(vi vec) {
        adj.pb(std::move(vec));
        low.pb(0), pos.pb(0), vis.pb(0);
        if (size(adj.back())) {
            int i = k;
        nxt:
            seen.clear();
            if (dfs(size(adj)-1, ++k-i)) return 1;
            for (auto &v : seen) for (auto &e : adj[v])
                if (rank[e] < inf && vis[at(e)] < k)
                    goto nxt;
            for (auto &v : seen) {
                low[v] = inf;
				for (auto &w : adj[v]) rank[w] = inf;
            }
        }
        return 0;
    }
    bool dfs(int v, int g) {
        if (vis[v] < k) vis[v] = k, seen.pb(v);
        while (low[v] < g) {
            int e = adj[v][pos[v]];
            if (at(e) != v && low[v] == rank[e]) {
                rank[e]++;
                if (at(e) == -1 || dfs(at(e), rank[e]))
                    return at(e) = v, 1;
            } else if (++pos[v] == size(adj[v])) {
                pos[v] = 0; low[v]++;
            }
        }
        return 0;
    }
};