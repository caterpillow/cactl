/**
 * Author: caterpillow
 * Date: 2026-08-27
 * License: CC0
 * Description: Durand--Kerner: all $n$ complex roots of
 * $a_0 + a_1 x + \dots + a_n x^n$ at once ($a_n \ne 0$). Quadratic
 * convergence for simple roots, only linear (\tilde{}1e-5) for multiple
 * ones (a Newton polish is included). Scale the coefficients so roots
 * are $O(1)$. Real roots: \texttt{abs(imag) < eps}.
 * Usage: dk_roots({-2, 0, 1}) // x^2 - 2
 * Time: O(n^2) per iteration, typically 50--500 iterations
 * Status: stress-tested
 */
#pragma once

vt<complex<db>> dk_roots(vt<db> a, int iters = 500) {
	int n = size(a) - 1;
	vt<complex<db>> z(n);
	F0R (i, n)
		z[i] = polar(1 + .2 * i / n, 2 * M_PI * i / n + .4);
	F0R (it, iters) {
		db mv = 0;
		F0R (i, n) {
			complex<db> v = a[n], d = 1;
			ROF (k, 0, n) v = v * z[i] + a[k];
			F0R (j, n) if (j != i) d *= z[i] - z[j];
			v /= a[n] * d, z[i] -= v, mv = max(mv, abs(v));
		}
		if (mv < 1e-14) break;
	}
	F0R (i, n) F0R (t, 3) { // Newton polish
		complex<db> v = 0, d = 0;
		ROF (k, 0, n + 1) d = d * z[i] + v, v = v * z[i] + a[k];
		if (abs(d) > 0) z[i] -= v / d;
	}
	return z;
}
