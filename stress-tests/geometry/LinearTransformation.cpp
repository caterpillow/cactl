// Tests linearTransformation.h: the similarity mapping p0->q0, p1->q1 sends
// p0, p1 exactly and preserves ratios/angles for random points.
// Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/geometry/linearTransformation.h"
mt19937 rng(8);
db rd() { return (int) (rng() % 2001) - 1000; }
int main() {
	F0R (it, 100000) {
		P p0{rd(), rd()}, p1{rd(), rd()}, q0{rd(), rd()}, q1{rd(), rd()}, r{rd(), rd()};
		if ((p1 - p0).dist2() < 1 || (q1 - q0).dist2() < 1) continue;
		auto f = [&](P x) { return linear_transformation(p0, p1, q0, q1, x); };
		assert((f(p0) - q0).dist() < 1e-6 && (f(p1) - q1).dist() < 1e-6);
		P fr = f(r); // same relative position: ratio of distances and signed area scale
		db s = (q1 - q0).dist() / (p1 - p0).dist();
		assert(abs((fr - q0).dist() - s * (r - p0).dist()) < 1e-6 * (1 + s * (r - p0).dist()));
		assert(abs((q1 - q0).cross(fr - q0) - s * s * (p1 - p0).cross(r - p0)) < 1e-3 * (1 + s * s * abs((p1 - p0).cross(r - p0))));
	}
	puts("Tests passed!");
}
