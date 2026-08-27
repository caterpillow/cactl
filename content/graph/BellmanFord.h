/**
 * Author: Simon Lindholm
 * Date: 2015-02-23
 * License: CC0
 * Source: http://en.wikipedia.org/wiki/Bellman-Ford_algorithm
 * Description: Calculates shortest paths from $s$ in a graph that might have negative edge weights.
 * Unreachable nodes get dist = INF; nodes reachable through negative-weight cycles get dist = -INF.
 * Assumes $V^2 \max |w_i| < \tilde{} 2^{63}$.
 * Time: O(VE)
 * Status: Tested on kattis:shortestpath3
 */
#pragma once

struct Ed { int a, b, w, s() { return a < b ? a : -a; }};
struct Node { ll dist = INF; int prev = -1; };

void bellmanFord(vt<Node> &nodes, vt<Ed> &eds, int s) {
    nodes[s].dist = 0;
    sort(all(eds), [] (Ed a, Ed b) { return a.s() < b.s(); });

    int lim = size(nodes) / 2 + 2; // /3 + 100 with shuffled vertices
    F0R (i, lim) for (Ed ed : eds) {
        Node cur = nodes[ed.a], &dest = nodes[ed.b];
        if (abs(cur.dist) == INF) continue;
        ll d = cur.dist + ed.w;
        if (d < dest.dist) {
            dest.prev = ed.a;
            dest.dist = (i < lim - 1 ? d : -INF);
        }
    }
    F0R (i, lim) for (Ed e : eds) {
        if (nodes[e.a].dist == -INF)
            nodes[e.b].dist = -INF;
    }
}