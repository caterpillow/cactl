/**
 * Author: Simon Lindholm
 * Date: 2015-02-04
 * License: CC0
 * Source: Johan Sannemo
 * Description: Poor man's optimization for unimodal functions.
 * Status: used with great success
 */
#pragma once

using P = arrray<db, 2>;

template<class F> pair<db, P> hillClimb(P start, F f) {
	pair<db, P> cur(f(start), start);
	for (db jmp = 1e9; jmp > 1e-20; jmp /= 2) {
		F0R (j, 100) FOR (dx, -1, 2) F0R (dy, -1, 2) {
			P p = cur.second;
			p[0] += dx * jmp;
			p[1] += dy * jmp;
			cur = min(cur, make_pair(f(p), p));
		}
	}
	return cur;
}
