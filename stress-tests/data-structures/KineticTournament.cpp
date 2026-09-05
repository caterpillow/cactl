// Tests KineticTournament.h: max p.dot(t) as t sweeps CCW through a half
// plane, with point updates. Directions are exact integers in the strict
// right half-plane (x>=1), sorted by the angular <, so the sweep is
// monotone by construction. Compares to a brute max at every step;
// covers duplicates, near-collinear points, big coords, and mid-sweep upd.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/data-structures/KineticTournament.h"

int main() {
    mt19937 rng(7);
    F0R (iter, 120000) {
        int n = rng() % 10 + 1;
        ll C = rng() % 3 ? 1000 : 1000000;
        auto rp = [&] { return P{(ll)(rng() % (2 * C + 1) - C),
                                 (ll)(rng() % (2 * C + 1) - C)}; };
        vt<P> a(n); for (P &p : a) p = rp();
        if (rng() % 3 == 0) { P b = rp(); for (P &p : a) if (rng() % 2) p = b; }
        if (rng() % 3 == 0) for (P &p : a) p = {p.x, (ll)(rng() % 3 - 1)};
        int k = rng() % 10 + 1; vt<P> ds;
        F0R (i, k) ds.pb({(ll)(rng() % 50 + 1), (ll)(rng() % 101 - 50)});
        sort(all(ds), [] (P u, P w) { return u < w; });
        vt<P> mono; for (P d : ds) if (mono.empty() || mono.back() < d) mono.pb(d);
        KineticTournament kt(a, {1, -1000000000LL}, {0, 1});
        for (P d : mono) {
            if (rng() % 3 == 0) { int i = rng() % n; P np = rp(); a[i] = np; kt.upd(i, np); }
            kt.heaten(d);
            ll got = kt.query(), want = LLONG_MIN;
            for (P p : a) want = max(want, p.dot(d));
            assert(got == want);
        }
    }
    // lines a*x + b at increasing x: point (a, -b), dir (x, -1)
    F0R (iter, 60000) {
        int n = rng() % 8 + 1; vt<ll> A(n), B(n); vt<P> pts(n);
        F0R (i, n) A[i] = rng() % 2001 - 1000, B[i] = rng() % 2001 - 1000,
                   pts[i] = {A[i], -B[i]};
        ll BIG = 1000000000LL;
        KineticTournament kt(pts, {-BIG, -1}, {BIG, -1});
        ll x = -BIG + 1;
        F0R (j, rng() % 10 + 1) {
            x += rng() % 50; kt.heaten({x, -1});
            ll got = kt.query(), want = LLONG_MIN;
            F0R (i, n) want = max(want, A[i] * x + B[i]);
            assert(got == want);
        }
    }
    cout << "Tests passed!" << endl;
}
