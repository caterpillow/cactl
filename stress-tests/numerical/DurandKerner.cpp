// Tests DurandKerner.h: random real-rooted polynomials (well-separated
// roots to 1e-10, clustered ones to 1e-5), complex roots, and a double
// root (1e-4). Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/numerical/DurandKerner.h"
mt19937 rng(4);
int main() {
	db worst_sep = 0, worst_clu = 0;
	F0R (it, 2000) {
		int n = rng() % 8 + 1; vt<db> r(n);
		for (db& x : r) x = ((int) (rng() % 2001) - 1000) / 250.0;
		db sep = 1e9; F0R (i, n) F0R (j, i) sep = min(sep, abs(r[i] - r[j]));
		vt<db> a{1}; for (db x : r) { vt<db> b(size(a) + 1); F0R (j, size(a)) b[j] -= x * a[j], b[j + 1] += a[j]; a = b; }
		auto z = dk_roots(a);
		for (db x : r) {
			db best = 1e9; for (auto c : z) best = min(best, abs(c - x));
			(sep >= 0.5 ? worst_sep : worst_clu) = max(sep >= 0.5 ? worst_sep : worst_clu, best);
			assert(best < (sep >= 0.5 ? 1e-10 : 1e-5) * (1 + abs(x)));
		}
	}
	printf("worst error: separated %.1e, clustered %.1e\n", worst_sep, worst_clu);
	auto z = dk_roots({-2, 1, -2, 1}); // (x^2 + 1)(x - 2)
	for (auto c : z) assert(abs(c - complex<db>(2, 0)) < 1e-9 || abs(abs(imag(c)) - 1) < 1e-9);
	z = dk_roots({3, -5, 1, 1}); // (x - 1)^2 (x + 3)
	for (auto c : z) assert(abs(c - complex<db>(1, 0)) < 1e-4 || abs(c - complex<db>(-3, 0)) < 1e-9);
	puts("Tests passed!");
}
