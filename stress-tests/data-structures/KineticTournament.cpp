// Tests KineticTournament.h: seg[1] = argmin p.dot(t) as t sweeps CCW
// through a window under 180 degrees, vs a brute-force min at every step.
// Directions are exact integers inside the window sorted by cross, so the
// sweep is monotone. Covers mid-sweep upd, duplicate and near-collinear
// points, |coord| and |t| up to 1e9 under UBSan (cross reaches 8e18),
// sweeps that end exactly at tn, a tiny-coordinate run where exact ties
// at the certificate direction are common, and the lines mapping.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/data-structures/KineticTournament.h"

mt19937 rng(5);
ll rnd(ll lo, ll hi) { return lo + (ll)(rng() % (unsigned long long)(hi - lo + 1)); }
bool lt(P a, P b) { return a.cross(b) > 0; }

void run(P t0, P tn, ll C, ll D, int iters) {
    F0R (iter, iters) {
        int cnt = (int) rnd(1, 12);
        auto rp = [&] { return P{rnd(-C, C), rnd(-C, C)}; };
        vt<P> a(cnt); for (P &p : a) p = rp();
        if (rng() % 3 == 0) { P b = rp(); for (P &p : a) if (rng() % 2) p = b; }
        if (rng() % 3 == 0) for (P &p : a) p = {p.x, rnd(-1, 1)};
        vt<P> ds;
        F0R (i, 14) { P d{rnd(-D, D), rnd(-D, D)};
            if ((d.x || d.y) && lt(t0, d) && lt(d, tn)) ds.pb(d); }
        if (rng() % 4 == 0) ds.pb(tn); // end exactly at tn
        sort(all(ds), lt);
        vt<P> mono; for (P d : ds) if (mono.empty() || lt(mono.back(), d)) mono.pb(d);
        KineticTournament kt(a, t0, tn);
        for (P d : mono) {
            if (rng() % 3 == 0) { int i = rng() % cnt; P np = rp(); a[i] = np; kt.upd(i, np); }
            kt.heaten(d);
            ll want = LLONG_MAX; for (P p : a) want = min(want, p.dot(d));
            assert(kt.seg[1].dot(d) == want);
        }
    }
}
int main() {
    run({2, -1}, {-1, 2}, 1000, 100, 100000);        // general window
    run({2, -1}, {-1, 2}, 3, 3, 100000);             // tiny: exact ties galore
    run({-1000000, -1}, {1000000, -1}, 1000000, 1000000, 40000); // lower half
    run({2, -1000000000}, {-1, 1000000000}, 1000000000, 1000000000, 40000); // 1e9
    // min a*x + b over lines at increasing x: point (a, -b), t = (x, -1)
    F0R (iter, 40000) {
        int n = (int) rnd(1, 8); vt<ll> A(n), B(n); vt<P> pts(n);
        F0R (i, n) A[i] = rnd(-1000, 1000), B[i] = rnd(-1000, 1000), pts[i] = {A[i], -B[i]};
        ll BIG = 1000000000; KineticTournament kt(pts, {-BIG, -1}, {BIG, -1});
        ll x = -BIG + 1;
        F0R (j, rnd(1, 10)) {
            x += rnd(0, 50); kt.heaten({x, -1});
            ll want = LLONG_MAX; F0R (i, n) want = min(want, A[i] * x + B[i]);
            assert(kt.seg[1].dot(P{x, -1}) == want);
        }
    }
    cout << "Tests passed!" << endl;
}
