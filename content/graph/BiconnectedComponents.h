/**
 * Author: Simon Lindholm
 * Date: 2017-04-17
 * License: CC0
 * Source: folklore
 * Description: Finds all biconnected components in an undirected graph.
 *  In a biconnected component there are at least two distinct paths between any two nodes (a cycle exists through them). 
 *  Note that a node can be in several components. 
 *  An edge which is not in a component is a bridge, i.e., not part of any cycle.
 *  Note that degree 0 nodes are not considered components. 
 * Time: O(E + V)
 * Status: tested on yosupo
 */
#pragma once

struct BCC { 
    int t; 
    vt<vt<pi>> adj;
    vt<vi> comps; // lists of edges of bcc
    vi tin, stk, is_bridge; // stk for bcc only
    // vi is_art;
 
    void init(int n, vt<pi> &edges) {
        int m = size(edges);
        adj.resize(n);
        F0R (i, m) {
            auto [u, v] = edges[i];
            adj[u].pb({v, i});
            adj[v].pb({u, i});
        }
        t = 0;
        tin.resize(n);
        // is_art.resize(n);
        is_bridge.resize(m);
        F0R (u, n) if (!tin[u]) dfs(u, -1); 
        // if we include bridges as 2-node bcc
        // F0R (i, m) if (is_bridge[i]) comps.pb({i});
    }
 
    int dfs(int u, int par) {
        int me = tin[u] = ++t, dp = me;
        // int cnt = (par != -1); // art
        for (auto [v, ei] : adj[u]) if (ei != par) {
            if (tin[v]) {
                dp = min(dp, tin[v]);
                if (tin[v] < me) stk.pb(ei); // bcc
            } else {
                int si = size(stk), up = dfs(v, ei);
                dp = min(dp, up);
                // cnt += up >= me; // art
                if (up == me) { // bcc 
                    stk.pb(ei);
                    comps.pb({si + all(stk)});
                    stk.resize(si);
                } else if (up < me) stk.push_back(ei); // bcc
                if (up > me) is_bridge[ei] = 1; // bridges
            }
        }
        // is_art[u] = cnt >= 2; // art
        return dp;
    }
};