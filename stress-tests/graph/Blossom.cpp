// Tests Blossom.h (general matching): size vs subset-DP brute on n<=8, incl.
// the single-edge ae(1,2) regression; global + stack instances, init() reuse.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/random.h"
#include "../../content/graph/Blossom.h"
#include <functional>

Blossom G; // global instance, reused across cases via init()

int brute(int n, vt<pi>& es) { // es 0-indexed
	vi memo(1 << n, -1);
	function<int(int)> f = [&](int mask) -> int {
		int& r = memo[mask]; if (r != -1) return r;
		r = 0;
		for (auto& e : es) if ((mask >> e.f & 1) && (mask >> e.s & 1))
			r = max(r, 1 + f(mask ^ (1 << e.f) ^ (1 << e.s)));
		return r;
	};
	return f((1 << n) - 1);
}

void checkMate(Blossom& B, int n, int want) {
	int cnt = 0;
	FOR (i, 1, n + 1) if (B.mate[i]) { assert(B.mate[B.mate[i]] == i); cnt++; }
	assert(cnt == 2 * want);
}

int main() {
	srand(1234);
	// regression: single edge used to be dropped
	G.init(2); G.ae(1, 2); assert(G.solve() == 1);
	F0R (it, 30000) {
		int n = randIncl(1, 8), m = n < 2 ? 0 : randIncl(0, n * (n - 1) / 2 + 2);
		vt<pi> es;
		F0R (j, m) {
			int u = randRange(n), v = randRange(n);
			if (u != v) es.pb({u, v});
		}
		int want = brute(n, es);
		G.init(n);                     // reuse global via init()
		Blossom S; S.init(n);          // fresh stack instance
		for (auto& e : es) G.ae(e.f + 1, e.s + 1), S.ae(e.f + 1, e.s + 1);
		assert(G.solve() == want);
		assert(S.solve() == want);
		checkMate(G, n, want); checkMate(S, n, want);
	}
	cout << "Tests passed!" << endl;
}
