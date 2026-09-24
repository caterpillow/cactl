/**
 * Author: caterpillow, Claude
 * Date: 2026-09-25
 * License: CC0
 * Source: Tarjan, Yannakakis 1984
 * Description: Recognises a chordal graph (every cycle of length $\ge 4$
 * has a chord) and gives a perfect elimination ordering: each vertex's
 * neighbours later in \texttt{peo} form a clique. Built by maximum
 * cardinality search (repeatedly take the unvisited vertex with the most
 * visited neighbours) with lazy buckets; \texttt{peo} is that order
 * reversed, and it is perfect iff the graph is chordal. Check: with $p$
 * the neighbour of $v$ visited last before $v$, the other earlier
 * neighbours of $v$ must all be neighbours of $p$. If chordal: max clique
 * $= \max_v 1 + |$later neighbours$|$ = chromatic number, colouring
 * greedily along reversed \texttt{peo} is optimal. Simple graphs only.
 * Time: O(V + E)
 * Status: stress-tested
 */
#pragma once

struct Chordal {
    vi peo, pos; bool ok = 1; // pos: index in the search
    Chordal(vt<vi> &g) {
        int n = size(g), hi = 0; vi w(n), par(n, -1), mk(n, -1);
        vt<vi> b(n+1), ch(n); pos.assign(n, -1);
        F0R (v, n) b[0].pb(v);
        F0R (i, n) {
            int v = -1;
            while (v < 0) {
                while (b[hi].empty()) hi--;
                v = b[hi].back(), b[hi].pop_back();
                if (pos[v] >= 0) v = -1; // stale: seen higher up
            }
            pos[v] = i, peo.pb(v);
            for (int u : g[v]) if (pos[u] < 0)
                b[++w[u]].pb(u), hi = max(hi, w[u]);
        } // <hash>
        for (int v : peo) {
            for (int u : g[v]) if (pos[u] < pos[v] && (par[v] < 0
                || pos[u] > pos[par[v]])) par[v] = u;
            if (par[v] >= 0) ch[par[v]].pb(v);
        }
        F0R (p, n) {
            for (int u : g[p]) mk[u] = p;
            for (int v : ch[p]) for (int u : g[v])
                if (pos[u] < pos[v] && u != p && mk[u] != p)
                    ok = 0;
        }
        reverse(all(peo));
    }
};
