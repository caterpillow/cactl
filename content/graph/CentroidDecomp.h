/**
 * Author: caterpillow
 * Date: 2025-09-13
 * License: CC0
 * Source: some cf blog
 * Description: Give order to call dfs on.
 * Time: O(N)
 * Status: true
 */

#pragma once

vector<pair<int, int>> ord;
auto dfs1 = [&] (auto &&self, int u, int p) -> int {
    int msk1 = 0, msk2 = 0;
    for (auto [v, w] : adj[u]) if (v - p) {
        int res = self(self, v, u);
        msk2 |= msk1 & res;
        msk1 |= res;
    }
    int res = (msk1 | ((1 << __lg(2 * msk2 + 1)) - 1)) + 1;
    pri[u] = __builtin_ctz(res);
    ord.push_back({-__builtin_ctz(res), u});
    return res;
};
dfs1(dfs1, 0, -1);
sort(all(ord));