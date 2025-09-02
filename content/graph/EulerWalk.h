/**
 * Author: caterpillow
 * Date: 2025-09-01
 * License: CC0
 * Source: idk
 * Description: Eulerian undirected/directed path/cycle algorithm.
 * For edges, push the edge u came from instead of current node.
 * First, the graph (after removing directivity) must be connected.
 * For undirected graphs, a tour exists when all nodes have even degree.
 * For directed graphs, a tour exists when all nodes have equal in and out degree.
 * For trails, the condition is the same as if you added an edge from t -> s.
 * Time: O(V + E)
 * Status: no way this doesn't work
 */
#pragma once

int n, m;
vt<vt<pair<int, int>>> adj;
vt<int> ret, used;

// 
void dfs(int u) {
    while (adj[u].size()) {
        auto [v, ei] = adj[u].back();
        adj[u].pop_back();
        if (used[ei]++) continue;
        dfs(v);
    }
    ret.push_back(u);
}
