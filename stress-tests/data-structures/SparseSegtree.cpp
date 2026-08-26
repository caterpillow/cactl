// Tests SparseSegtree.h: point assign / range min over [0, 2^30) vs map brute,
// mixing small and huge indices. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/SparseSegtree.h"

int main() {
    mt19937 rng(42);
    ptr root = new Node {INF};
    map<int, ll> brute;
    F0R (it, 4000) {
        int i = rng() % 2 ? rng() % 100 : rng() % sz;
        ll v = (int) rng();
        root->upd(i, v);
        brute[i] = v;
        int lo = rng() % sz, hi = rng() % sz;
        if (lo > hi) swap(lo, hi);
        if (rng() % 2) lo = 0, hi = rng() % 120; // hit dense region often
        ll want = INF;
        for (auto it2 = brute.lower_bound(lo); it2 != brute.end() && it2->f < hi; ++it2)
            want = min(want, it2->s);
        assert(root->query(lo, hi) == want);
    }
    cout << "Tests passed!" << endl;
}
