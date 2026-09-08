// Tests MonotonicCHT.h: max m*x + c with slopes added non-decreasing and
// queries at non-decreasing x, interleaved at random, vs a brute max over
// all lines. Slopes come from tiny ranges so runs of equal slopes in every
// order (better, worse, duplicate) are common; the two hand-picked
// sequences broke earlier drafts. Plus a 1e9-coordinate run.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/radial_point.h"

#include "../../content/data-structures/MonotonicCHT.h"

mt19937 rng(3);
ll rnd(ll lo, ll hi) { return lo + (ll)(rng() % (unsigned long long)(hi - lo + 1)); }

void run(ll dm, ll C, ll dx, int steps, int iters) {
    F0R (iter, iters) {
        MonotonicCHT cht; vt<pl> lines;
        ll m = -C, x = -C;
        F0R (step, steps) {
            if (lines.empty() || rng() % 2) {
                m += rnd(0, dm); ll c = rnd(-C, C);
                cht.add(m, c), lines.pb({m, c});
            } else {
                x += rnd(0, dx); ll want = LLONG_MIN;
                for (auto [a, b] : lines) want = max(want, a * x + b);
                assert(cht.query(x) == want);
            }
        }
    }
}
int main() {
    for (auto [m, c] : vt<pl>{{1, 5}, {1, 0}, {1, 3}}) { static MonotonicCHT s; s.add(m, c);
        assert(s.query(0) == 5); }
    MonotonicCHT s2; for (auto [m, c] : vt<pl>{{1, 5}, {1, 0}, {1, 3}, {2, 0}}) s2.add(m, c);
    assert(s2.query(0) == 5 && s2.query(10) == 20);
    MonotonicCHT s3; for (auto [m, c] : vt<pl>{{1, 0}, {1, 5}, {1, 3}}) s3.add(m, c);
    assert(s3.query(0) == 5 && s3.query(-7) == -2);
    run(1, 3, 3, 12, 200000);            // slopes in a tiny range: equal slopes galore
    run(2, 20, 5, 16, 100000);
    run(1000000, 1000000000, 1000000, 10, 40000); // big coords
    cout << "Tests passed!" << endl;
}
