// Tests GaussLegendre.h: exactness for degree < 2n polynomials, and
// agreement with closed forms / adaptive Simpson. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/numerical/GaussLegendre.h"
#include "../../content/numerical/IntegrateAdaptive.h"
mt19937 rng(3);
int main() {
	FOR (n, 1, 25) F0R (it, 50) { // random poly of degree < 2n on [a, b]
		int d = rng() % (2 * n); vt<db> c(d + 1);
		for (db& x : c) x = (int) (rng() % 21) - 10;
		db a = (int) (rng() % 7) - 3, b = a + rng() % 5 + 1, want = 0;
		F0R (k, d + 1) want += c[k] * (pow(b, k + 1) - pow(a, k + 1)) / (k + 1);
		db got = gauss(a, b, [&](db x) { db v = 0; ROF (k, 0, d + 1) v = v * x + c[k]; return v; }, n);
		assert(abs(got - want) <= 1e-9 * max<db>(1, abs(want)));
	}
	assert(abs(gauss(0, M_PI, [](db x) { return sin(x); }) - 2) < 1e-12);
	assert(abs(gauss(0, 1, [](db x) { return exp(x); }) - (exp(1) - 1)) < 1e-12);
	assert(abs(gauss(-1, 1, [](db x) { return 1 / (1 + 25 * x * x); }, 60)
		- quad(-1, 1, [](db x) { return 1 / (1 + 25 * x * x); }, 1e-12)) < 1e-8);
	puts("Tests passed!");
}
