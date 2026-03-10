/**
 * Author: caterpillow
 * Date: 2016-8-24
 * License: CC0
 * Source: me
 * Description: Generic sparse table for idempotent operations.
 * Time: O(N \log N) build, O(1) query.
 * Usage: Define the desired operation
 * Status: migrated from code-library - untested
 */
#pragma once

template<class T> struct RMQ {
    vt<vt<T>> dp;
    T query(int l, int r) {
        int d = max(0, __lg(r - l - 1));
        return min(dp[d][l], dp[d][r - (1 << d)]); 
    }
    void init(const vt<T>& v) {
        dp = {v};
        for (int w = 2; w <= size(v); w += w) {
            dp.emplace_back(size(v) - w + 1);
            for (int i = 0; i + w <= size(v); i++) 
                dp.back()[i] = query(i, i + w);
        }
    }
};