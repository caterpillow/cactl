/**
 * Author: CP-Algorithms
 * Date: 2025-09-05
 * License: CC0
 * Source: https://cp-algorithms.com/graph/desopo_pape.html
 * Description: Mystery shortest path algorithm with exponential breaking cases. 
 *  Works with negative weights, but dies on negative cycles.
 * Time: Linear to exponential, depending on how good data is. 
 * Status: looks like it works
 */
#pragma once

const ll INF = 1e18;
vt<vt<pair<int, ll>>> adj;

void shortest_paths(int s, vt<ll> &d, vi &p) {
    d.resize(n, INF);
    d[s] = 0;
    vi m(n, 2);
    deque<int> q {s};
    p.resize(n, -1);

    while (!q.empty()) {
        int u = q.front(); q.pop_front();
        m[u] = 0;
        for (auto [v, w] : adj[u]) {
            if (d[v] > d[u] + w) {
                d[v] = d[u] + w;
                p[v] = u;
                if (m[v] == 2) q.pb(v);
                else if (m[v] == 0) q.push_front(v);
                m[v] = 1;
            }
        }
    }
}