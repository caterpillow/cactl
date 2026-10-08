/**
 * Author: Mattias de Zalenski
 * Date: 2002-11-04
 * Description: Magic formula for the volume of a polyhedron. Faces should point outwards.
 * Usage: vt<array<int, 3>> t; // indices into A
 *  for (F x : hull3d(A)) t.pb({x.a, x.b, x.c});
 *  db vol = signedPolyVolume(A, t);
 * Status: tested
 */
#pragma once

template<class P>
db signedPolyVolume(const P &pts, const vt<array<int, 3>> &trilist) {
    db v = 0;
    for (auto [a, b, c] : trilist) v += pts[a].cross(pts[b]).dot(pts[c]);
    return v / 6;
}
