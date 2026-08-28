// Tests polygon_area, polygon_center and in_polygon on random simple (star-
// shaped, mostly non-convex) lattice polygons. polygon_area must equal a fan-
// triangulation sum and, on small grids, Pick's theorem 2I + B - 2 with I
// counted by an atan2 winding-number brute; in_polygon (strict / non-strict)
// must agree with that brute + an exact on-edge check on every lattice point
// around the polygon (random points and on-edge lattice points on big grids);
// polygon_center must equal the fan-weighted centroid, be orientation and
// translation invariant, and hit known values. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/PolygonCenter.h" // defines P = Point<db>
#include "../../content/geometry/InsidePolygon.h" // needs P (SegmentDistance)

using L = Point<ll>;
mt19937 rng(7);
int rnd(int lo, int hi) { return (int) (rng() % (hi - lo + 1)) + lo; }

bool on(L s, L e, L p) { return p.cross(s, e) == 0 && (s - p).dot(e - p) <= 0; }
// exact: closed segments ab and cd share a point
bool touch(L a, L b, L c, L d) {
    if (sgn(a.cross(b, c)) * sgn(a.cross(b, d)) < 0 &&
        sgn(c.cross(d, a)) * sgn(c.cross(d, b)) < 0) return true;
    return on(a, b, c) || on(a, b, d) || on(c, d, a) || on(c, d, b);
}

// random simple polygon: distinct lattice points sorted by angle about their
// mean, rejected unless a brute check finds it simple (no folded-back edge)
vt<L> gen(int n, int R) {
    while (true) {
        set<pl> st;
        while (size(st) < n) st.insert({rnd(0, R), rnd(0, R)});
        vt<L> p; L c{0, 0};
        for (auto [x, y] : st) p.pb({x, y}), c = c + L{x, y};
        sort(all(p), [&](L a, L b) {
            return (a * n - c).angle() < (b * n - c).angle();
        });
        bool ok = true;
        F0R (i, n) FOR (j, i + 1, n) {
            L a = p[i], b = p[(i + 1) % n], u = p[j], v = p[(j + 1) % n];
            bool adj = j == i + 1 || (i == 0 && j == n - 1);
            if (adj ? (b - a).cross(v - u) == 0 && (b - a).dot(v - u) < 0
                    : touch(a, b, u, v)) ok = false;
        }
        if (ok) return p;
    }
}

// brute point-in-polygon: exact boundary test, then winding number by angles
bool brute_in(const vt<L>& p, L q, bool strict) {
    int n = size(p); db w = 0;
    F0R (i, n) {
        L a = p[i], b = p[(i + 1) % n];
        if (on(a, b, q)) return !strict;
        w += q.angle(a, b);
    }
    return fabs(w) > 3; // ~2pi inside, ~0 outside
}

ll fan_area2(const vt<L>& p) {
    ll a = 0;
    FOR (i, 1, size(p) - 1) a += p[0].cross(p[i], p[i + 1]);
    return a;
}

P fan_center(const vt<P>& v) {
    P c{0, 0}; db a = 0;
    FOR (i, 1, size(v) - 1) {
        db t = v[0].cross(v[i], v[i + 1]);
        c = c + (v[0] + v[i] + v[i + 1]) * t, a += t;
    }
    return c / a / 3;
}

bool near(P a, P b, db eps = 1e-7) { return (a - b).dist() < eps; }

void check(vt<L> p, int R, bool full) {
    int n = size(p);
    ll a2 = polygon_area(p);
    assert(a2 == fan_area2(p));
    reverse(all(p));
    assert(polygon_area(p) == -a2);
    reverse(all(p));

    ll B = 0;
    F0R (i, n) {
        L d = p[(i + 1) % n] - p[i];
        B += gcd(abs(d.x), abs(d.y));
    }
    vt<L> qs;
    if (full) {
        FOR (x, -1, R + 2) FOR (y, -1, R + 2) qs.pb({x, y});
    } else {
        F0R (k, 200) qs.pb({rnd(-1, R + 1), rnd(-1, R + 1)});
        F0R (k, 50) { // lattice point on a random edge
            int i = rnd(0, n - 1);
            L d = p[(i + 1) % n] - p[i];
            ll g = gcd(abs(d.x), abs(d.y));
            qs.pb(p[i] + d / g * (ll) rnd(0, (int) g));
        }
    }
    ll I = 0;
    for (L q : qs) {
        bool in = brute_in(p, q, true), bd = brute_in(p, q, false) != in;
        assert(in_polygon(p, q, true) == in);
        assert(in_polygon(p, q, false) == (in || bd));
        I += in;
    }
    if (full) assert(abs(a2) == 2 * I + B - 2); // Pick's theorem

    vt<P> v, w, u;
    for (L q : p) v.pb({(db) q.x, (db) q.y});
    assert(fabs(polygon_area(v) - (db) a2) < 1e-7);
    P c = polygon_center(v);
    assert(near(c, fan_center(v)));
    P sh{(db) rnd(-100, 100), (db) rnd(-100, 100)};
    for (P q : v) w.pb(q + sh);
    assert(near(polygon_center(w), c + sh));
    u = v, reverse(all(u));
    assert(near(polygon_center(u), c));
}

int main() {
    vt<P> tri = {P{0, 0}, P{6, 4}, P{0, 9}};
    assert(polygon_area(tri) == 54 && near(polygon_center(tri), P{2, 13. / 3}));
    vt<P> rect = {P{1, 1}, P{1, 5}, P{7, 5}, P{7, 1}}; // clockwise
    assert(polygon_area(rect) == -48 && near(polygon_center(rect), P{4, 3}));
    vt<P> ell = {P{0, 0}, P{2, 0}, P{2, 1}, P{1, 1}, P{1, 2}, P{0, 2}};
    assert(polygon_area(ell) == 6 && near(polygon_center(ell), P{2.5 / 3, 2.5 / 3}));

    F0R (it, 400) {
        int R = rnd(2, 12), n = rnd(3, min(9, (R + 1) * (R + 1) / 2));
        check(gen(n, R), R, true);
    }
    F0R (it, 150) check(gen(rnd(3, 30), 1000), 1000, false);
    cout << "Tests passed!" << endl;
}
