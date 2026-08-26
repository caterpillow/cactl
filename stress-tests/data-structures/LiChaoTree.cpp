// Tests LiChaoTree.h: empty query returns INF; random lines |m|,|c|<=1e9,
// min at x in [0,2^30) vs brute (values far above 2^30). written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/LiChaoTree.h"

int main() {
    mt19937_64 rng(123);
    assert(query(nullptr, 5) == INF); // empty tree
    F0R (round, 60) {
        ptr root = nullptr;
        vt<Line> lines;
        assert(query(root, rng() % sz) == INF);
        int L = 1 + rng() % 200;
        F0R (i, L) {
            Line ln {(ll) (rng() % 2000000001) - 1000000000, (ll) (rng() % 2000000001) - 1000000000};
            lines.pb(ln);
            add(root, ln);
            F0R (q, 20) {
                ll x = rng() % sz;
                if (q < 3) x = sz - 1 - q; // near top of domain
                ll best = INF;
                for (auto& l2 : lines) best = min(best, l2(x));
                assert(query(root, x) == best);
            }
        }
        delete root; // leak is fine but keep it tidy-ish (shallow)
    }
    cout << "Tests passed!" << endl;
}
