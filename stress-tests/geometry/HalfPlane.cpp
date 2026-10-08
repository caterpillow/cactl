// Tests HalfPlane.h: hpi() vs brute-force clipping of a big box by every
// half-plane (PolygonCut style). Small integer lines give parallel, opposite,
// duplicate and empty cases; compares area and vertex sets. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/HalfPlane.h"

db area(const vt<P>& v) {
    db a = 0;
    F0R (i, size(v)) a += v[i].cross(v[(i + 1) % size(v)]);
    return a / 2;
}
vt<P> clip(vt<P> poly, const Hp& h) { // keep points left of h (closed)
    vt<P> r;
    F0R (i, size(poly)) {
        P a = poly[i], b = poly[(i + 1) % size(poly)];
        db ca = h.d.cross(a - h.p), cb = h.d.cross(b - h.p);
        if (ca >= 0) r.pb(a);
        if ((ca > 0 && cb < 0) || (ca < 0 && cb > 0)) r.pb(a + (b - a) * (ca / (ca - cb)));
    }
    return r;
}
db distSeg(P p, P a, P b) {
    P d = b - a; db t = d.dot(p - a) / max(d.dot(d), 1e-18);
    t = max((db) 0, min((db) 1, t));
    P q = a + d * t;
    return sqrt((p - q).dot(p - q));
}
db distPoly(P p, const vt<P>& v) {
    db r = 1e18;
    F0R (i, size(v)) r = min(r, distSeg(p, v[i], v[(i + 1) % size(v)]));
    return r;
}

int main() {
    mt19937 rng(12345);
    int nonEmpty = 0, empty = 0;
    F0R (it, 300000) {
        db B = 20; int lim = 1 + (int) (rng() % 8), m = (int) (rng() % 8);
        auto rc = [&]() { return (db) ((int) (rng() % (2 * lim + 1)) - lim); };
        vt<Hp> h = {Hp(P{-B, -B}, P{B, -B}), Hp(P{B, -B}, P{B, B}),
                    Hp(P{B, B}, P{-B, B}), Hp(P{-B, B}, P{-B, -B})};
        F0R (i, m) {
            P a{rc(), rc()}, b{rc(), rc()};
            if (a.x == b.x && a.y == b.y) b.x += 1;
            h.pb(Hp(a, b));
            if (rng() % 4 == 0) h.pb(Hp(b, a)); // opposite
            if (rng() % 6 == 0) h.pb(Hp(a * (db) 2 - b, b)); // same line, same dir
        }
        shuffle(all(h), rng);
        vt<P> poly = {P{-B, -B}, P{B, -B}, P{B, B}, P{-B, B}};
        for (auto& x : h) poly = clip(poly, x);
        db A = size(poly) >= 3 ? area(poly) : 0;
        vt<P> got = hpi(h);
        if (A < 1e-7) { // empty or degenerate: strict region has area 0
            assert(size(got) == 0 || area(got) < 1e-7);
            empty++; continue;
        }
        nonEmpty++;
        assert(size(got) >= 3);
        assert(fabs(area(got) - A) < 1e-6);
        for (P p : got) { // every output vertex is on the brute polygon boundary
            assert(distPoly(p, poly) < 1e-6);
            for (auto& x : h) assert(x.d.cross(p - x.p) > -1e-6);
        }
        for (P p : poly) assert(distPoly(p, got) < 1e-6); // and vice versa
    }
    assert(nonEmpty > 1000 && empty > 1000);
    cout << "Tests passed!" << endl;
}
