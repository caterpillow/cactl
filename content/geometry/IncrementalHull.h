/**
 * Author: OpenAI
 * Date: 2026-09-24
 * Description: Convex hull under point insertions in arbitrary order.
 *  Returns minimal hull vertices CCW from the lexicographically smallest point,
 *  without repeating the first point. Handles empty input, duplicates and
 *  collinearity; an entirely collinear hull has at most two endpoints.
 *  T must be signed and wide enough for cross products; requires reliable signs.
 * Usage: IncrementalHull<ll> h; h.add({x, y}); auto ps = h.hull();
 * Time: Amortized O(\log n) per insertion; O(h) to report h hull vertices.
 * Memory: O(h)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

template<class T> struct IncrementalHull {
    using P = Point<T>;
    set<P> lo, hi;
    void insert(set<P>& chain, P p, bool upper) {
        auto [it, added] = chain.insert(p);
        if (!added) return;
        auto bad = [&](auto v) {
            if (v == chain.begin() || next(v) == chain.end()) return false;
            auto turn = prev(v)->cross(*v, *next(v));
            return upper ? turn >= 0 : turn <= 0;
        };
        if (bad(it)) { chain.erase(it); return; }
        while (it != chain.begin() && bad(prev(it))) chain.erase(prev(it));
        while (next(it) != chain.end() && bad(next(it))) chain.erase(next(it));
    }
    void add(P p) { insert(lo, p, false); insert(hi, p, true); }
    vt<P> hull() const {
        if (size(lo) <= 1) return {all(lo)};
        vt<P> ans(lo.begin(), prev(lo.end()));
        ans.insert(ans.end(), hi.rbegin(), prev(hi.rend()));
        return ans;
    }
};
