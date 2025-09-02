/**
 * Author: chilli
 * Date: 2019-04-26
 * License: CC0
 * Source: https://cp-algorithms.com/graph/dinic.html
 * Description: Flow algorithm with complexity $O(VE\log U)$ where $U = \max |\text{cap}|$.
 * $O(\min(E^{1/2}, V^{2/3})E)$ if $U = 1$; $O(\sqrt{V}E)$ for bipartite matching.
 * Status: Tested on SPOJ FASTFLOW and SPOJ MATCHING, stress-tested
 */
#pragma once

struct Dinic {
    struct Edge {
        int to, rev;
        ll c, oc;
        ll flow() { return max(oc - c, 0LL); } // if you need flows
    };
    vi lvl, ptr, q;
    vt<vt<Edge>> adj;
 
    void init(int n) {
        lvl = ptr = q = vi(n);
        adj.resize(n);
    }
 
    void ae(int a, int b, ll c, ll rcap = 0) {
        adj[a].pb({b, size(adj[b]), c, c});
        adj[b].pb({a, size(adj[a]) - 1, rcap, rcap});
    }
    ll dfs(int v, int t, ll f) {
        if (v == t || !f) return f;
        for (int& i = ptr[v]; i < size(adj[v]); i++) {
            auto &[to, rev, c, _] = adj[v][i];
            if (lvl[to] == lvl[v] + 1)
                if (ll p = dfs(to, t, min(f, c))) {
                    c -= p, adj[to][rev].c += p;
                    return p;
                }
        }
        return 0;
    }
    ll calc(int s, int t) {
        ll flow = 0; q[0] = s;
        F0R (L, 31) do { // 'int L = 30' maybe faster for random data
            lvl = ptr = vi(size(q));
            int qi = 0, qe = lvl[s] = 1;
            while (qi < qe && !lvl[t]) {
                int v = q[qi++];
                for (Edge e : adj[v])
                    if (!lvl[e.to] && e.c >> (30 - L))
                        q[qe++] = e.to, lvl[e.to] = lvl[v] + 1;
            }
            while (ll p = dfs(s, t, LLONG_MAX)) flow += p;
        } while (lvl[t]);
        return flow;
    }
    bool left_of_min_cut(int a) { return lvl[a] != 0; }
};