/**
 * Author: Simon Lindholm
 * Date: 2015-02-11
 * License: CC0
 * Source: Wikipedia
 * Description: Fast integration using an adaptive Simpson's rule.
 *  eps is the absolute error budget for the whole interval (halves get
 *  eps/2); recursion also stops below width 1e-10, so rescale tiny
 *  domains. Split at kinks and discontinuities; for endpoint
 *  singularities substitute $x = a + t^2$ ($dx = 2t\,dt$), for infinite
 *  ranges $x = \tan t$ or $x = t/(1-t)$. Nested quads cost eps$^{-d}$.
 * Usage: quad(a, b, f, eps = 1e-8) // e.g.
	db sphereVolume = quad(-1, 1, [](db x) {
	return quad(-1, 1, [\&](db y) {
	return quad(-1, 1, [\&](db z) {
	return x*x + y*y + z*z < 1; });});});
 * Status: mostly untested
 */
#pragma once

using d = db;
#define S(a,b) (f(a) + 4*f((a+b) / 2) + f(b)) * (b-a) / 6

template <class F>
d rec(F& f, d a, d b, d eps, d S) {
    d c = (a + b) / 2;
    d S1 = S(a, c), S2 = S(c, b), T = S1 + S2;
    if (abs(T - S) <= 15 * eps || b - a < 1e-10)
        return T + (T - S) / 15;
    return rec(f, a, c, eps / 2, S1) + rec(f, c, b, eps / 2, S2);
}
template<class F>
d quad(d a, d b, F f, d eps = 1e-8) {
    return rec(f, a, b, eps, S(a, b));
}
