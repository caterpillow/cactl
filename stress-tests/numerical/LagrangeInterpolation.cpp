// Tests LagrangeInterpolation.h: random polynomials mod p sampled at 0..n,
// evaluated at random x (incl. sample points and negatives) vs Horner;
// plus sum of k^m via interpolation. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/numerical/LagrangeInterpolation.h"
mt19937_64 rng(12);
int main() {
	F0R (it, 20000) {
		int n = rng() % 12; vl c(n + 1), y(n + 1);
		for (ll& v : c) v = rng() % mod;
		auto f = [&](ll x) { x = (x % mod + mod) % mod; ll v = 0; ROF (k, 0, n + 1) v = (v * x + c[k]) % mod; return v; };
		F0R (i, n + 1) y[i] = f(i);
		ll x = it % 3 == 0 ? (ll) (rng() % (n + 1)) : it % 3 == 1 ? -(ll) (rng() % 1000) : (ll) (rng() % mod);
		assert(lagrange(y, x) == f(x));
	}
	F0R (m, 6) { // sum_{k<=N} k^m has degree m+1: sample m+2 values
		vl y(m + 2); ll s = 0;
		F0R (i, m + 2) { ll p = 1; F0R (j, m) p = p * i % mod; s = (s + p) % mod; y[i] = s; }
		ll N = 123456789, want = 0; // brute via ModPow: sum over a small N instead
		N = 1000; want = 0; FOR (k, 0, N + 1) { ll p = 1; F0R (j, m) p = p * k % mod; want = (want + p) % mod; }
		assert(lagrange(y, N) == want);
	}
	puts("Tests passed!");
}
