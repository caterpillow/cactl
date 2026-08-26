#include "../utilities/template.h"

#define FOR(i, a, b) for (int i = a; i < (b); ++i)
#define F0R(i, b) FOR(i, 0, b)
#define vt vector
#define size(x) ((int)(x).size())
using db = double;

#include "../../content/numerical/PolyRoots.h"

void check(vt<db> got, vt<db> want) {
	assert(size(got) == size(want));
	F0R (i, size(got))
		assert(abs(got[i] - want[i]) < 1e-9);
}

int main() {
	check(poly_roots({{6, -5, 1}}, -10, 10), {2, 3});
	check(poly_roots({{-2, 1}}, 0, 1), {});
	check(poly_roots({{10100, -201, 1}}, 0, 1), {});
	check(poly_roots({{-2, 1, 0, 0}}, 0, 1), {});
	check(poly_roots({{1}}, -1, 1), {});
	check(poly_roots({{0}}, -1, 1), {});
	check(poly_roots({{0, -1, 1}}, 0, 1), {0, 1});
	check(poly_roots({{1, -2, 1}}, -2, 3), {1});
	check(poly_roots({{1, -2, 1}}, 1, 1), {1});
	check(poly_roots({{1, -2, 1}}, 0, 0), {});

	FOR (mask, 1, 1 << 9) if (__builtin_popcount(mask) <= 6) {
		Poly p{{1}};
		vt<db> want;
		F0R (i, 9) if (mask >> i & 1) {
			db r = i - 4;
			vt<db> a(size(p.a) + 1);
			F0R (j, size(p.a))
				a[j] -= r * p.a[j], a[j + 1] += p.a[j];
			p.a = a;
			want.push_back(r);
		}
		check(poly_roots(p, -5, 5), want);
	}
}
