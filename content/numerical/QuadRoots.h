/**
 * Author: idk
 * Date: 2025-10-23
 * License: CC0
 * Source: cp-geo
 * Description: Calculates the roots of a quadratic equation with better precision. Returns the number of roots.
 * Time: $O(1)$
 * Status: asdf
 */
#pragma once

int quadRoots(db a, db b, db c, pair<db, db> &out) {
    assert(a != 0);
    db disc = b * b - 4 * a * c;
    if (disc < 0) return 0;
    db sum = (b >= 0) ? -b - sqrt(disc) : -b + sqrt(disc);
    out = {sum / (2 * a), sum == 0 ? 0 : (2 * c) / sum};
    return 1 + (disc > 0);
}