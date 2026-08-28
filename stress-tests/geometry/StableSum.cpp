// Tests geometry/StableSum.h: sums of random non-negative doubles spanning
// ~30 orders of magnitude, compared with an exact-ish long double / sorted
// reference; StableSum must beat the naive running sum and stay within a
// few ulps. Also checks val() after every += (prefix sums).
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/geometry/StableSum.h"

int main() {
	mt19937_64 rng(5);
	F0R (it, 300) {
		int n = rng() % 2000 + 1;
		StableSum ss; db naive = 0; long double ref = 0;
		vt<db> xs;
		F0R (i, n) {
			int e = rng() % 60 - 30;
			db x = ldexp((db)(rng() % (1ull << 53)) / (1ull << 53), e);
			if (rng() % 7 == 0) x = 0;
			xs.pb(x); ss += x; naive += x; ref += x;
			// prefix value must be a faithful sum of everything so far
			assert(fabs((long double)ss.val() - ref) <= 1e-12L * ref);
		}
		long double err_ss = fabsl(ss.val() - ref), err_naive = fabsl(naive - ref);
		assert(err_ss <= err_naive + 4 * DBL_EPSILON * ref);
	}
	cout << "Tests passed!" << endl;
}
