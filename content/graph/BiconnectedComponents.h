/**
 * Author: Simon Lindholm
 * Date: 2017-04-17
 * License: CC0
 * Source: folklore
 * Description: Finds all biconnected components in an undirected graph.
 *  In a biconnected component there are at least two distinct paths between any two nodes. 
 *  Note that a node can be in several components. 
 *  An edge which is not in a component is a bridge, i.e., not part of any cycle.
 *  Note that degree 0 nodes are not considered components. 
 * Time: O(E + V)
 * Status: tested on yosupo
 */
#pragma once

struct BCC { 
    int n, m, t; 
    vt<vt<pair<int, int>>> adj;
	vector<pair<int, int>> edges;
    vt<vt<int>> comps; // lists of edges of bcc
    vt<int> tin, stk, is_art, is_bridge;
 
    void init(int _n, vt<pair<int, int>> &_edges) {
        n = _n;
		edges = _edges;
		m = size(edges);
        adj.resize(n);
		F0R (i, m) {
			auto [u, v] = edges[i];
			adj[u].pb({v, i});
			adj[v].pb({u, i});
		}
		t = 0;
        tin = is_art = vt<int>(n);
		is_bridge.resize(m);
        F0R (u, n) if (!tin[u]) dfs(u, -1); 
		// if we include bridges as 2-node bcc
		F0R (i, m) if (is_bridge[i]) comps.pb({i});
    }
 
    int dfs(int u, int par) {
        int me = tin[u] = ++t, dp = me;
        for (auto [v, ei] : adj[u]) if (ei != par) {
            if (tin[v]) {
                dp = min(dp, tin[v]);
                if (tin[v] < me)
                    stk.push_back(ei);
            } else {
                int si = size(stk), up = dfs(v, ei);
                dp = min(dp, up);
                if (up == me) {
					is_art[u] = 1;
                    stk.pb(ei);
                    comps.pb({si + all(stk)});
                    stk.resize(si);
                } else if (up < me) stk.push_back(ei);
                else { is_bridge[ei] = 1; }
            }
        }
        return dp;
    }
};