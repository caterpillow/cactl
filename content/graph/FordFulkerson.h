/**
 * Author: caterpillow
 * Date: 2025-09-01
 * License: CC0
 * Description: Short algo for computing maximum flows with a bounded answer.
 * Time: O(FM)
 * Status: tested i think
 */
#pragma once

const int mx = 2000;
int seen[mx], tim;
unordered_map<int, int> adj[mx];

int flow(int s, int t) {
    auto dfs = [&] (auto &&self, int u) {
        if (u == t) return 1;
        if (exchange(seen[u], tim) == tim) return 0;
        for (auto &[v, c] : adj[u]) 
            if (c && self(self, v)) return --adj[u][v], ++adj[v][u];
        return 0;
    }; 
    int flow = 0;
    while (tim++, dfs(dfs, s)) flow++;
    return flow;
}