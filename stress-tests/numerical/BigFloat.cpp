// Tests BigFloat.h (qdlong long-double-double): division keeps both words
// ((1/3)*3-1 tiny), a/b*b ~= a over wide exponent range, sqrt sanity.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/numerical/BigFloat.h"

mt19937_64 rng(4242);

int main() {
	// (1/3)*3 - 1 should be ~2^-125 ~ 2e-38 or smaller
	qdlong r = (qdlong(1.0L) / qdlong(3.0L)) * qdlong(3.0L) - qdlong(1.0L);
	ldb e13 = fabsl(r.to_long_double()) + fabsl(r.lo) * 0; // hi+lo
	assert(fabsl(r.hi + r.lo) < 1e-37L);

	// random a/b*b ~= a, relative 1e-35, incl. tiny/huge exponents
	auto rnd = [&](int emin, int emax) {
		ldb m = 1.0L + (ldb)rng() / (ldb)UINT64_MAX; // [1,2)
		if (rng() & 1) m = -m;
		int e = (int)(rng() % (emax - emin + 1)) + emin;
		return ldexpl(m, e);
	};
	F0R (it, 100000) {
		int span = it % 3; // 0: normal, 1: tiny, 2: huge
		int lo = span == 1 ? -4000 : span == 2 ? 2000 : -40;
		int hi = span == 1 ? -2000 : span == 2 ? 4000 : 40;
		qdlong a(rnd(lo, hi)), b(rnd(lo, hi));
		qdlong q = a / b, back = q * b, d = back - a;
		ldb rel = fabsl((d.hi + d.lo) / a.hi);
		assert(rel < 1e-35L);
	}
	// sqrt sanity
	F0R (it, 10000) {
		ldb v = fabsl(rnd(-100, 100));
		qdlong s = sqrt(qdlong(v)), d = s * s - qdlong(v);
		assert(fabsl((d.hi + d.lo) / v) < 1e-35L);
	}
	qdlong z = sqrt(qdlong(0.0L));
	assert(z.hi == 0.0L && z.lo == 0.0L);
	cout << "Tests passed! (1/3)*3-1 = " << (double)e13 << endl;
}
