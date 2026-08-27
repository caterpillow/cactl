/**
 * Author: Simon Lindholm
 * Date: 2016-12-15
 * License: CC0
 * Source: http://en.wikipedia.org/wiki/Floyd–Warshall_algorithm
 * Description: Calculates all-pairs shortest path in a directed graph that might have negative edge weights.
 * Input is an distance matrix $m$, where $m[i][j] = \texttt{INF}$ if $i$ and $j$ are not adjacent.
 * As output, $m[i][j]$ is set to the shortest distance between $i$ and $j$, \texttt{INF} if no path,
 * or \texttt{-INF} if the path goes through a negative-weight cycle.
 * Time: O(N^3)
 * Status: slightly tested
 */
#pragma once

void floydWarshall(vt<vt<ll>>& m) {
    int n = size(m);
    F0R (i, n) m[i][i] = min(m[i][i], 0LL);
    F0R (k, n) F0R (i, n) F0R (j, n)
        if (m[i][k] != INF && m[k][j] != INF)
            m[i][j] = min(m[i][j], max(m[i][k] + m[k][j], -INF));
    F0R (k, n) if (m[k][k] < 0) F0R (i, n) F0R (j, n)
        if (m[i][k] != INF && m[k][j] != INF) m[i][j] = -INF;
}
