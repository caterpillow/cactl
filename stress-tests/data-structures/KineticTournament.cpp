// Tests KineticTournament.h: seg[1] = argmin p.dot(t) as t sweeps CCW
// through a <180 window, with point updates (including mid-sweep). Exact
// integer directions inside the window, sorted by the angular <, so the
// sweep is monotone. Covers two windows (a general one and a lower-half
// one), duplicates, near-collinear points, and large coords; vs brute min.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/data-structures/KineticTournament.h"

vt<P> dirs(mt19937 &rng, P t0, P tn, int k) {
    vt<P> ds;
    F0R (i, k) {
        ll x = (ll)(rng() % 201) - 100, y = (ll)(rng() % 201) - 100;
        P d{x, y};
        if ((x || y) && t0.cross(d) > 0 && d.cross(tn) > 0) ds.pb(d);
    }
    sort(all(ds), [] (P u, P w) { return u < w; });
    vt<P> m; for (P d : ds) if (m.empty() || m.back() < d) m.pb(d);
    return m;
}
int main() {
    mt19937 rng(5);
    auto run = [&] (P t0, P tn, P pad, ll C, int iters) {
        F0R (iter, iters) {
            int cnt = rng() % 12 + 1;
            auto rp = [&] { return P{(ll)(rng() % (2*C+1) - C),
                                     (ll)(rng() % (2*C+1) - C)}; };
            vt<P> a(cnt); for (P &p : a) p = rp();
            if (rng() % 3 == 0) { P b = rp(); for (P &p : a) if (rng()%2) p = b; }
            if (rng() % 3 == 0) for (P &p : a) p = {p.x, (ll)(rng() % 3 - 1)};
            auto mono = dirs(rng, t0, tn, 14); if (mono.empty()) continue;
            KineticTournament kt(cnt, t0, tn, pad);
            F0R (i, cnt) kt.upd(i, a[i]);
            for (P d : mono) {
                if (rng() % 3 == 0) { int i = rng()%cnt; P np = rp(); a[i]=np; kt.upd(i,np); }
                kt.heaten(d);
                ll got = kt.seg[1].dot(d), want = LLONG_MAX;
                for (P p : a) want = min(want, p.dot(d));
                assert(got == want);
            }
        }
    };
    run({2, -1}, {-1, 2}, {1000000000000LL, 1000000000000LL}, 1000, 120000);
    run({2, -1}, {-1, 2}, {1000000000000LL, 1000000000000LL}, 1000000, 60000);
    run({-1000000, -1}, {1000000, -1}, {0, -1000000000000LL}, 1000000, 60000);
    cout << "Tests passed!" << endl;
}
