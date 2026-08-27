/**
 * Author: caterpillow
 * Date: 2026-08-27
 * License: CC0
 * Description: Safeguarded Newton: a root of $f$ in $[lo, hi]$ where
 * $f(lo)$, $f(hi)$ have opposite signs. Takes the Newton step when it
 * stays inside the bracket and bisects otherwise, so it cannot diverge;
 * quadratic convergence near simple roots (\tilde{}6 iterations), linear
 * at multiple roots. To minimise a convex $g$, pass $g'$ and $g''$.
 * Usage: newton(0, 2, [](db x) { return x*x - 2; },
 *               [](db x) { return 2*x; })
 * Status: stress-tested
 */
#pragma once

template<class F, class D>
db newton(db lo, db hi, F f, D df) {
	db x = (lo + hi) / 2, fl = f(lo);
	F0R (it, 200) {
		db fx = f(x);
		if ((fx < 0) == (fl < 0)) lo = x, fl = fx; else hi = x;
		db nx = x - fx / df(x);
		if (!(lo < nx && nx < hi)) nx = (lo + hi) / 2; // bisect
		if (abs(nx - x) <= 1e-13 * (1 + abs(x))) return nx;
		x = nx;
	}
	return x;
}
