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
    int ti = 0; G &adj;
    vi disc, comp, stk;
    SCC(int n, G &adj) : adj(adj), disc(n), comp(n, -1) {
        F0R (i, n) if (!disc[i]) dfs(i);
        // reverse(all(comps));
    }
    int dfs(int u) {
        int low = disc[u] = ++ti; 
        stk.pb(u);
        for (int v : adj[u]) if (comp[v] == -1)
            low = min(low, disc[v] ?: dfs(v)); 
        if (low == disc[u]) {
            // comps.pb(u);
            for (int y = -1; y != u;) 
                comp[y = stk.back()] = u, stk.pop_back();
        }
        return low;
    }
};