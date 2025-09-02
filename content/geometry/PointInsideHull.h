/**
 * Author: chilli
 * Date: 2019-05-17
 * License: CC0
 * Source: https://github.com/ngthanhtrung23/ACM_Notebook_new
 * Description: Determine whether a point t lies inside a convex hull (CCW
 * order, with no collinear points). Returns true if point lies within
 * the hull. If strict is true, points on the boundary aren't included.
 * Usage:
 * Status: stress-tested
 * Time: O(\log N)
 */
#pragma once

#include "Point.h"
#include "sideOf.h"
#include "OnSegment.h"

using P = Point<ll>;
bool in_hull(const vector<P>& l, P p, bool strict = true) {
    int a = 1, b = size(l) - 1, r = !strict;
    if (size(l) < 3) return r && on_segment(l[0], l.back(), p);
    if (side_of(l[0], l[a], l[b]) > 0) swap(a, b);
    if (side_of(l[0], l[a], p) >= r || side_of(l[0], l[b], p) <= -r)
        return false;
    while (abs(a - b) > 1) {
        int c = (a + b) / 2;
        (side_of(l[0], l[c], p) > 0 ? b : a) = c;
    }
    return sgn(l[a].cross(l[b], p)) < r;
}