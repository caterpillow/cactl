// CircleTangents.h (+ lineDistance.h): external/internal tangents of two circles.
// Tangent count is checked against exact integer geometry, the tangency points
// against an independent acos-based construction, and every returned tangent
// against the on-circle / distance / side properties. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/CircleTangents.h"
#include "../../content/geometry/lineDistance.h"

using P = Point<db>;
const db EPS = 1e-9;

// Independent construction: a common tangent has unit normal n with
// n.(c2-c1) = r2-r1; the tangency point of circle (c, r) is then c - n*r.
vt<pair<P, P>> brute(P c1, db r1, P c2, db r2) {
    P d = c2 - c1;
    db dd = d.dist();
    if (dd == 0) return {};
    db c = (r2 - r1) / dd;
    if (abs(c) > 1) return {};
    db a = acos(c);
    vt<pair<P, P>> out;
    for (db t : {d.angle() + a, d.angle() - a}) {
        P n{cos(t), sin(t)};
        out.pb({c1 - n * r1, c2 - n * r2});
        if (abs(c) == 1) break;
    }
    return out;
}

bool near(P a, P b) { return (a - b).dist() < EPS; }
bool same(pair<P, P> a, pair<P, P> b) { return near(a.f, b.f) && near(a.s, b.s); }

int main() {
    srand(1);
    int seen[3] = {};
    F0R(it, 400000) {
        P c1{db(rand() % 11 - 5), db(rand() % 11 - 5)};
        P c2{db(rand() % 11 - 5), db(rand() % 11 - 5)};
        db r1, r2;
        bool exact = it < 200000;
        if (exact) r1 = rand() % 6, r2 = rand() % 6;
        else r1 = db(rand() % 450) / 100 + 0.5, r2 = db(rand() % 450) / 100 + 0.5;
        for (int sgn2 : {1, -1}) {
            db rr = sgn2 * r2;
            db d2 = (c2 - c1).dist2(), h2 = d2 - (r1 - rr) * (r1 - rr);
            if (!exact && abs(h2) < 1e-6) continue; // ill-conditioned, both sides
            int want = d2 == 0 || h2 < 0 ? 0 : h2 == 0 ? 1 : 2;
            auto got = tangents(c1, r1, c2, rr), exp = brute(c1, r1, c2, rr);
            assert(size(got) == want && size(exp) == want);
            seen[want]++;
            F0R(i, want) {
                P a = got[i].f, b = got[i].s;
                assert(abs((a - c1).dist() - r1) < EPS && abs((b - c2).dist() - r2) < EPS);
                if (r1 == 0) assert(a == c1);
                if (r2 == 0) assert(b == c2);
            }
            if (want == 1) {
                assert(near(got[0].f, got[0].s) && same(got[0], exp[0]));
                assert(abs((got[0].f - c1).cross(c2 - c1)) < EPS); // on the center line
            }
            if (want == 2) {
                assert((same(got[0], exp[0]) && same(got[1], exp[1])) ||
                       (same(got[0], exp[1]) && same(got[1], exp[0])));
                for (auto [a, b] : got) {
                    db l1 = line_dist(a, b, c1), l2 = line_dist(a, b, c2);
                    assert(abs(abs(l1) - r1) < EPS && abs(abs(l2) - r2) < EPS);
                    // external: centers on the same side; internal: opposite sides
                    if (r1 > 0 && r2 > 0) assert(sgn(l1) * sgn(l2) == sgn2);
                }
            }
        }
    }
    F0R(k, 3) assert(seen[k] > 1000);
    cout << "Tests passed!" << endl;
}
