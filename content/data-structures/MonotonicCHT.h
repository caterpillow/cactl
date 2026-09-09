/**
 * Author: caterpillow
 * Date: 2026-09-09
 * License: CC0
 * Description: Convex hull trick when slopes are added non-decreasing and
 * queries come at non-decreasing $x$, both amortized $O(1)$ (LineContainer
 * is the general case). A line $(m, c)$ is a point, a query is the dot
 * with $(x, 1)$, \texttt{h} is the upper hull in slope order and
 * \texttt{p} walks it. A line with the tail's slope replaces it if higher
 * and is dropped otherwise, so the cross test never sees equal slopes,
 * the one case it cannot decide. Returns the max; for the min negate
 * $m$, $c$ and the result. The cross is $\Delta m \cdot \Delta c$: if that
 * can exceed $9 \cdot 10^{18}$ use Point<\_\_int128>.
 * Time: O(1) amortized
 * Status: stress-tested
 */
#pragma once

#include "../geometry/Point.h"

using P = Point<ll>; // __int128 if dm * dc overflows
struct MonotonicCHT {
    vt<P> h; int p = 0;
    void add(ll m, ll c) {
        P l{m, c};
        if (size(h) && h.back().x == m && h.back().y >= c) return;
        while (size(h) && (h.back().x == m || (size(h) > 1 &&
                end(h)[-2].cross(h.back(), l) >= 0)))
            h.pop_back();
        h.pb(l), p = min(p, size(h) - 1);
    }
    ll query(ll x) {
        assert(size(h)); P d{x, 1};
        while (p + 1 < size(h) && h[p+1].dot(d) > h[p].dot(d))
            p++;
        return h[p].dot(d);
    }
};
