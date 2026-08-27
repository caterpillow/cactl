// Tests MinPlusConvolution.h: min_smawk row minima on totally monotone
// matrices, min_plus_smawk (convex b) and min_plus_concave_one (concave b)
// vs O(NM) brute force. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/numerical/MinPlusConvolution.h"
mt19937 rng(5);
vl shape(int m, bool convex) { // convex: increasing slopes, concave: decreasing
	vl b(m); ll v = rng() % 100 - 50, slope = convex ? -(ll) (rng() % 20) : rng() % 20;
	F0R (i, m) { b[i] = v; v += slope; slope += convex ? rng() % 4 : -(ll) (rng() % 4); }
	return b;
}
vl brute(const vl& a, const vl& b) {
	vl c(size(a) + size(b) - 1, INF);
	F0R (i, size(a)) F0R (j, size(b)) c[i + j] = min(c[i + j], a[i] + b[j]);
	return c;
}
int main() {
	F0R (it, 20000) {
		int n = rng() % 8 + 1, m = rng() % 8 + 1;
		vl a(n); for (ll& x : a) x = rng() % 200 - 100;
		vl bc = shape(m, 1), bv = shape(m, 0);
		assert(min_plus_smawk(a, bc) == brute(a, bc));
		assert(min_plus_concave_one(a, bv) == brute(a, bv));
		// row minima of f(r, c) = a[c] + b[r - c] style totally monotone matrix
		int R = rng() % 8 + 1, C = rng() % 8 + 1; vl col = shape(C, 1);
		auto f = [&](int r, int c) { return col[c] + (ll) (r - c) * (r - c); };
		vi got = min_smawk(f, R, C);
		F0R (r, R) { ll best = INF; F0R (c, C) best = min(best, f(r, c)); assert(f(r, got[r]) == best); }
	}
	puts("Tests passed!");
}
