// Tests PolygonUnion.h: disjoint/overlapping/nested squares, then random
// axis-aligned rectangle pairs & triples vs inclusion-exclusion.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/PolygonUnion.h"

vt<P> rect(db x1, db y1, db x2, db y2) { // CCW
    return {P{x1, y1}, P{x2, y1}, P{x2, y2}, P{x1, y2}};
}
db inter(pair<pi, pi> a, pair<pi, pi> b) { // rect overlap area
    db w = min(a.s.f, b.s.f) - max(a.f.f, b.f.f), h = min(a.s.s, b.s.s) - max(a.f.s, b.f.s);
    return max(w, (db) 0) * max(h, (db) 0);
}

int main() {
    { // two disjoint unit squares -> 2
        vt<vt<P>> ps = {rect(0, 0, 1, 1), rect(3, 0, 4, 1)};
        assert(abs(poly_union(ps) - 2) < 1e-9);
    }
    { // overlapping squares -> 7
        vt<vt<P>> ps = {rect(0, 0, 2, 2), rect(1, 1, 3, 3)};
        assert(abs(poly_union(ps) - 7) < 1e-9);
    }
    { // nested + identical -> 9
        vt<vt<P>> ps = {rect(0, 0, 3, 3), rect(1, 1, 2, 2), rect(0, 0, 3, 3)};
        assert(abs(poly_union(ps) - 9) < 1e-9);
    }
    mt19937 rng(5);
    F0R (it, 100000) {
        int k = 2 + it % 2; // 2 or 3 rectangles
        vt<pair<pi, pi>> rs;
        vt<vt<P>> ps;
        F0R (i, k) {
            int x1 = rng() % 8, y1 = rng() % 8, x2 = x1 + 1 + rng() % 8, y2 = y1 + 1 + rng() % 8;
            rs.pb({{x1, y1}, {x2, y2}});
            ps.pb(rect(x1, y1, x2, y2));
        }
        db want = 0; // inclusion-exclusion
        F0R (i, k) want += inter(rs[i], rs[i]);
        F0R (i, k) FOR (j, i + 1, k) want -= inter(rs[i], rs[j]);
        if (k == 3) {
            int w = min({rs[0].s.f, rs[1].s.f, rs[2].s.f}) - max({rs[0].f.f, rs[1].f.f, rs[2].f.f});
            int h = min({rs[0].s.s, rs[1].s.s, rs[2].s.s}) - max({rs[0].f.s, rs[1].f.s, rs[2].f.s});
            want += (db) max(w, 0) * max(h, 0);
        }
        assert(abs(poly_union(ps) - want) < 1e-6);
    }
    cout << "Tests passed!" << endl;
}
