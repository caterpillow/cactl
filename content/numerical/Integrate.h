/**
 * Author: Simon Lindholm
 * Date: 2015-02-11
 * License: CC0
 * Source: Wikipedia
 * Description: Simple integration of a function over an interval using
 *  Simpson's rule. The error should be proportional to $h^4$, although in
 *  practice you will want to verify that the result is stable to desired
 *  precision when epsilon changes.
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
