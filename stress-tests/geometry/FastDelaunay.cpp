// Stress test for FastDelaunay.h, written by Claude (audit).
// Checks triangulate() against brute force: every triangle is CCW with an
// exactly empty circumcircle (__int128 incircle determinant), no duplicate
// triangles, every point is a vertex, #triangles == 2n-h-2 (h = points on
// the hull boundary), twice the triangle areas sum to
// polygon_area(convex_hull(pts)), and for point sets in general position
// (no 3 collinear, no 4 concyclic) the triangle set equals the unique
// O(n^4) brute-force Delaunay set. Inputs: small integer boxes (many
// collinear/concyclic points), all-collinear sets, a full lattice grid,
// 20 lattice points on one circle, and larger random sets.
#include "../utilities/template.h"

#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/PolygonArea.h"

// FastDelaunay.h never frees its Quads; route `new Quad{...}` into a bump
// arena that is reset (together with the header's free list H) per case.
const size_t CAP = 64 << 20;
alignas(16) char arena_buf[CAP];
struct Arena {
	size_t top = CAP;
	template<class T> T* operator=(T&& x) {
		assert(top >= sizeof(T));
		top = (top - sizeof(T)) & ~size_t(15);
		return new (arena_buf + top) T(move(x));
	}
} arena;
#define new arena =
#include "../../content/geometry/FastDelaunay.h"
#undef new

mt19937 rng(12345);
ll rnd(ll lo, ll hi) { return uniform_int_distribution<ll>(lo, hi)(rng); }

// > 0 iff d is strictly inside the circumcircle of CCW triangle abc
lll incircle(P a, P b, P c, P d) {
	a = a - d, b = b - d, c = c - d;
	return (lll) a.dist2() * b.cross(c) + (lll) b.dist2() * c.cross(a)
	     + (lll) c.dist2() * a.cross(b);
}

void check(vt<P> pts) {
	sort(all(pts)); pts.erase(unique(all(pts)), end(pts));
	int n = size(pts);
	arena.top = CAP; H = 0;
	vt<P> tri = triangulate(pts);
	assert(size(tri) % 3 == 0);

	auto idx = [&](P p) {
		int i = int(lower_bound(all(pts), p) - begin(pts));
		assert(i < n && pts[i] == p);
		return i;
	};
	vt<array<int, 3>> got;
	vi used(n);
	ll area = 0;
	for (int t = 0; t < size(tri); t += 3) {
		array<int, 3> id;
		F0R(j, 3) id[j] = idx(tri[t + j]);
		P a = pts[id[0]], b = pts[id[1]], c = pts[id[2]];
		assert(a.cross(b, c) > 0);
		F0R(l, n) assert(incircle(a, b, c, pts[l]) <= 0);
		area += a.cross(b, c);
		F0R(j, 3) used[id[j]] = 1;
		sort(all(id)); got.pb(id);
	}
	sort(all(got));
	assert(adjacent_find(all(got)) == end(got));

	bool line = true;
	FOR(i, 2, n) if (pts[0].cross(pts[1], pts[i])) line = false;
	if (line) { assert(got.empty()); return; }

	F0R(i, n) assert(used[i]);
	vt<P> hull = convex_hull(pts);
	int h = 0;
	F0R(i, n) F0R(j, size(hull)) {
		P a = hull[j], b = hull[(j + 1) % size(hull)], p = pts[i];
		if (a.cross(b, p) == 0 && (p - a).dot(p - b) <= 0) { h++; break; }
	}
	assert(size(got) == 2 * n - h - 2);
	assert(area == polygon_area(hull));

	// general position -> the Delaunay triangulation is unique
	if (n > 12) return;
	F0R(i, n) F0R(j, i) F0R(k, j) {
		if (pts[i].cross(pts[j], pts[k]) == 0) return;
		F0R(l, k) if (incircle(pts[i], pts[j], pts[k], pts[l]) == 0) return;
	}
	vt<array<int, 3>> want;
	F0R(i, n) F0R(j, i) F0R(k, j) {
		P a = pts[k], b = pts[j], c = pts[i];
		if (a.cross(b, c) < 0) swap(b, c);
		bool ok = true;
		F0R(l, n) if (incircle(a, b, c, pts[l]) > 0) ok = false;
		if (ok) want.pb({k, j, i});
	}
	sort(all(want));
	assert(got == want);
}

int main() {
	// small integer boxes: lots of collinear / concyclic points
	F0R(it, 30000) {
		int n = (int) rnd(1, 20);
		ll rx = rnd(1, 50), ry = rnd(1, 50);
		vt<P> v;
		F0R(i, n) v.pb({rnd(-rx, rx), rnd(-ry, ry)});
		check(v);
	}
	// general position: exact comparison with the brute-force set
	F0R(it, 5000) {
		int n = (int) rnd(3, 12);
		vt<P> v;
		F0R(i, n) v.pb({rnd(-10000, 10000), rnd(-10000, 10000)});
		check(v);
	}
	// all points on one line
	F0R(it, 300) {
		int n = (int) rnd(1, 30);
		ll dx = rnd(-5, 5), dy = rnd(-5, 5), bx = rnd(-50, 50), by = rnd(-50, 50);
		if (!dx && !dy) dx = 1;
		vt<P> v;
		F0R(i, n) { ll t = rnd(-100, 100); v.pb({bx + dx * t, by + dy * t}); }
		check(v);
	}
	{ // lattice grid and 20 lattice points on x^2 + y^2 = 25^2
		vt<P> v;
		F0R(x, 8) F0R(y, 8) v.pb({x, y});
		check(v);
		v.clear();
		FOR(x, -25, 26) FOR(y, -25, 26) if (x * x + y * y == 625) v.pb({x, y});
		assert(size(v) == 20);
		check(v);
	}
	// larger random sets, larger coordinates
	F0R(it, 3) {
		vt<P> v;
		F0R(i, 2000) v.pb({rnd(-1000000, 1000000), rnd(-1000000, 1000000)});
		check(v);
		v.clear();
		F0R(i, 500) v.pb({rnd(-200000000, 200000000), rnd(-200000000, 200000000)});
		check(v);
	}
	cout << "Tests passed!" << endl;
}
