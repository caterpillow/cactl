/**
 * Author: caterpillow
 * Date: 2026-08-29
 * License: CC0
 * Source: __gnu_pbds
 * Description: Shortest paths from s, non-negative weights. Decrease-key
 * keeps exactly one entry per vertex. Measured against a regular
 * \texttt{priority\_queue<pl, vt<pl>, greater<>>} that pushes on every
 * relaxation and skips stale entries when popped: 1.2--1.4x faster on
 * dense graphs and when a vertex is relaxed often, up to 1.4x slower on
 * sparse random and grid graphs. Against pushing every neighbour and
 * skipping a vertex only once it has been popped: 1.3--2x faster, and
 * 50--70x on a dense graph, where that version pushes every edge.
 * Unreachable vertices keep INF. The include must precede the template's
 * \texttt{size(x)} macro, which breaks extc++.
 * Time: O(E + V \log V)
 * Status: stress-tested
 */
#pragma once

#include <bits/extc++.h> /** keep-include */

using pq_t = __gnu_pbds::priority_queue<pl, greater<pl>>;
vt<vt<pl>> adj; // adj[u] = {{v, w}, ...}

vl dijkstra(int s) {
    vl d(size(adj), INF);
    vt<pq_t::point_iterator> it(size(adj));
    pq_t pq;
    d[s] = 0, it[s] = pq.push({0, s});
    while (!pq.empty()) {
        ll u = pq.top().s; pq.pop();
        // a popped u is final, so v below is still queued
        for (auto [v, w] : adj[u]) if (d[u] + w < d[v]) {
            bool queued = d[v] < INF;
            d[v] = d[u] + w;
            if (queued) pq.modify(it[v], {d[v], v});
            else it[v] = pq.push({d[v], v});
        }
    }
    return d;
}
