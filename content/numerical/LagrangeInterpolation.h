/**
 * Author: caterpillow
 * Date: 2026-08-28
 * License: CC0
 * Description: Given $y_i = f(i)$ for $i = 0..n$ and $\deg f \le n$,
 * evaluates $f(x) \bmod p$ in $O(n)$. Idea: $L_i(x) = \prod_{j \ne i}
 * \frac{x - j}{i - j}$ is 1 at $i$ and 0 at every other sample, so
 * $f(x) = \sum_i y_i L_i(x)$; with samples $0..n$ the denominators are
 * $\pm i!\,(n-i)!$ and the numerators are prefix/suffix products of
 * $(x - j)$. Typical use: $\sum_{k \le N} k^m$ is a polynomial of degree
 * $m+1$ in $N$, so sample $m+2$ values. General points: PolyInterpolate.
 * Time: O(n) per call; factorials are cached (O(\log p) per new index)
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

ll lagrange(vl &y, ll x) { // y[i] = f(i), i in [0, n]
	int n = size(y) - 1; x = (x % mod + mod) % mod;
	static vl fac{1}, ifac{1}; // cached across calls
	while (size(fac) <= n) {
		fac.pb(fac.back() * size(fac) % mod);
		ifac.pb(mpow(fac.back()));
	}
	vl pre(n + 2, 1), suf(n + 2, 1);
	F0R (i, n + 1)
		pre[i + 1] = pre[i] * ((x - i + mod) % mod) % mod;
	ROF (i, 0, n + 1)
		suf[i] = suf[i + 1] * ((x - i + mod) % mod) % mod;
	ll res = 0;
	F0R (i, n + 1) {
		ll t = y[i] % mod * pre[i] % mod * suf[i + 1] % mod
			* ifac[i] % mod * ifac[n - i] % mod;
		res = (res + ((n - i) & 1 ? mod - t : t)) % mod;
	}
	return res;
}
