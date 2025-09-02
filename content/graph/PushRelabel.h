/**
 * Author: Simon Lindholm
 * Date: 2015-02-24
 * License: CC0
 * Source: Wikipedia, tinyKACTL
 * Description: Push-relabel using the highest label selection rule and the gap heuristic. Quite fast in practice.
 *  To obtain the actual flow, look at positive values only.
 * Time: $O(V^2\sqrt E)$
 * Status: Tested on Kattis and SPOJ, and stress-tested
 */
#pragma once

template<typename flow_t = long long>
struct PushRelabel {
    struct Edge {
        int to, rev;
        flow_t f, c;
    };
    vt<vt<Edge>> g;
    vt<flow_t> ec;
    vt<Edge*> cur;
    vt<vt<int>> hs;
    vt<int> h;

    void init(int n) {
        g.resize(n);
        ec.resize(n);
        cur.resize(n);
        hs.resize(2 * n);
        h.resize(n);
    }

    void ae(int s, int t, flow_t cap, flow_t rcap = 0) {
        if (s == t) return;
        g[s].push_back({t, size(g[t]), 0, cap});
        g[t].push_back({s, size(g[s]) - 1, 0, rcap});
    }
    void add_flow(Edge& e, flow_t f) {
        Edge &back = g[e.to][e.rev];
        if (!ec[e.to] && f)
            hs[h[e.to]].push_back(e.to);
        e.f += f; e.c -= f;
        ec[e.to] += f;
        back.f -= f; back.c += f;
        ec[back.to] -= f;
    }
    flow_t calc(int s, int t) {
        int v = size(g);
        h[s] = v;
        ec[t] = 1;
        vt<int> co(2 * v);
        co[0] = v - 1;
		F0R (i, v) cur[i] = g[i].data();
        for(auto &e : g[s]) add_flow(e, e.c);
        if (size(hs[0]))
        for (int hi = 0; hi >= 0;) {
            int u = hs[hi].back();
            hs[hi].pop_back();
            while (ec[u] > 0) // discharge u
                if (cur[u] == g[u].data() + size(g[u])) {
                    h[u] = 1e9;
                    for (auto &e : g[u])
                        if (e.c && h[u] > h[e.to] + 1)
                            h[u] = h[e.to] + 1, cur[u] = &e;
                    if (++co[h[u]], !--co[hi] && hi < v)
						F0R (i, v)
                            if (hi < h[i] && h[i] < v)
                                --co[h[i]], h[i] = v + 1;
                    hi = h[u];
                } else if (cur[u]->c && h[u] == h[cur[u]->to] + 1)
                    add_flow(*cur[u], min(ec[u], cur[u]->c));
                else ++cur[u];
            while (hi >= 0 && hs[hi].empty()) --hi;
        }
        return -ec[s];
    }
    bool leftOfMinCut(int a) { return h[a] >= size(g); }
};
