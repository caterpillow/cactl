/**
 * Author: caterpillow
 * Date: 2026-08-27
 * License: CC0
 * Description: n-point Gauss--Legendre quadrature on $[a, b]$: exact for
 * polynomials of degree $< 2n$, converges very fast for smooth $f$.
 * Nodes are found by Newton on $P_n$, so keep $n \le 60$ or so.
 * Usage: gauss(0, 1, [](db x) { return x * x; }, 20)
 * Time: O(n^2) setup, n evaluations
 * Status: stress-tested
 */
#pragma once

template<class F>
db gauss(db a, db b, F f, int n = 20) {
	db res = 0;
	F0R (i, n) {
		db x = cos(M_PI * (i + .75) / (n + .5)), dp = 1;
		F0R (it, 100) { // Newton on P_n(x)
			db p0 = 1, p1 = x;
			FOR (k, 2, n + 1) tie(p0, p1) =
				pair{p1, ((2 * k - 1) * x * p1 - (k - 1) * p0) / k};
			dp = n * (x * p1 - p0) / (x * x - 1);
			db dx = p1 / dp; x -= dx;
			if (abs(dx) < 1e-15) break;
		}
		db w = 2 / ((1 - x * x) * dp * dp);
		res += w * f((a + b) / 2 + (b - a) / 2 * x);
	}
	return res * (b - a) / 2;
}
