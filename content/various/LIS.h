/**
 * Author: Johan Sannemo
 * License: CC0
 * Description: \texttt{lis\_dp} gives the strictly increasing LIS length ending at each index, \texttt{lis\_construct} the indices of one LIS.
 * Time: $O(N \log N)$
 * Status: Tested on kattis:longincsubseq, stress-tested
 */
#pragma once

vi lis_dp(const vi &a) {
    int n = size(a);
    vi ans(n), dp(n, INT_MAX);
    F0R (i, n) {
        int j = lower_bound(all(dp), a[i]) - dp.begin(); // strict
        dp[j] = a[i], ans[i] = j + 1;
    }
    return ans;
}

vi lis_construct(const vi &a) {
    if (a.empty()) return {};
    int n = size(a);
    vi prev(n);
    vt<pi> res;
    F0R (i, n) {
		// change 0 -> i for longest non-decreasing subsequence
        auto it = lower_bound(all(res), pi{a[i], 0});
        if (it == res.end()) res.emplace_back(), it = res.end() - 1;
        *it = {a[i], i};
        prev[i] = it == res.begin() ? 0 : (it - 1)->second;
    }
    int m = size(res), cur = res.back().s;
    vi ans(m);
    while (m--) ans[m] = cur, cur = prev[cur];
    return ans;
}
