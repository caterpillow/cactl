// Tests MinkowskiSum.h: minkowski_sum vs convex hull of all pairwise sums,
// and diameter2 vs O(n^2) brute force. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/geometry/MinkowskiSum.h"

mt19937 rng(20260827);
vP randPoly(int n, int C) { // random convex polygon (possibly a point/segment)
	vP p(n);
	for (auto &q : p) q = {(ll) (rng() % (2 * C + 1)) - C, (ll) (rng() % (2 * C + 1)) - C};
	return convex_hull(p);
}
bool sameSet(vP a, vP b) { sort(all(a)); sort(all(b)); return a == b; }

int main() {
	F0R (it, 20000) {
		int C = it % 2 ? 5 : 1000;
		vP a = randPoly(rng() % 6 + 1, C), b = randPoly(rng() % 6 + 1, C);
		vP all_sums;
		for (P x : a) for (P y : b) all_sums.pb(x + y);
		vP want = convex_hull(all_sums), got = minkowski_sum(a, b);
		// got may contain collinear boundary points; hull it to compare
		assert(sameSet(convex_hull(got), want));
		if (size(got) >= 3) F0R (i, size(got)) // CCW check
			assert(got[i].cross(got[(i + 1) % size(got)], got[(i + 2) % size(got)]) >= 0);
	}
	F0R (it, 3000) {
		vP p(rng() % 8 + 1);
		for (auto &q : p) q = {(ll) (rng() % 2001) - 1000, (ll) (rng() % 2001) - 1000};
		ll want = 0;
		for (P x : p) for (P y : p) want = max(want, (x - y).dist2());
		assert(diameter2(p) == want);
	}
	puts("Tests passed!");
}
