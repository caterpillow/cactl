// Tests ClosestPair.h: closest() vs O(n^2) brute force on random integer point
// sets (small coords with duplicates, huge coords near ll limits), also checks
// the returned pair consists of two distinct input points. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ClosestPair.h"

int main() {
    mt19937 rng(1234);
    auto rnd = [&] (ll lo, ll hi) { return (ll) (rng() % (unsigned long long) (hi - lo + 1)) + lo; };
    auto check = [&] (int n, ll lim) {
        vt<P> ps;
        F0R(i, n) ps.pb(P{rnd(-lim, lim), rnd(-lim, lim)});
        ll best = LLONG_MAX;
        F0R(i, n) FOR(j, i + 1, n) best = min(best, (ps[i] - ps[j]).dist2());
        auto [a, b] = closest(ps);
        int ca = 0, cb = 0;
        for (P p : ps) ca += p == a, cb += p == b;
        assert(ca >= 1 && cb >= 1 && (!(a == b) || ca >= 2)); // real input points
        assert((a - b).dist2() == best);
    };
    F0R(it, 200000) check((int) rnd(2, 15), rnd(1, 20));
    F0R(it, 20000) check((int) rnd(2, 60), rnd(1, 1000000000LL));
    F0R(it, 20) check(2000, 1000000000LL);
    F0R(it, 20) check(2000, 30);
    cout << "Tests passed!" << endl;
}
