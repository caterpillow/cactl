/**
 * Author: caterpillow
 * Date: 2025-10-30
 * Description: Finds all articulation points in a graph.
 * Time: O(E + V)
 * Status: tested on orac
 */
#pragma once

struct Arts : vi { 
    int t; 
    vi tin;
    Arts(int n, vt<vi> &adj) : vi(n), t(0), tin(n) {
        F0R (u, n) if (!tin[u]) dfs(u, adj, 0); 
    }
 
    int dfs(int u, vt<vi> &adj, int cnt = 1) {
        int dp = tin[u] = ++t;
        for (int v : adj[u]) {
            if (tin[v]) dp = min(dp, tin[v]);
            else {
                int up = dfs(v, adj);
                dp = min(dp, up);
                cnt += up >= tin[u]; 
            }
        }
        at(u) = cnt >= 2;
        return dp;
    }
};