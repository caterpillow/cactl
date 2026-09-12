// Tests hull_tangents(poly, p) against an O(n) exact brute force: for every
// vertex the sign of cross(poly[i]-p, poly[j]-p) over all j gives the set of
// vertices qualifying as a (all <= 0) / b (all >= 0); expected answer is
// (i, -1) or (i, i+1) with i the earlier vertex in ccw order (wrap included).
// Hulls: sampled subsets of a 4x4 grid with every integer p in [-3,6]^2
// strictly outside; random hulls with random / far / near-vertex p and p on
// the extension line of a side (tie answers, both orientations, counted).
// A ~1e5-vertex hull is checked for tangency only. A copy of the function
// using the eps side_of (as in the header comment) on Point<double> must agree
// on the hulls with |coord| <= 1000, and with coordinates perturbed by +-1e-12
// on those with |coord| <= 8 (a perturbation of p next to a vertex rotates the
// ray by 1e-12 rad, which times a lever arm of 1000 exceeds eps). Degenerate
// 1-/2-point hulls: n = 1 must just not crash (it returns (0,0) twice), n = 2
// with p off the segment's line must match the brute force.
// Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/Point.h"
using P = Point<ll>;
using D = Point<db>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/sideOf.h"
#include "../../content/geometry/PointInsideHull.h"
#include "../../content/geometry/HullTangents.h"

// header's function with the doubles line from its comment
#define angd(i,j) (d * side_of(p, poly[(i)%n], poly[(j)%n], eps))
#define tangd(i) (angd(i + 1, i) >= 0 && angd(i, i - 1 + n) < 0)
pair<pi, pi> hull_tangents_d(vt<D>& poly, D p, db eps) {
    int n = size(poly); pi res[2];
    for (int d : {1, -1}) {
        int lo = 0, hi = n;
        if (!tangd(0)) while (lo + 1 < hi) {
            int m = (lo + hi) / 2;
            if (tangd(m)) { lo = m; break; }
            int a = angd(lo + 1, lo), b = angd(m + 1, m);
            (a < b || (a == b && a == angd(lo, m)) ? hi : lo) = m;
        }
        res[d < 0] = {lo, angd(lo, lo + 1) != 0 ? -1 : (lo + 1) % n};
    }
    return {res[0], res[1]};
}

mt19937 rng(2026);
ll rnd(ll a, ll b) { return uniform_int_distribution<ll>(a, b)(rng); }

ll cnt[4]; // a single, a pair, b single, b pair
ll cntWrap, cntBig, cntDeg, cntTests;

bool outside(vt<P>& poly, P p) {
    if (size(poly) >= 3) return !in_hull(poly, p, false);
    return !on_segment(poly[0], poly.back(), p);
}

// exact expectation for n >= 3, p strictly outside
pair<pi, pi> brute(vt<P>& poly, P p) {
    int n = size(poly);
    vi A, B;
    F0R (i, n) {
        bool a = 1, b = 1;
        F0R (j, n) {
            int s = sgn(p.cross(poly[i], poly[j]));
            a &= s <= 0, b &= s >= 0;
        }
        if (a) A.pb(i);
        if (b) B.pb(i);
    }
    auto mk = [&](vi& v) {
        assert(size(v) == 1 || size(v) == 2);
        if (size(v) == 1) return pi{v[0], -1};
        if (v[0] == 0 && v[1] == n - 1) return pi{n - 1, 0};
        assert(v[1] == v[0] + 1);
        return pi{v[0], v[1]};
    };
    return {mk(A), mk(B)};
}

void fail(vt<P>& poly, P p, pair<pi, pi> got, pair<pi, pi> exp, const char* w) {
    cout << w << " hull:"; for (P q : poly) cout << ' ' << q; cout << '\n';
    cout << "p " << p << "got (" << got.f.f << "," << got.f.s << ") ("
         << got.s.f << "," << got.s.s << ") expected (" << exp.f.f << ","
         << exp.f.s << ") (" << exp.s.f << "," << exp.s.s << ")\n";
    exit(1);
}

vt<D> toD(vt<P>& poly, db pert) {
    vt<D> r;
    for (P q : poly) r.pb({(db) q.x + pert * (db) rnd(-1, 1), (db) q.y + pert * (db) rnd(-1, 1)});
    return r;
}

