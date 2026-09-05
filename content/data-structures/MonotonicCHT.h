/**
 * Author: caterpillow
 * Date: 2026-09-06
 * License: CC0
 * Description: Convex hull trick when slopes are added non-decreasing and
 * queries come at non-decreasing $x$: both are amortized $O(1)$, no
 * \texttt{multiset} (cf. LineContainer). Returns $\max_i m_ix+c_i$; for
 * min, negate m and c on both add and the result. \texttt{bad} is the
 * cross product of the line-points $(m,c)$: the middle line is below the
 * hull of its neighbours. \texttt{query} needs a non-empty hull.
 * Time: O(1) amortized per add/query
 * Status: stress-tested
 */
#pragma once

struct MonotonicCHT {
    struct L { ll m, c; ll operator()(ll x) { return m*x+c; } };
    vt<L> h; int p = 0;
    typedef __int128 L2;
    bool bad(L a, L b, L c) { // b redundant given a, c
        return L2(b.m - a.m) * (c.c - a.c)
            >= L2(c.m - a.m) * (b.c - a.c);
    }
    void add(ll m, ll c) {
        L l{m, c};
        if (size(h) && h.back().m == m) { // same slope, keep hi
            if (h.back().c >= c) return;
            h.pop_back();
        }
        while (size(h) >= 2 && bad(h[size(h)-2], h.back(), l))
            h.pop_back();
        h.pb(l), p = min(p, size(h) - 1);
    }
    ll query(ll x) {
        assert(!h.empty());
        p = min(p, size(h) - 1);
        while (p + 1 < size(h) && h[p+1](x) >= h[p](x)) p++;
        return h[p](x);
    }
};
