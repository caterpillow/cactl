/**
 * Author: caterpillow, Claude
 * Date: 2026-09-25
 * License: CC0
 * Source: Tarjan, Yannakakis 1984
 * Description: A graph is chordal (every cycle of length $\ge 4$ has a
 * chord) iff it has a perfect elimination ordering: each vertex's
 * neighbours later in the order form a clique. \texttt{mcs} returns an
 * order that is perfect iff one exists: maximum cardinality search
 * (always take the unvisited vertex with the most visited neighbours)
 * reversed. \texttt{isPeo} checks any order: with $p$ the earliest later
 * neighbour of $v$, the other later neighbours of $v$ must be adjacent
 * to $p$. If chordal: max clique $= \max_v 1 + |$later neighbours$|$ =
 * chromatic number, and greedy colouring from the back is optimal.
 * Simple graphs only.
 * Usage: bool chordal = isPeo(g, mcs(g));
 * Time: O(V + E)
 * Status: stress-tested
 */
#pragma once

vi mcs(vt<vi> &g) {
    int n = size(g), hi = 0; vi w(n), ord; vt<vi> b(n + 1);
    F0R (v, n) b[0].pb(v);
    while (size(ord) < n) {
        while (b[hi].empty()) hi--;
        int v = b[hi].back(); b[hi].pop_back();
        if (w[v] < 0) continue; // stale: visited from higher up
        w[v] = -1, ord.pb(v);
        for (int u : g[v]) if (w[u] >= 0)
            b[++w[u]].pb(u), hi = max(hi, w[u]);
    }
    reverse(all(ord));
    return ord;
} // <hash>

bool isPeo(vt<vi> &g, const vi &ord) {
    int n = size(g); vi pos(n), mk(n, -1); vt<vi> ch(n);
    F0R (i, n) pos[ord[i]] = i;
    F0R (v, n) {
        int p = -1;
        for (int u : g[v]) if (pos[u] > pos[v] &&
            (p < 0 || pos[u] < pos[p])) p = u;
        if (p >= 0) ch[p].pb(v);
    }
    F0R (p, n) {
        for (int u : g[p]) mk[u] = p;
        for (int v : ch[p]) for (int u : g[v])
            if (pos[u] > pos[v] && u != p && mk[u] != p) return 0;
    }
    return 1;
}
