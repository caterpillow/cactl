// Tests cyc_peak / cyc_max (CyclicMax.h) against a brute-force argmax with the
// (i, i+1 mod n) tie pair: exhaustively on every valid sequence of length <= 8
// over {0..3}, on random cyclically unimodal ll / pair<int,int> / noisy double
// sequences (n up to 1e5, ties at max and min, random rotation), argmin via
// the negated comparator, comparator-call counts, and against extrVertex and
// hull_tangents on random strictly convex integer hulls. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/various/CyclicMax.h" // before LHI.h's cmp macro
#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/LineHullIntersection.h"
#include "../../content/geometry/HullTangents.h"

mt19937 rng(2026);
ll rnd(ll a, ll b) { return uniform_int_distribution<ll>(a, b)(rng); }
ll calls = 0, maxCalls = 0;

// is a cyclically unimodal (ties only at max/min, adjacent, never three)?
bool valid(const vl& a) {
    int n = size(a);
    if (n == 1) return true;
    ll mn = *min_element(all(a));
    int r = -1;
    F0R (i, n) if (a[i] == mn && a[(i + 1) % n] != mn) r = i;
    if (r < 0) return n == 2; // all equal
    vl b(n);
    F0R (i, n) b[i] = a[(r + i) % n];
    int p = int(max_element(all(b)) - b.begin());
    F0R (i, p) if (b[i] >= b[i + 1]) return false;
    int q = p + 1 < n && b[p + 1] == b[p] ? p + 1 : p;
    FOR (i, q, n - 1) if (b[i] <= b[i + 1]) return false;
    return true;
}
pi brute(const vl& a) { // argmax with the tie pair
    int n = size(a);
    ll mx = *max_element(all(a));
    vi at;
    F0R (i, n) if (a[i] == mx) at.pb(i);
    if (n == 1) return {0, -1};
    if (size(at) == 1) return {at[0], -1};
    assert(size(at) == 2);
    if (at[1] == at[0] + 1) return {at[0], at[1]};
    assert(at[0] == 0 && at[1] == n - 1);
    return {n - 1, 0};
}
void fail(const vl& a, pi got, pi exp, const char* what) {
    cout << what << ": got (" << got.f << ", " << got.s << ") expected ("
         << exp.f << ", " << exp.s << ")\nseq:";
    for (ll x : a) cout << ' ' << x;
    cout << '\n';
    exit(1);
}
// run all exact variants on a valid sequence
void check(const vl& a) {
    int n = size(a);
    pi exp = brute(a);
    auto fn = [&](int i) { calls++; return a[i]; };
    pi got = cyc_max(n, fn);
    if (got != exp) fail(a, got, exp, "cyc_max");
    auto cm = [&](int i, int j) {
        calls++; return (a[j] < a[i]) - (a[i] < a[j]); };
    got = cyc_peak(n, cm);
    if (got != exp) fail(a, got, exp, "cyc_peak");
    vl neg(n);
    F0R (i, n) neg[i] = -a[i];
    exp = brute(neg);
    got = cyc_peak(n, [&](int i, int j) { return -cm(i, j); });
    if (got != exp) fail(a, got, exp, "argmin via -cmp");
    ll big = max(-*min_element(all(a)), *max_element(all(a)));
    if (big > (ll) 1e12) return; // pair<int,int> below needs |a| < 2e12
    vt<pi> pr(n); // pair<int,int> values, order-preserving
    F0R (i, n) {
        ll q = a[i] >= 0 ? a[i] / 1000 : -((-a[i] + 999) / 1000);
        pr[i] = {(int) q, (int) (a[i] - q * 1000)};
    }
    exp = brute(a);
    got = cyc_max(n, [&](int i) { return pr[i]; });
    if (got != exp) fail(a, got, exp, "cyc_max pair");
}
// random cyclically unimodal sequence of length n, values in [-R, R]
vl gen(int n, ll R) {
    while (true) {
        int tmax = (int) rnd(0, 1), tmin = (int) rnd(0, 1);
        int L = n - tmax - tmin;
        if (L < 1 || (L == 1 && tmax + tmin > 1)) continue;
        int p = (int) rnd(1, L); // ascending part incl. min and max
        set<ll> s;
        while (size(s) < p) s.insert(rnd(-R, R));
        vl asc(all(s)), a = asc;
        if (tmax) a.pb(asc.back());
        ll lo = asc.front(), hi = asc.back();
        if (L > p && hi - lo - 1 < L - p) continue;
        set<ll> d;
        while (size(d) < L - p) d.insert(rnd(lo + 1, hi - 1));
        for (auto it = d.rbegin(); it != d.rend(); ++it) a.pb(*it);
        if (tmin) a.pb(lo);
        int r = (int) rnd(0, n - 1);
        rotate(a.begin(), a.begin() + r, a.end());
        assert(size(a) == n && valid(a));
        return a;
    }
}

