/**
 * Author: caterpillow, Claude
 * Date: 2026-09-13
 * License: CC0
 * Source: extrVertex in LineHullIntersection.h
 * Description: Tangents from a point $p$ strictly outside a ccw convex
 * polygon with no collinear points, coordinates up to $10^9$. Returns
 * $\{a, b\}$: the polygon lies to the right of the ray $p \to a$ and to
 * the left of $p \to b$, so the ccw walk from $a$ to $b$ is the side of
 * the hull facing $p$. A tangent is $(i, -1)$ if it touches only vertex
 * $i$, or $(i, i+1)$ (indices in hull order) if it contains side
 * $(i, i+1)$. For doubles use the eps side\_of (sideOf.h) as in the
 * comment; every vertex not on a tangent must then be further than eps
 * from it.
 * Time: O(\log n)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

// doubles: d * side_of(p, poly[(i)%n], poly[(j)%n], eps)
#define ang(i,j) sgn(d * p.cross(poly[(i)%n], poly[(j)%n]))
#define tang(i) (ang(i + 1, i) >= 0 && ang(i, i - 1 + n) < 0)
template<class P>
pair<pi, pi> hull_tangents(vt<P>& poly, P p) {
    int n = size(poly);
    auto go = [&](int d) -> pi {
        int lo = 0, hi = n;
        if (!tang(0)) while (lo + 1 < hi) {
            int m = (lo + hi) / 2;
            if (tang(m)) { lo = m; break; }
            int a = ang(lo + 1, lo), b = ang(m + 1, m);
            (a < b || (a == b && a == ang(lo, m)) ? hi : lo) = m;
        }
        return {lo, ang(lo, lo + 1) || n < 2 ? -1 : (lo + 1) % n};
    };
    return {go(1), go(-1)};
}
