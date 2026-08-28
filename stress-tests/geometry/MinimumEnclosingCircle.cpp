// Tests MinimumEnclosingCircle.h: mec() vs brute force (smallest of all
// pair-diameter / triple-circumcircle candidates enclosing every point) on random
// small-integer sets (duplicates, collinear triples), fully collinear sets and
// real-valued sets; also checks the farthest point lies on the returned circle
// and two hand-computed cases. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/MinimumEnclosingCircle.h"

int main() {
    mt19937 rng(7);
    auto check = [&] (const vt<P>& ps) {
        auto [o, r] = mec(ps);
        db tol = 1e-8 * (1 + r), far = 0;
        for (P p : ps) far = max(far, (p - o).dist());
        assert(abs(far - r) < tol);
        int n = size(ps);
        db best = n == 1 ? 0 : 1e18;
        auto cand = [&] (P c, db rad) {
            for (P p : ps) if ((p - c).dist() > rad + tol) return;
            best = min(best, rad);
        };
        F0R(i, n) F0R(j, i) {
            cand((ps[i] + ps[j]) / 2, (ps[i] - ps[j]).dist() / 2);
            F0R(k, j) if (ps[i].cross(ps[j], ps[k]) != 0)
                cand(cc_center(ps[i], ps[j], ps[k]), cc_radius(ps[i], ps[j], ps[k]));
        }
        assert(abs(best - r) < tol);
    };
    {   // known values: diameter case and right triangle
        auto [o, r] = mec({P{0, 0}, P{2, 0}, P{1, 1}});
        assert((o - P{1, 0}).dist() < 1e-9 && abs(r - 1) < 1e-9);
        auto [o2, r2] = mec({P{0, 0}, P{4, 0}, P{0, 3}});
        assert((o2 - P{2, 1.5}).dist() < 1e-9 && abs(r2 - 2.5) < 1e-9);
    }
    auto ri = [&] (int lo, int hi) { return (db) (int) (rng() % (unsigned) (hi - lo + 1)) + lo; };
    F0R(it, 20000) {
        int n = (int) (rng() % 9) + 1;
        vt<P> ps;
        F0R(i, n) ps.pb(P{ri(-5, 5), ri(-5, 5)});
        check(ps);
    }
    uniform_real_distribution<db> real(-1e3, 1e3);
    F0R(it, 5000) {
        int n = (int) (rng() % 8) + 1;
        vt<P> ps;
        P d{ri(-3, 3), ri(-3, 3)};
        F0R(i, n) ps.pb(d * ri(-10, 10));
        check(ps);
        ps.clear();
        F0R(i, n) ps.pb(P{real(rng), real(rng)});
        check(ps);
    }
    cout << "Tests passed!" << endl;
}
