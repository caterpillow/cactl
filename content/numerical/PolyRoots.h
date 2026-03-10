/**
 * Author: Per Austrin
 * Date: 2004-02-08
 * License: CC0
 * Description: Finds the real roots to a polynomial.
 * Usage: polyRoots({{2,-3,1}},-1e9,1e9) // solve x^2-3x+2 = 0
 * Time: O(n^2 \log(1/\epsilon))
 */
#pragma once

#include "Polynomial.h"

vt<db> poly_roots(Poly p, double xmin, double xmax) {
	if (size(p.a) == 2) { return {-p.a[0] / p.a[1]}; }
	vt<db> ret;
	Poly der = p;
	der.diff();
	auto dr = poly_roots(der, xmin, xmax);
	dr.push_back(xmin - 1);
	dr.push_back(xmax + 1);
	sort(all(dr));
	F0R (i, size(dr) - 1) {
		db l = dr[i], h = dr[i + 1];
		bool sign = p(l) > 0;
		if (sign ^ (p(h) > 0)) {
			F0R (it, 60) { // while (h - l > 1e-8)
				double m = (l + h) / 2, f = p(m);
				if ((f <= 0) ^ sign) l = m;
				else h = m;
			}
			ret.push_back((l + h) / 2);
		}
	}
	return ret;
}
