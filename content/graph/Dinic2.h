/**
 * Author: caterpillow, Claude
 * Date: 2026-09-10
 * License: CC0
 * Source: Dinic.h
 * Description: Dinic.h, faster: 16-byte edges, one DFS per phase that keeps
 * pushing after a success and kills dead ends, no capacity scaling.
 * \texttt{calc(s, t, lim)} pushes at most lim more units from the current
 * state and returns them, so between calls the residual graph may be
 * edited as long as it stays a valid flow: both residuals of a pair
 * $\ge 0$ with a constant sum, and conservation at every vertex but s, t.
 * Raise the cap of edge $i$ of $a$ by $d$: \texttt{adj[a][i].c += d}, then
 * \texttt{calc(s, t)}. Lower it below its flow $f$ ($=$ the reverse
 * residual if rcap $= 0$) to $c'$: $x = f - c'$, \texttt{adj[a][i].c = 0},
 * reverse \texttt{.c -= x}, then \texttt{z = x - calc(a, b, x); calc(a, s, z);
 * calc(t, b, z); flow -= z; flow += calc(s, t);}. left\_of\_min\_cut is for
 * the last \texttt{calc(s, t)}. A calc that finds nothing
 * is one BFS, $O(V + E)$; one that finds $F$ costs $O(E)$ per augmenting
 * path and phase, so incremental use is never worse than from scratch.
 * Time: O(V^2 E), O(E \sqrt V) on unit graphs. 1.5--2x Dinic.h without its
 * scaling, 2--16x with it, 1.3--14x PushRelabel.h: doc/dinic.md
 * Status: stress-tested, incl. the edit recipes
 */
#pragma once

struct Dinic {
    struct Edge { int to, rev; ll c; }; // rcap 0: flow = rev's c
    vt<vt<Edge>> adj; vi lvl, ptr, q;
    void init(int n) { adj.resize(n), lvl = ptr = q = vi(n); }
    void ae(int a, int b, ll c, ll rcap = 0) {
        adj[a].pb({b, size(adj[b]), c});
        adj[b].pb({a, size(adj[a]) - 1, rcap});
    } // <hash>
    ll dfs(int v, int t, ll f) { // pushes up to f
        if (v == t) return f;
        ll g = 0;
        for (int &i = ptr[v]; i < size(adj[v]); i++) {
            auto &[to, rev, c] = adj[v][i];
            if (lvl[to] != lvl[v] + 1 || !c) continue;
            ll w = min(f - g, c), p = dfs(to, t, w);
            if (p < w) lvl[to] = -1; // nothing left behind it
            c -= p, adj[to][rev].c += p, g += p;
            if (g == f) break; // not i++: edge may have room
        }
        return g;
    } // <hash>
    ll calc(int s, int t, ll lim = LLONG_MAX) {
        ll flow = 0; q[0] = s;
        do {
            fill(all(lvl), 0), fill(all(ptr), 0);
            int qi = 0, qe = lvl[s] = 1;
            while (qi < qe && !lvl[t]) {
                int v = q[qi++];
                for (Edge e : adj[v]) if (!lvl[e.to] && e.c)
                    q[qe++] = e.to, lvl[e.to] = lvl[v] + 1;
            }
            ll p = dfs(s, t, lim); flow += p, lim -= p;
        } while (lvl[t] && lim);
        return flow;
    }
    bool left_of_min_cut(int a) { return lvl[a]; }
};
