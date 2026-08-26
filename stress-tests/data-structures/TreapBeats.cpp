// Tests TreapBeats.h: range chmin/chmax/add + sum/min/max queries vs brute,
// n=1000, values near +-9e8 so sums exceed int. written by Claude (audit)
#include "../utilities/template.h"
#undef sz // header defines int sz(ptr); the legacy shim macro would mangle it
#include "../../content/data-structures/TreapBeats.h"

// no merge/split used, so heap priorities don't matter: build balanced
ptr build(vi& v, int l, int r) {
    if (l >= r) return nullptr;
    int m = (l + r) / 2;
    ptr n = new Node(Value::make(v[m]));
    n->l = build(v, l, m);
    n->r = build(v, m + 1, r);
    return pull(n);
}

int main() {
    mt19937 rng(99);
    auto rv = [&] { return (int) (rng() % 1800000001) - 900000000; };
    int n = 1000;
    vi v(n);
    for (int& x : v) x = rv();
    ptr root = build(v, 0, n);
    F0R (it, 5000) {
        int lo = rng() % n, hi = rng() % n;
        if (lo > hi) swap(lo, hi);
        hi++;
        int t = rng() % 4;
        if (t == 0) { // query
            Value q = queryi(root, lo, hi);
            ll s = 0; int mx = -inf, mn = inf;
            FOR (k, lo, hi) s += v[k], mx = max(mx, v[k]), mn = min(mn, v[k]);
            assert(q.sum == s && q.mx == mx && q.mn == mn);
        } else if (t == 1) {
            int x = rv();
            updi(root, lo, hi, chmin_tag(x));
            FOR (k, lo, hi) v[k] = min(v[k], x);
        } else if (t == 2) {
            int x = rv();
            updi(root, lo, hi, chmax_tag(x));
            FOR (k, lo, hi) v[k] = max(v[k], x);
        } else {
            int x = (int) (rng() % 2001) - 1000;
            updi(root, lo, hi, add_tag(x));
            FOR (k, lo, hi) v[k] += x;
        }
    }
    Value q = queryi(root, 0, n);
    ll s = 0;
    for (int x : v) s += x;
    assert(q.sum == s);
    cout << "Tests passed!" << endl;
}
