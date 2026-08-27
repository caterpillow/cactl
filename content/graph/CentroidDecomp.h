/**
 * Author: caterpillow
 * Date: 2025-09-13
 * License: CC0
 * Source: some cf blog
 * Description: Order to process vertices in: dfs from each, stopping
 * at already processed ones. Paste into main; needs \texttt{adj}, root 0.
 * Not true centroids, but depth $\le \log_2 N + 1$, total $O(N \log N)$.
 * Time: O(N)
 * Status: true
 */

#pragma once

vt<pi> ord;
auto dfs1 = [&] (auto &&self, int u, int p) -> int {
    int msk1 = 0, msk2 = 0;
    for (int v : adj[u]) if (v - p) {
        int res = self(self, v, u);
        msk2 |= msk1 & res;
        msk1 |= res;
    }
    int res = (msk1 | ((1 << __lg(2 * msk2 + 1)) - 1)) + 1;
    ord.pb({-__builtin_ctz(res), u});
    return res;
};
dfs1(dfs1, 0, -1);
sort(all(ord));