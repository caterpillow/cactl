// Smoke tests for various/ headers with no dedicated stress test:
// GrayCode (bijection, inverse, adjacent codes differ in 1 bit),
// IntervalCover (valid + minimal vs brute force), LeakSanitizer (compiles).
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/various/GrayCode.h"
#include "../../content/various/IntervalCover.h"
#include "../../content/various/LeakSanitizer.h"

int bruteCover(pi G, vt<pi>& I) { // min #intervals covering [G.f, G.s), or inf
	int n = size(I), best = inf;
	F0R (m, 1 << n) {
		int cur = G.f, cnt = __builtin_popcount(m);
		if (cnt >= best) continue;
		bool prog = true;
		while (cur < G.s && prog) {
			prog = false;
			F0R (i, n) if (m >> i & 1 && I[i].f <= cur && I[i].s > cur) cur = I[i].s, prog = true;
		}
		if (cur >= G.s) best = cnt;
	}
	return best;
}

int main() {
	F0R (n, 1 << 20) assert(rev_g(g(n)) == n);
	FOR (n, 1, 1 << 20) assert(__builtin_popcount(g(n) ^ g(n - 1)) == 1);

	mt19937 rng(7);
	F0R (it, 20000) {
		int n = rng() % 9;
		vt<pi> I(n);
		for (auto& [l, r] : I) { l = rng() % 20; r = l + rng() % 10; }
		pi G = {rng() % 15, 0}; G.s = G.f + rng() % 10;
		vi R = cover(G, I);
		int brute = bruteCover(G, I);
		if (R.empty() && G.f < G.s) assert(brute == inf);
		else {
			int cur = G.f; // R's intervals must cover greedily left-to-right
			for (int i : R) { assert(I[i].f <= cur); cur = max(cur, I[i].s); }
			assert(cur >= G.s);
			assert(size(R) == (G.f < G.s ? brute : 0));
		}
	}
	printf("Tests passed!\n");
}
