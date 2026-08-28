// Stress test for InsidePolygon.h (in_polygon). Written by Claude (audit).
// Random star-shaped simple polygons with small integer coords (many
// vertex-on-ray / on-boundary cases, both orientations). Expected answer:
// exact on-segment check + winding number by angle summation. Checks both
// Point<ll> and Point<db> instantiations, plus the doc's usage example.
#include "../utilities/template.h"

#include "../../content/geometry/Point.h"
using P = Point<db>; // SegmentDistance.h (pulled in by the header) needs global P
#include "../../content/geometry/InsidePolygon.h"

using L = Point<ll>;
mt19937 rng(1234);
ll rnd(ll n) { return uniform_int_distribution<ll>(0, n - 1)(rng); }

// simple polygon: distinct directions around c sorted by angle, all gaps < pi
vt<L> gen(int n, ll R) {
    L c{R / 2, R / 2};
    for (;;) {
        vt<L> v;
        F0R (i, n) {
            L p{rnd(R), rnd(R)};
            bool ok = !(p == c);
            for (L q : v)
                if ((p - c).half() == (q - c).half() && (p - c).cross(q - c) == 0) ok = false;
            if (ok) v.pb(p);
        }
        sort(all(v), [&](L a, L b) {
            a = a - c, b = b - c;
            return a.half() != b.half() ? a.half() < b.half() : a.cross(b) > 0;
        });
        bool ok = size(v) >= 3;
        F0R (i, size(v)) if ((v[i] - c).cross(v[(i + 1) % size(v)] - c) <= 0) ok = false;
        if (ok) return v;
    }
}

// 0 = outside, 1 = inside, 2 = on boundary
int brute(vt<L>& v, L p) {
    int n = size(v);
    long double w = 0, tau = 2 * acosl(-1);
    F0R (i, n) {
        L a = v[i] - p, b = v[(i + 1) % n] - p;
        if (a.cross(b) == 0 && a.dot(b) <= 0) return 2;
        w += atan2l((long double) a.cross(b), (long double) a.dot(b));
    }
    ll k = llroundl(w / tau);
    assert(abs(k) <= 1 && fabsl(w / tau - (long double) k) < 1e-6);
    return k != 0;
}

int main() {
    // doc example: triangle, (3,3) inside, (1,2) vertex, (2.5,3) on an edge
    vt<P> tri = {P{4, 4}, P{1, 2}, P{2, 1}};
    assert(in_polygon(tri, P{3, 3}) && in_polygon(tri, P{3, 3}, false));
    assert(!in_polygon(tri, P{1, 2}) && in_polygon(tri, P{1, 2}, false));
    assert(!in_polygon(tri, P{2.5, 3}) && in_polygon(tri, P{2.5, 3}, false));
    assert(!in_polygon(tri, P{3, 1}, false) && !in_polygon(tri, P{0, 0}, false));

    F0R (it, 3000) {
        ll R = it % 3 == 0 ? 5 : it % 3 == 1 ? 12 : 1000;
        vt<L> v = gen((int) rnd(30) + 3, R);
        if (rnd(2)) reverse(all(v));
        vt<P> vd;
        for (L q : v) vd.pb({(db) q.x, (db) q.y});
        F0R (j, 30) {
            L q{rnd(R + 2) - 1, rnd(R + 2) - 1};
            P qd{(db) q.x, (db) q.y};
            int b = brute(v, q);
            for (bool st : {false, true}) {
                bool want = b == 2 ? !st : b;
                assert(in_polygon(v, q, st) == want);
                assert(in_polygon(vd, qd, st) == want);
            }
        }
    }
    cout << "Tests passed!" << endl;
}