// full check of one (hull, p) with p strictly outside;
// dbl: also doubles, 1 = only unperturbed, 2 = also perturbed
void check(vt<P>& poly, P p, int dbl) {
    int n = size(poly);
    cntTests++;
    auto got = hull_tangents(poly, p);
    if (n < 3) { // outside precondition: only must not crash
        cntDeg++;
        if (n == 1) assert(got.f == pi(0, -1) && got.s == pi(0, -1));
        if (n == 1 || p.cross(poly[0], poly[1]) == 0) return;
    }
    auto exp = brute(poly, p);
    if (got != exp) fail(poly, p, got, exp, "exact");
    cnt[0] += exp.f.s < 0, cnt[1] += exp.f.s >= 0;
    cnt[2] += exp.s.s < 0, cnt[3] += exp.s.s >= 0;
    cntWrap += exp.f == pi{n - 1, 0} || exp.s == pi{n - 1, 0};
    if (!dbl) return;
    for (db pert : {0.0, 1e-12}) {
        if (pert > 0 && dbl < 2) break;
        vt<D> pd = toD(poly, pert);
        D q{(db) p.x + pert * (db) rnd(-1, 1), (db) p.y + pert * (db) rnd(-1, 1)};
        auto gd = hull_tangents_d(pd, q, 1e-9);
        if (gd != exp) fail(poly, p, gd, exp, pert ? "double perturbed" : "double");
    }
}

int main() {
    // 1. sampled subsets of the 4x4 grid, every p in [-3,6]^2 outside
    F0R (mask, 1 << 16) {
        if (__builtin_popcount(mask) >= 3 && rnd(0, 3)) continue;
        vt<P> pts;
        F0R (i, 16) if (mask >> i & 1) pts.pb({i % 4, i / 4});
        if (pts.empty()) continue;
        vt<P> poly = convex_hull(pts);
        FOR (x, -3, 7) FOR (y, -3, 7) {
            P p{x, y};
            if (outside(poly, p)) check(poly, p, 2);
        }
    }
    assert(cntDeg > 1000);

    // 2. random hulls
    ll tie[2] = {};
    F0R (it, 60000) {
        ll R = it % 4 == 0 ? 3 : it % 4 == 1 ? 8 : it % 4 == 2 ? 1000 : 300000000;
        int m = (int) rnd(3, 40);
        vt<P> pts(m);
        for (P& q : pts) q = {rnd(-R, R), rnd(-R, R)};
        vt<P> poly = convex_hull(pts);
        int n = size(poly);
        if (n < 3) continue;
        int dbl = R <= 8 ? 2 : R <= 1000;
        // random / far / near a vertex
        F0R (k, 6) {
            P p;
            if (k < 2) p = {rnd(-2 * R, 2 * R), rnd(-2 * R, 2 * R)};
            else if (k < 4) p = {rnd(-5 * R, 5 * R), rnd(-5 * R, 5 * R)};
            else p = poly[(int) rnd(0, n - 1)] + P{rnd(-2, 2), rnd(-2, 2)};
            if (outside(poly, p)) check(poly, p, dbl);
        }
        // on the extension line of a side, beyond either endpoint
        F0R (k, 4) {
            int i = (int) rnd(0, n - 1), j = (i + 1) % n;
            P e = poly[j] - poly[i];
            ll g = __gcd(llabs(e.x), llabs(e.y));
            e = e / g;
            ll t = rnd(1, R > 1000 ? 2 : 4);
            P p = k & 1 ? poly[j] + e * t : poly[i] - e * t;
            if (!outside(poly, p)) continue;
            auto exp = brute(poly, p);
            tie[0] += exp.f.s >= 0, tie[1] += exp.s.s >= 0;
            check(poly, p, dbl);
        }
    }
    assert(tie[0] > 5000 && tie[1] > 5000);

    // 3. big hull: tangency condition only
    {
        vt<P> pts;
        const int N = 100000;
        const db R = 1e8;
        F0R (i, N) {
            db t = 2 * M_PI * i / N;
            pts.pb({llround(R * cos(t)), llround(R * sin(t))});
        }
        vt<P> poly = convex_hull(pts);
        int n = size(poly);
        assert(n > 50000);
        F0R (it, 200) {
            P p;
            if (it % 4 == 3) {
                int i = (int) rnd(0, n - 1), j = (i + 1) % n;
                P e = poly[j] - poly[i];
                e = e / __gcd(llabs(e.x), llabs(e.y));
                p = it & 4 ? poly[j] + e : poly[i] - e;
            } else p = {rnd(-1000000000, 1000000000), rnd(-1000000000, 1000000000)};
            if (!outside(poly, p)) continue;
            cntBig++;
            auto got = hull_tangents(poly, p);
            for (int d : {1, -1}) {
                pi r = d > 0 ? got.f : got.s;
                assert(0 <= r.f && r.f < n);
                if (r.s >= 0) assert(r.s == (r.f + 1) % n);
                F0R (j, n) {
                    ll c = d * p.cross(poly[r.f], poly[j]);
                    if (j == r.f || j == r.s) assert(c == 0);
                    else assert(c < 0);
                }
            }
        }
        assert(cntBig > 100);
    }

    cout << "checked " << cntTests << " (hull, p) pairs, " << cntDeg
         << " degenerate, " << cntBig << " on the big hull\n";
    cout << "a single " << cnt[0] << ", a side " << cnt[1] << ", b single "
         << cnt[2] << ", b side " << cnt[3] << ", wrap sides " << cntWrap
         << ", extension-line ties a " << tie[0] << " b " << tie[1] << "\n";
    cout << "Tests passed!" << endl;
}
