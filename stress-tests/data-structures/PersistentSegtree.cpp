// Tests PersistentSegtree.h: build initial full tree manually, make 200 versions
// via point updates, range-min query every version vs brute snapshots.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/PersistentSegtree.h"

const int M = 256; // active prefix; rest stays inf
ptr build(int l = 0, int r = sz) {
    if (r - l == 1) return new Node {inf};
    int m = (l + r) / 2;
    ptr lc = build(l, m), rc = build(m, r);
    return new Node {min(lc->v, rc->v), lc, rc};
}

int main() {
    mt19937 rng(7);
    vt<ptr> roots = {build()};
    vt<vi> snaps = {vi(M, inf)};
    F0R (it, 200) {
        int i = rng() % M, v = (int) (rng() % 2000000001) - 1000000000;
        roots.pb(roots.back()->upd(i, v));
        snaps.pb(snaps.back());
        snaps.back()[i] = v;
    }
    F0R (q, 5000) {
        int ver = rng() % size(roots);
        int lo = rng() % (2 * M), hi = rng() % (2 * M);
        if (lo > hi) swap(lo, hi);
        hi++;
        int want = inf;
        FOR (k, lo, min(hi, M)) want = min(want, snaps[ver][k]);
        assert(roots[ver]->query(lo, hi) == want);
    }
    cout << "Tests passed!" << endl;
}
