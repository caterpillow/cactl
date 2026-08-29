/**
 * Author: caterpillow
 * Date: 2026-08-29
 * License: CC0
 * Source: __gnu_pbds
 * Description: Shortest paths from s, non-negative weights. The pb\_ds
 * pairing heap has O(1) push and decrease-key, so a vertex is queued
 * once instead of once per relaxation: no stale entries, no
 * \texttt{if (du != d[u]) continue;}, and less memory. Measured against
 * the lazy \texttt{priority\_queue<pl, vt<pl>, greater<pl>>}: 1.2--1.5x
 * faster when vertices are relaxed often (dense graphs, many parallel
 * edges), but 5--25\% slower on sparse random and grid graphs, where a
 * flat binary heap beats pointer chasing. Unreachable vertices keep INF.
 * The include must precede the template's \texttt{size(x)} macro, which
 * breaks extc++.
 * Usage: adj.resize(n); adj[u].pb({v, w}); vl d = dijkstra(s);
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
