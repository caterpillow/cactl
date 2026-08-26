// Tests 2sat.h vs brute force over 2^n assignments: solve() nonempty iff
// satisfiable, returned assignment satisfies all clauses (either, implies,
// force, exactly_one, tie, nand, at_most_one). Includes force(0) regression.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/graph/2sat.h"

mt19937 rng(9001);

bool ev(int lit, int mask) { // literal value under assignment mask
    return lit >= 0 ? mask >> lit & 1 : !(mask >> ~lit & 1);
}

int main() {
    { // force(0) single-variable regression + contradiction
        TwoSAT a{1}; a.force(0);
        assert(a.solve() == vt<bool>{true});
        TwoSAT b{1}; b.force(~0);
        assert(b.solve() == vt<bool>{false});
        TwoSAT c{1}; c.force(0); c.force(~0);
        assert(c.solve().empty());
    }
    F0R (it, 200000) {
        int n = rng() % 5 + 1, m = rng() % 9;
        TwoSAT ts{n};
        vt<pi> ors;   // brute model: disjunctions of two literals
        vt<vi> amos;  // brute model: at-most-one lists
        auto lit = [&] { int v = rng() % n; return rng() % 2 ? v : ~v; };
        F0R (j, m) {
            int x = lit(), y = lit(), t = rng() % 10;
            if (t < 4) ts.either(x, y), ors.pb({x, y});
            else if (t < 5) ts.implies(x, y), ors.pb({~x, y});
            else if (t < 6) ts.force(x), ors.pb({x, x});
            else if (t < 7) ts.exactly_one(x, y), ors.pb({x, y}), ors.pb({~x, ~y});
            else if (t < 8) ts.nand(x, y), ors.pb({~x, ~y});
            else if (t < 9) ts.tie(x, y), ors.pb({~x, y}), ors.pb({x, ~y});
            else {
                vi li(rng() % 5);
                for (int& l : li) l = lit();
                ts.at_most_one(li);
                amos.pb(li);
            }
        }
        auto ok = [&](int mask) {
            for (auto& [x, y] : ors) if (!ev(x, mask) && !ev(y, mask)) return false;
            for (auto& li : amos) {
                int cnt = 0;
                for (int l : li) cnt += ev(l, mask);
                if (cnt > 1) return false;
            }
            return true;
        };
        bool sat = false;
        F0R (mask, 1 << n) sat |= ok(mask);
        vt<bool> ans = ts.solve();
        assert(sat == !ans.empty());
        if (sat) {
            int mask = 0;
            F0R (i, n) mask |= ans[i] << i;
            assert(ok(mask));
        }
    }
    cout << "Tests passed!" << endl;
}
