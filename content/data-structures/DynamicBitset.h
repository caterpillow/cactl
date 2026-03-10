/**
 * Author: caterpillow
 * Date: 2025-09-12
 * License: CC0
 * Source: codeforces
 * Description: Workaround for dynamic bitsets. Adds compile time and memory factor. Reduce multiplying factor depending on desired tradeoff.
 * Status: just works
 */
#pragma once

const int MAX_LEN = 1 << 20;
template <int len = 1>
void solve(int n) {
    if (n > len) {
        solve<min(len * 2, MAX_LEN)>(n);
        return;
    }
    // solution using bitset<len>
}