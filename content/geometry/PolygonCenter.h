/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-08
 * License: CC0
 * Source:
 * Description: Area centroid of a polygon (needs nonzero area):
 * $C = \frac{1}{6A}\sum (p_i + p_{i+1})(p_i \times p_{i+1})$, $A$ the signed area.
 * Time: O(n)
 * Status: Tested
 */
#pragma once

#include "Point.h"

using P = Point<db>;
P polygon_center(const vt<P>& v) {
    P res{}; db a = 0;
    for (int i = 0, j = size(v) - 1; i < size(v); j = i++) {
        res = res + (v[i] + v[j]) * v[j].cross(v[i]);
        a += v[j].cross(v[i]);
    }
    return res / a / 3;
}
