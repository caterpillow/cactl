/**
 * Author: Claude
 * Date: 2026-10-06
 * License: CC0
 * Source: cp-algorithms.com (Half-plane intersection)
 * Description: Intersection of half-planes. \texttt{Hp(a, b)} is the
 * set of points strictly left of the line $a \to b$. Returns the
 * polygon in CCW order, empty if the area is 0. Add 4 half-planes of
 * a big box first: the result must be bounded.
 * Usage: vt<Hp> h = {Hp(P{-B, -B}, P{B, -B}), ...};
 *  vt<P> r = hpi(h);
 * Time: O(N \log N)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

using P = Point<db>;
struct Hp {
    P p, d; db a; // d = direction, a = its angle
    Hp(P s, P e) : p(s), d(e - s), a(atan2(d.y, d.x)) {}
    bool out(P q) const { return d.cross(q - p) < -1e-9; }
    P inter(const Hp& o) const { // not parallel
        return p + d * (o.d.cross(o.p - p) / o.d.cross(d));
    }
};
vt<P> hpi(vt<Hp> h) {
    sort(all(h), [](Hp& a, Hp& b) { return a.a < b.a; });
    int n = size(h), l = 0, r = 0;
    vt<Hp> q(n, h[0]); vt<P> t(n);
    FOR (i, 1, n) {
        while (l < r && h[i].out(t[r - 1])) r--;
        while (l < r && h[i].out(t[l])) l++;
        q[++r] = h[i];
        if (fabs(q[r].d.cross(q[r - 1].d)) < 1e-12) { // parallel
            if (q[r].d.dot(q[r - 1].d) < 0) return {}; // opposite
            r--; if (h[i].out(q[r].p)) q[r] = h[i];
        }
        if (l < r) t[r - 1] = q[r - 1].inter(q[r]);
    }
    while (r - l > 1 && q[l].out(t[r - 1])) r--;
    while (r - l > 1 && q[r].out(t[l])) l++;
    if (r - l <= 1) return {};
    t[r] = q[r].inter(q[l]);
    return {t.begin() + l, t.begin() + r + 1};
}
