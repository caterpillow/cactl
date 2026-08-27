/**
 * Author: Simon Lindholm
 * Date: 2021-01-09
 * License: CC0
 * Source: https://en.wikipedia.org/wiki/Stoer%E2%80%93Wagner_algorithm
 * Description: Find a global minimum cut in an undirected graph, as
 * represented by an adjacency matrix. Weights are ll; keep the total
 * graph weight below \tilde{}1e17.
 * Time: O(V^3)
 * Status: Stress-tested together with GomoryHu
 */
#pragma once

pair<ll, vi> globalMinCut(vt<vl> mat) {
    pair<ll, vi> best = {INF, {}};
    int n = size(mat);
    vt<vi> co(n);
    F0R (i, n) co[i] = {i};
    FOR (ph, 1, n) {
        vl w = mat[0];
        size_t s = 0, t = 0;
        F0R (it, n - ph) { // O(V^2) -> O(E log V) with prio. queue
            w[t] = -INF;
            s = t, t = max_element(all(w)) - w.begin();
            F0R (i, n) w[i] += mat[t][i];
        }
        best = min(best, {w[t] - mat[t][t], co[t]});
        co[s].insert(co[s].end(), all(co[t]));
        F0R (i, n) mat[s][i] += mat[t][i];
        F0R (i, n) mat[i][s] = mat[s][i];
        mat[0][t] = -INF;
    }
    return best;
}
