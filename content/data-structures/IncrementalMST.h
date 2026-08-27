/**
 * Author: caterpillow
 * Date: 2025-09-13
 * License: CC0
 * Source: some yosupo submission
 * Description: Incremental minimum spanning forest keyed by weight.f
 * (globally distinct). Interface: \texttt{merge(u, v, w)} inserts edge
 * (u, v) and returns the edge that is now NOT in the forest ({inf, -1}
 * if none was displaced); \texttt{find(u, w)} = representative of u's
 * component using only edges of weight $\le w$ (default: all), so
 * connectivity ``as of'' any threshold is a query, no deletions needed;
 * \texttt{max\_edge(u, v)} = vertex whose parent edge is the heaviest on
 * the u--v path (-1 if disconnected); \texttt{delete\_max\_edge(u, v, w)}
 * removes the edge of weight w, valid only if w is the current maximum
 * weight in the whole structure. Offline dynacon: weight = -deletion
 * time, answer queries at time t with find(u, -t-1) == find(v, -t-1).
 * Time: O(\log n) expected
 * Status: stress-tested
 */
#pragma once

struct DSU {
    vi par, pri;
    vt<pi> weight;

    DSU(int n): par(n), pri(n), weight(n) {
        for (int i = 0; i < n; ++i) {
            par[i] = pri[i] = i;
            weight[i] = pi{inf, -1};
        }
        shuffle(all(pri), mt19937(random_device{}()));
    }

    int parent(int u) {
        if (par[u] == u) return par[u];
        while (weight[par[u]].f <= weight[u].f) {
            par[u] = par[par[u]];
        }
        return par[u];
    }

    int find(int u, int w = inf - 1) {
        while (weight[u].f <= w) u = parent(u);
        return u;
    }

    int connect(int v, int w = inf - 1) {
        while (weight[v].f <= w) {
            v = par[v];
        }
        return v;
    }

    void add_edge(int u, int v, pi w) { // link two components
        while (u != v) {
            u = connect(u, w.f);
            v = connect(v, w.f);
            if (pri[u] < pri[v]) swap(u, v);
            swap(par[v], u);
            swap(weight[v], w);
        }
        connect(u);
    }

    int max_edge(int u, int v) {
        if (find(u) != find(v)) return -1;
        while (true) {
            if (weight[u].f > weight[v].f) swap(u, v);
            if (par[u] == v) break;
            u = par[u];
        }
        return u;
    }

    void delete_edge(int v, int w) {
        while (par[v] != v) {
            if (weight[v].f == w) {
                par[v] = v;
                weight[v] = {inf, -1};
                return;
            }
            v = parent(v);
        }
    }

    // delete edge (u, v) of weight w; w must be the global max
    void delete_max_edge(int u, int v, int w) {
        delete_edge(u, w);
        delete_edge(v, w);
    }

    // return weight of deleted edge; {inf, -1} otherwise
    pi merge(int u, int v, pi w) {
        if (u == v) return w;
        int p = max_edge(u, v);
        if (p == -1) {
            add_edge(u, v, w);
            return {inf, -1};
        } else if (weight[p].f > w.f) {
            pi res = weight[p];
            delete_edge(p, res.f);
            add_edge(u, v, w);
            return res;
        }
        return w;
    }
};