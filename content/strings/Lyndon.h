/**
 * Author: Claude
 * Date: 2026-10-06
 * License: CC0
 * Source: Duval's algorithm (cp-algorithms.com)
 * Description: Lyndon factorization: the unique split $s = w_1 w_2
 * \cdots w_k$ with $w_1 \ge \dots \ge w_k$, each $w_i$ smaller than all
 * its proper suffixes. Returns the start index of every factor.
 * Time: O(N)
 * Status: stress-tested
 */
#pragma once

vi duval(const string& s) {
    int n = size(s); vi st;
    for (int i = 0; i < n;) {
        int j = i + 1, k = i;
        while (j < n && s[k] <= s[j])
            k = s[k] < s[j] ? i : k + 1, j++;
        while (i <= k) st.pb(i), i += j - k;
    }
    return st;
}
