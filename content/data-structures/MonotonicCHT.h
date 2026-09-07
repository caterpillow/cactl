/**
 * Author: caterpillow
 * Date: 2026-09-07
 * License: CC0
 * Description: Convex hull trick when slopes are added non-decreasing and
 * queries come at non-decreasing $x$, both amortized $O(1)$ (LineContainer
 * is the general case). A line $(m, c)$ is a point and a query is the dot
 * with $(x, 1)$. \texttt{h[i]} overtakes \texttt{h[i-1]} from direction
 * \texttt{e[i]} on, so a new line pops the tail while it would overtake
 * it no later than the tail overtook its own predecessor. Equal slopes
 * give a horizontal \texttt{e}, first or last in the radial order, i.e.
 * never or always: no special case. Returns the max; for the min negate
 * $m$, $c$ and the result. \texttt{lt} crosses two \texttt{e}'s, which is
 * $\Delta m \cdot \Delta c$: if that can exceed $9 \cdot 10^{18}$ use
 * \texttt{Point<\_\_int128>}.
 * Time: O(1) amortized
 * Status: stress-tested
 */
#pragma once

#include "../geometry/Point.h"

using P = Point<ll>; // Point<__int128> if dm * dc can overflow
struct MonotonicCHT {
    vt<P> h, e; int p = 0; // h[i] wins from direction e[i] on
    static bool lt(P a, P b) { // radial: angle in [0, 360)
        if (a.half() != b.half()) return a.half() < b.half();
        return a.cross(b) > 0;
    }
    P dir(P l) { // where l overtakes the tail: -inf if no tail
        return size(h) ? (l - h.back()).perp() : P{-1, 0};
    }
    void add(ll m, ll c) {
        P l{m, c};
        while (size(h) && !lt(dir(l), e.back()))
            h.pop_back(), e.pop_back();
        e.pb(dir(l)), h.pb(l), p = min(p, size(h) - 1);
    }
    ll query(ll x) {
        assert(size(h)); P d{x, 1};
        while (p + 1 < size(h) && h[p+1].dot(d) > h[p].dot(d))
            p++;
        return h[p].dot(d);
    }
};
