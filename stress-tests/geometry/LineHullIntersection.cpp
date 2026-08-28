// Tests lineHull(a, b, poly) (and extrVertex through it) against an O(n)
// brute force: classify every hull vertex by side of the directed line, derive
// the exact expected answer (miss / corner touch / along side / two crossings
// ordered by exact rational hit parameter), both line directions. Hulls come
// from convex_hull on random small integer point sets; lines are random or
// deliberately pass through hull corners. Degenerate 1-/2-point hulls (which
// violate the header's no-collinear-points precondition) only get hit/no-hit
// and corner-touch checks. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/LineHullIntersection.h"

struct Hit { ll num, den; int idx; }; // hit parameter t = num/den, den > 0

// exact expected answer for a strictly convex ccw polygon with n >= 3
array<int, 2> brute(P a, P b, vt<P>& poly) {
    int n = size(poly);
    P d = b - a;
    vi sd(n);
    int pos = 0, neg = 0, zero = 0;
    F0R (i, n) {
        sd[i] = sgn(d.cross(poly[i] - a));
        pos += sd[i] > 0, neg += sd[i] < 0, zero += sd[i] == 0;
    }
    assert(zero <= 2);
    if (!pos || !neg) { // polygon on one side: miss / corner / side
        if (!zero) return {-1, -1};
        if (zero == 1) F0R (i, n) if (!sd[i]) return {i, -1};
        F0R (i, n) if (!sd[i] && !sd[(i + 1) % n]) return {i, i};
        assert(false);
    }
    vt<Hit> hits; // a corner crossing at i counts as side (i, i+1)
    F0R (i, n) {
        int j = (i + 1) % n;
        if (sd[i] == 0) hits.pb({(poly[i] - a).dot(d), d.dist2(), i});
        else if (sd[i] * sd[j] < 0) {
            P e = poly[j] - poly[i];
            ll num = (poly[i] - a).cross(e), den = d.cross(e);
            if (den < 0) num = -num, den = -den;
            hits.pb({num, den, i});
        }
    }
    assert(size(hits) == 2);
    Hit h0 = hits[0], h1 = hits[1];
    assert(h0.num * h1.den != h1.num * h0.den); // two distinct hit points
    if (h0.num * h1.den > h1.num * h0.den) swap(h0, h1);
    return {h0.idx, h1.idx};
}

int main() {
    mt19937 rng(7);
    auto rnd = [&](int lo, int hi) {
        return (ll) uniform_int_distribution<int>(lo, hi)(rng);
    };
    int cntHull[3] = {};
    F0R (it, 200000) {
        int R = it % 3 == 0 ? 1000 : it % 3 == 1 ? 8 : 3;
        int m = (int) rnd(1, R > 100 ? 40 : 12);
        vt<P> pts(m);
        for (P& p : pts) p = {rnd(-R, R), rnd(-R, R)};
        vt<P> poly = convex_hull(pts);
        int n = size(poly);
        assert(n >= 1);
        P a{rnd(-R - 1, R + 1), rnd(-R - 1, R + 1)};
        P b{rnd(-R - 1, R + 1), rnd(-R - 1, R + 1)};
        int mode = (int) rnd(0, 3);
        if (mode >= 1) a = poly[(int) rnd(0, n - 1)];
        if (mode == 2) b = poly[(int) rnd(0, n - 1)];
        if (a == b) continue;
        cntHull[min(n, 3) - 1]++;

        auto fail = [&](P x, P y, array<int, 2> got, array<int, 2> exp) {
            cout << "hull:"; for (P p : poly) cout << ' ' << p; cout << '\n';
            cout << "line " << x << "-> " << y << "got (" << got[0] << ", "
                 << got[1] << ") expected (" << exp[0] << ", " << exp[1]
                 << ")\n";
            exit(1);
        };

        F0R (dir, 2) {
            P x = dir ? b : a, y = dir ? a : b;
            array<int, 2> got = lineHull(x, y, poly);
            if (n >= 3) {
                array<int, 2> exp = brute(x, y, poly);
                if (got != exp) fail(x, y, got, exp);
                continue;
            }
            // degenerate hulls: only hit/no-hit and lone corner touch
            P d = y - x;
            int s0 = sgn(d.cross(poly[0] - x));
            int s1 = sgn(d.cross(poly[n - 1] - x));
            array<int, 2> miss = {-1, -1};
            if (s0 * s1 > 0 || (n == 1 && s0)) {
                if (got != miss) fail(x, y, got, miss);
            } else if (got == miss) fail(x, y, got, {0, -1});
            else if (n == 2 && (s0 == 0) != (s1 == 0)) {
                array<int, 2> exp = {s0 == 0 ? 0 : 1, -1};
                if (got != exp) fail(x, y, got, exp);
            }
        }
    }
    // make sure all hull sizes actually occurred
    F0R (i, 3) assert(cntHull[i] > 1000);
    cout << "Tests passed!" << endl;
}
