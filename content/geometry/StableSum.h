/**
 * Author: idk
 * Date: 2025-09-13
 * License: CC0
 * Source: cp-geo
 * Description: Accumulates positive floating point numbers with better precision.
 * Status: probably good right
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
    double val() { return pref.back(); }
};