// CircleIntersection.h: random circles with integer centres and half-integer
// radii (so r^2 and d^2 are exact). Checks the intersect/no-intersect verdict
// against the exact predicate (r1-r2)^2 <= d^2 <= (r1+r2)^2, that both returned
// points lie on both circles, that they are distinct exactly when the circles
// are not tangent, the a == b case, and one hand-computed example.
// Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/CircleIntersection.h"

int main() {
	pair<P, P> out;
	// known values: unit circles at (0,0) and (1,0) meet at (1/2, +-sqrt(3)/2)
	assert(circle_inter(P{0, 0}, P{1, 0}, 1, 1, &out));
	assert(abs(out.f.x - 0.5) < 1e-12 && abs(out.s.x - 0.5) < 1e-12);
	assert(abs(abs(out.f.y) - sqrt(3) / 2) < 1e-12 && abs(out.f.y + out.s.y) < 1e-12);

	mt19937 rng(2);
	auto rnd = [&](int lo, int hi) { return (int)(rng() % (unsigned)(hi - lo + 1)) + lo; };
	int proper = 0, tangent = 0;
	F0R(it, 300000) {
		P a{(db)rnd(-20, 20), (db)rnd(-20, 20)}, b{(db)rnd(-20, 20), (db)rnd(-20, 20)};
		int k1 = rnd(0, 20), k2 = rnd(0, 20);
		db r1 = k1 / 2.0, r2 = k2 / 2.0;
		if (a == b) {
			if (k1 == k2) continue; // header asserts on identical circles
			assert(!circle_inter(a, b, r1, r2, &out));
			continue;
		}
		bool ret = circle_inter(a, b, r1, r2, &out);
		ll d4 = 4 * (ll)(b - a).dist2(), lo = (ll)(k1 - k2) * (k1 - k2), hi = (ll)(k1 + k2) * (k1 + k2);
		assert(ret == (lo <= d4 && d4 <= hi));
		if (!ret) continue;
		for (P p : {out.f, out.s}) {
			assert(abs((p - a).dist() - r1) < 1e-9);
			assert(abs((p - b).dist() - r2) < 1e-9);
		}
		db gap = (out.f - out.s).dist();
		if (lo < d4 && d4 < hi) assert(gap > 1e-6), proper++;
		else assert(gap < 1e-5), tangent++;
	}
	assert(proper > 10000 && tangent > 100);
	cout << "Tests passed!" << endl;
}
