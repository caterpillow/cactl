// Tests CirclePolygonIntersection.h: closed-form cases (disc inside polygon,
// polygon inside disc, quarter/half discs, tangencies, cw sign), random
// star-shaped and lattice polygons vs the old KACTL algorithm (areaCT), and
// a grid brute force. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/CirclePolygonIntersection.h"

typedef long double ld;
typedef Point<ld> Q;
ld len(Q p) { return sqrtl(p.dist2()); }
// old KACTL implementation (independent algorithm), in long double
ld areaCT(Q pa, Q pb, ld r) {
    if (len(pa) < len(pb)) swap(pa, pb);
    if (sgn(len(pb)) == 0) return 0;
    ld a = len(pb), b = len(pa), c = len(pb - pa);
    ld sinB = fabsl(pb.cross(pb - pa) / a / c), cosB = pb.dot(pb - pa) / a / c,
       sinC = fabsl(pa.cross(pb) / a / b), cosC = pa.dot(pb) / a / b;
    ld B = atan2l(sinB, cosB), C = atan2l(sinC, cosC);
    if (a > r) {
        ld S = C / 2 * r * r, h = a * b * sinC / c;
        if (h < r && B < M_PI / 2) S -= acosl(h / r) * r * r - h * sqrtl(r * r - h * h);
        return S;
    } else if (b > r) {
        ld theta = M_PI - B - asinl(sinB / r * a);
        return a * r * sinl(theta) / 2 + (C - theta) / 2 * r * r;
    } else return sinC * a * b / 2;
}
ld slow(P c, db r, const vt<P>& ps) {
    ld area = 0;
    F0R (i, size(ps)) {
        P p = ps[i] - c, q = ps[(i + 1) % size(ps)] - c;
        Q a{p.x, p.y}, b{q.x, q.y};
        area += areaCT(a, b, r) * sgn(a.cross(b));
    }
    return area;
}
bool inPoly(const vt<P>& ps, P a) { // crossing number
    bool in = 0;
    F0R (i, size(ps)) {
        P p = ps[i], q = ps[(i + 1) % size(ps)];
        if ((p.y > a.y) != (q.y > a.y) && a.x < p.x + (a.y - p.y) * (q.x - p.x) / (q.y - p.y)) in ^= 1;
    }
    return in;
}
db grid(P c, db r, const vt<P>& ps, db h) { // count lattice points of spacing h
    int n = (int) (r / h) + 1; ll cnt = 0;
    FOR (i, -n, n + 1) FOR (j, -n, n + 1) {
        P a{c.x + i * h, c.y + j * h};
        if ((a - c).dist2() <= r * r && inPoly(ps, a)) cnt++;
    }
    return (db) cnt * h * h;
}

mt19937 rng(7);
db rnd(db lo, db hi) { return uniform_real_distribution<db>(lo, hi)(rng); }
db ri(int lo, int hi) { return uniform_int_distribution<int>(lo, hi)(rng); } // integer as db
vt<P> star(P o, int n) { // ccw star-shaped (possibly concave) polygon around o
    vt<db> ang(n);
    for (db& a : ang) a = rnd(0, 2 * M_PI);
    sort(all(ang));
    vt<P> ps;
    for (db a : ang) ps.pb(o + P{cos(a), sin(a)} * rnd(0.5, 6));
    return ps;
}

int main() {
    auto eq = [](db a, db b, db eps = 1e-9) { return abs(a - b) < eps; };
    vt<P> big = {P{-10, -10}, P{10, -10}, P{10, 10}, P{-10, 10}};
    vt<P> sq = {P{0, 0}, P{10, 0}, P{10, 10}, P{0, 10}};
    vt<P> tri = {P{0, 0}, P{4, 0}, P{0, 3}};
    assert(eq(circlePoly(P{0, 0}, 3, big), 9 * M_PI));   // disc inside polygon
    assert(eq(circlePoly(P{1, 1}, 100, tri), 6));        // polygon inside disc
    assert(eq(circlePoly(P{0, 0}, 2, sq), M_PI));        // quarter disc at corner
    assert(eq(circlePoly(P{5, 0}, 2, sq), 2 * M_PI));    // half disc on edge
    assert(eq(circlePoly(P{-2, 5}, 2, sq), 0));          // externally tangent
    assert(eq(circlePoly(P{2, 5}, 2, sq), 4 * M_PI));    // internally tangent
    assert(eq(circlePoly(P{50, 50}, 3, sq), 0));         // disjoint
    reverse(all(big));
    assert(eq(circlePoly(P{0, 0}, 3, big), -9 * M_PI));  // cw -> negated

    F0R (it, 30000) { // random star polygons vs old algorithm
        vt<P> ps = star(P{rnd(-5, 5), rnd(-5, 5)}, (int) ri(3, 12));
        P c{rnd(-8, 8), rnd(-8, 8)}; db r = rnd(0.1, 8);
        db got = circlePoly(c, r, ps), want = (db) slow(c, r, ps);
        assert(eq(got, want, 1e-7));
        if (it < 8) assert(eq(got, grid(c, r, ps, 0.01), 0.05)); // brute force
    }
    F0R (it, 30000) { // lattice rectangles / triangles with integer discs: exact tangencies
        vt<P> ps;
        if (it & 1) {
            db x1 = ri(0, 6), y1 = ri(0, 6), x2 = x1 + ri(1, 6), y2 = y1 + ri(1, 6);
            ps = {P{x1, y1}, P{x2, y1}, P{x2, y2}, P{x1, y2}};
        } else {
            F0R (k, 3) ps.pb(P{ri(0, 6), ri(0, 6)});
            db cr = ps[0].cross(ps[1], ps[2]);
            if (cr == 0) continue;
            if (cr < 0) reverse(all(ps));
        }
        P c{ri(-2, 8), ri(-2, 8)}; db r = ri(1, 8);
        assert(eq(circlePoly(c, r, ps), (db) slow(c, r, ps), 1e-7));
    }
    cout << "Tests passed!" << endl;
}