int main() {
    // 1. exhaustive
    ll cnt = 0;
    FOR (n, 1, 9) F0R (mask, 1 << (2 * n)) {
        vl a(n);
        F0R (i, n) a[i] = mask >> (2 * i) & 3;
        if (!valid(a)) continue;
        cnt++, check(a);
    }
    assert(cnt > 300); // 4 distinct values bound the lengths
    // 2, 3, 6. random ll sequences, call counts on the big ones
    F0R (it, 20000) {
        int n = (int) rnd(1, 60);
        ll R = it % 3 == 1 ? (ll) 1e12 : (ll) 4e18;
        if (it % 3 == 0) R = 5, n = (int) rnd(1, 8); // few distinct values
        vl a = gen(n, R);
        check(a);
    }
    F0R (it, 60) {
        int n = (int) rnd(90000, 110000);
        vl a = gen(n, (ll) 4e18);
        pi exp = brute(a);
        calls = 0;
        pi got = cyc_peak(n, [&](int i, int j) {
            calls++; return (a[j] < a[i]) - (a[i] < a[j]); });
        if (got != exp) fail(a, got, exp, "cyc_peak big");
        maxCalls = max(maxCalls, calls);
        int lg = 0;
        while ((1 << lg) < n) lg++;
        assert(calls <= 5 * lg);
    }
    // 4. doubles with noise
    F0R (it, 20000) {
        int n = (int) rnd(1, 60);
        vl a = gen(n, 1000000);
        pi exp = brute(a);
        vt<db> d(n);
        F0R (i, n) d[i] = (db) a[i] * 1e-6 + (db) rnd(-100, 100) * 1e-12
            + (rnd(0, 1) ? 1e-12 : 0);
        pi got = cyc_max(n, [&](int i) { return d[i]; }, 1e-9);
        if (got != exp) fail(a, got, exp, "cyc_max eps");
    }
    // 5. geometry: extrVertex and hull_tangents
    F0R (it, 30000) {
        int R = it % 2 ? 1000 : 6, m = (int) rnd(3, 30);
        vt<P> pts(m);
        for (P& p : pts) p = {rnd(-R, R), rnd(-R, R)};
        vt<P> poly = convex_hull(pts);
        int n = size(poly);
        if (n < 3) continue;
        P dir{rnd(-R, R), rnd(-R, R)};
        if (dir == P{0, 0}) continue;
        pi got = cyc_max(n, [&](int i) { return poly[i].dot(dir); });
        int ev = extrVertex(poly, dir);
        if (got.f != ev) {
            cout << "extrVertex " << ev << " vs (" << got.f << ", " << got.s
                 << ") dir " << dir << "\nhull:";
            for (P p : poly) cout << ' ' << p;
            cout << '\n';
            exit(1);
        }
        P p{rnd(-2 * R, 2 * R), rnd(-2 * R, 2 * R)};
        bool out = false;
        F0R (i, n) out |= poly[i].cross(poly[(i + 1) % n], p) < 0;
        if (!out) continue;
        // tang(i) in HullTangents.h is peak(i) for cmp(i, j) = ang(j, i)
        auto cm = [&](int i, int j) { return sgn(p.cross(poly[j], poly[i])); };
        pair<pi, pi> t = hull_tangents(poly, p);
        pi a = cyc_peak(n, cm);
        pi b = cyc_peak(n, [&](int i, int j) { return -cm(i, j); });
        if (a != t.f || b != t.s) {
            cout << "hull_tangents (" << t.f.f << "," << t.f.s << ") ("
                 << t.s.f << "," << t.s.s << ") vs (" << a.f << "," << a.s
                 << ") (" << b.f << "," << b.s << ") p " << p << "\nhull:";
            for (P q : poly) cout << ' ' << q;
            cout << '\n';
            exit(1);
        }
    }
    cout << "exhaustive cases " << cnt << ", max comparator calls at n ~ 1e5: " << maxCalls << "\n";
    cout << "Tests passed!" << endl;
}
