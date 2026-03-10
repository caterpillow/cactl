/**
 * Author: caterpillow
 * Date: 2025-09-01
 * License: CC0
 * Source: idk
 * Description: Finds strongly connected components in a
 * directed graph. \texttt{comps} has edges right to left.
 * Time: O(E + V)
 * Status: good i think
 */
#pragma once

using G = vt<vi>; // vt<basic_string<int>> faster
struct SCC {
    int n; 
    G &adj, radj;
    vi todo, comp;
    SCC (G &adj) : adj(adj), n(size(adj)), radj(n), comp(n), todo(n) {
        F0R (i, size(adj)) dfs(i);
        for (int u : todo) rdfs(u, u); 
    }
    void dfs(int u) {
        if (comp[u]--) return;
        for (int v : adj[u]) dfs(v), radj[v].pb(u); 
        todo[--n] = u;
    }
    void rdfs(int u, int w) {
        if (comp[u] >= 0) return;
        comp[u] = w; 
        // if (u == w) comps.pb(u);
        for (int v : radj[u]) rdfs(v, w); 
    }
};