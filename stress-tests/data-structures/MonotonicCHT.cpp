// Tests MonotonicCHT.h: max m*x + c with slopes added non-decreasing and
// queries at non-decreasing x, interleaved, vs a brute max over all lines.
// Covers equal slopes, large coords, and a run with only queries after a
// batch of adds. written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/data-structures/MonotonicCHT.h"

int main() {
    mt19937 rng(3);
    F0R (iter, 300000) {
        MonotonicCHT cht; vt<pl> lines;
        ll m = -1000, x = -100000; bool any = false;
        int n = rng() % 12 + 1;
        F0R (step, n) {
            if (rng() % 2 || !any) {
                m += rng() % 50; ll c = (ll)(rng() % 2001) - 1000;
                cht.add(m, c); lines.pb({m, c}); any = true;
            } else {
                x += rng() % 40; ll got = cht.query(x), want = LLONG_MIN;
                for (auto &L : lines) want = max(want, L.f * x + L.s);
                assert(got == want);
            }
        }
    }
    // big coords: bad() must not overflow (uses __int128)
    mt19937 r(9);
    F0R (iter, 60000) {
        MonotonicCHT cht; vt<pl> lines;
        ll m = -1000000000LL, x = -1000000000LL; bool any = false;
        F0R (step, (int)(rng() % 8 + 2)) {
            if (r() % 2 || !any) {
                m += r() % 1000000;
                ll c = (ll)(r() % 2000000001LL) - 1000000000LL;
                cht.add(m, c); lines.pb({m, c}); any = true;
            } else {
                x += r() % 1000000; ll got = cht.query(x), want = LLONG_MIN;
                for (auto &L : lines) want = max(want, L.f * x + L.s);
                assert(got == want);
            }
        }
    }
    cout << "Tests passed!" << endl;
}
