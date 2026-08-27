/**
 * Author: Simon Lindholm
 * Date: 2015-02-11
 * License: CC0
 * Source: Wikipedia
 * Description: Simple integration of a function over an interval using
 *  Simpson's rule with $2n$ slices ($2n+1$ evaluations). Error is
 *  $O(h^4 f^{(4)})$, so double n until the result stops changing. Exact
 *  for cubics; useless across kinks or singularities -- split the
 *  interval there, and substitute $x = a + t^2$ near an endpoint
 *  singularity.
 * Usage: quad(a, b, f, n) // n = number of double-slices
 * Status: mostly untested
 */
#pragma once

template<class F>
db quad(db a, db b, F f, const int n = 1000) {
	db h = (b - a) / 2 / n, v = f(a) + f(b);
	FOR (i, 1, n * 2)
		v += f(a + i * h) * (i & 1 ? 4 : 2);
	return v * h / 3;
}
