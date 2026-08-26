// Tests LazySegtree.h: random range add / range assign + max&sum queries vs
// brute on n=64, values up to 1e9. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/LazySegtree.h"

int main() {
    mt19937 rng(11);
    int n = 64;
    vl v(n);
    Lazyseg tr;
    F0R (i, n) {
        v[i] = (int) (rng() % 2000000001) - 1000000000;
        tr.upd(i, i + 1, {v[i], false}); // initialize via range assign
    }
    F0R (it, 20000) {
        int lo = rng() % n, hi = rng() % n;
        if (lo > hi) swap(lo, hi);
        hi++;
        int t = rng() % 3;
        ll x = (ll) (rng() % 2000000001) - 1000000000;
        if (t == 0) {
            tr.upd(lo, hi, {x, true});
            FOR (k, lo, hi) v[k] += x;
        } else if (t == 1) {
            tr.upd(lo, hi, {x, false});
            FOR (k, lo, hi) v[k] = x;
        } else {
            Node q = tr.query(lo, hi);
            ll mx = -INF, s = 0;
            FOR (k, lo, hi) mx = max(mx, v[k]), s += v[k];
            assert(q.mx == mx && q.sum == s);
        }
    }
    cout << "Tests passed!" << endl;
}
