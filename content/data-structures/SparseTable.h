/**
 * Author: caterpillow
 * Date: 2016-8-24
 * License: CC0
 * Source: me
 * Description: Generic sparse table for idempotent operations.
 * Time: O(N \log N) build, O(1) query.
 * Usage: Define the desired operation
 * Status: stress-tested
 */
#pragma once

template<class T> struct RMQ {
    #define func min
    vt<vt<T>> dp;
    void init(const vt<T>& v) {
        assert(size(v));
        dp.resize(__lg(size(v)) + 1, vt<T>(size(v)));
        copy(all(v), begin(dp[0]));
        for (int j = 1; 1 << j <= size(v); ++j) {
            for (int i = 0; i <= size(v) - (1 << j); i++)
                dp[j][i] = func(dp[j - 1][i],
                    dp[j - 1][i + (1 << (j - 1))]);
        }
    }
    T query(int l, int r) { // [l, r), r > l
        int d = __lg(r - l);
        return func(dp[d][l], dp[d][r - (1 << d)]);
    }
    #undef func
};