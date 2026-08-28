/**
 * Author: idk
 * Date: 2025-09-13
 * License: CC0
 * Source: cp-geo
 * Description: Accumulates non-negative floating point numbers with better
 * precision than a running sum: a binary counter of partial sums, so only
 * numbers of similar magnitude are ever added together.
 * Time: O(\log N) amortised per +=
 * Status: stress-tested
 */

#pragma once

struct StableSum {
    int cnt = 0;
    vt<db> v, pref{0};
    void operator+=(db a) {
        assert(a >= 0);
        int s = ++cnt;
        while (s % 2 == 0) {
            a += v.back();
            v.pop_back(), pref.pop_back();
            s /= 2;
        }
        v.pb(a);
        pref.pb(pref.back() + a);
    }
    db val() { return pref.back(); }
};