/**
 * Author: Benq
 * Description: Minkowski sum of two convex polygons given in CCW order.
 * Uses Point's default (lexicographic) operator< for min\_element.
 * Time: O(N)
 * Source: https://cp-algorithms.com/geometry/minkowski.html#implementation
 * Status: stress-tested
 */
#pragma once

#include "Point.h"
using P = Point<ll>; using vP = vt<P>;
#include "ConvexHull.h"

vP minkowski_sum(vP a, vP b) {
    if (size(a) > size(b)) swap(a, b);
    if (!size(a)) return {};
    if (size(a) == 1) {
        for (auto &t : b) t = t + a[0];
        return b;
    }
    rotate(begin(a), min_element(all(a)), end(a));
    rotate(begin(b), min_element(all(b)), end(b));
    a.pb(a[0]), a.pb(a[1]);
    b.pb(b[0]), b.pb(b[1]);
    vP result;
    int i = 0, j = 0;
    while (i < size(a) - 2 || j < size(b) - 2) {
        result.pb(a[i] + b[j]);
        ll crs = (a[i + 1] - a[i]).cross(b[j + 1] - b[j]);
        i += (crs >= 0);
        j += (crs <= 0);
    }
    return result;
}

ll diameter2(vP p) { // example application: squared diameter
    vP a = convex_hull(p);
    vP b = a; for (auto &t : b) t = t * -1;
    vP c = minkowski_sum(a, b);
    ll ret = 0; for (auto &t : c) ret = max(ret, t.dist2());
    return ret;
}
