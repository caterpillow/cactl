// Tests PersistentLazySegtree.h: range add / range assign applied to random
// old versions (forking), max&sum queries on random versions vs brute
// snapshots. Cells [W, sz) are only touched by full-range updates, so they
// share one value. Also checks the node bound per upd and build().
#include "../utilities/template.h"
#include "../../content/data-structures/PersistentLazySegtree.h"

ptr build(vt<Node> &a, int l = 0, int r = sz) {
    if (r - l == 1) return new Seg{a[l], lid};
    int m = (l + r) / 2;
    ptr x = build(a, l, m), y = build(a, m, r);
    return new Seg{x->v + y->v, lid, x, y};
}

ll allocs = 0;
void *operator new(size_t n) { allocs++; return malloc(n); }
void operator delete(void *p) noexcept { free(p); }
void operator delete(void *p, size_t) noexcept { free(p); }

const int W = 96;
struct Snap { vl a; ll rest; };

Node brute(const Snap &s, int lo, int hi) {
    Node r = nid;
    FOR (k, lo, min(hi, W)) r = r + Node{s.a[k], s.a[k]};
    if (hi > W) r = r + Node{s.rest, s.rest * (hi - max(lo, W))};
    return r;
}

int main() {
    mt19937 rng(23);
    ptr root = new Seg{{0, 0}, lid};
    root->lc = root->rc = root;
    vt<ptr> roots = {root};
    vt<Snap> snaps = {{vl(W), 0}};
    ll worst = 0;
    auto rnd = [&](int a, int b) { return a + (int) (rng() % (b - a + 1)); };
    F0R (it, 30000) {
        int ver = rng() % size(roots);
        int t = rng() % 5;
        int lo, hi;
        if (rng() % 20 == 0) lo = rnd(0, W), hi = sz; // touches [W, sz)
        else {
            lo = rnd(0, W - 1), hi = rnd(0, W - 1);
            if (lo > hi) swap(lo, hi);
            hi++;
        }
        if (t <= 1) {
            ll x = rnd(-1000000, 1000000);
            bool inc = t == 0;
            if (hi == sz && lo > 0 && rng() % 2) lo = 0;
            ll before = allocs;
            roots.pb(roots[ver]->upd(lo, hi, {x, inc}));
            worst = max(worst, allocs - before);
            Snap s = snaps[ver];
            FOR (k, lo, min(hi, W)) s.a[k] = inc ? s.a[k] + x : x;
            if (hi == sz) s.rest = inc ? s.rest + x : x; // lo <= W
            snaps.pb(s);
        } else {
            ll before = allocs;
            Node q = roots[ver]->query(lo, hi);
            assert(allocs == before); // query allocates nothing
            Node w = brute(snaps[ver], lo, hi);
            assert(q.mx == w.mx && q.sum == w.sum);
        }
    }
    // full check of every version at the end (old versions unchanged)
    F0R (ver, size(roots)) F0R (k, W) {
        Node q = roots[ver]->query(k, k + 1);
        assert(q.sum == snaps[ver].a[k]);
    }
    int lg = __lg(sz);
    assert(worst <= 4 * lg);
    // build(): random leaves, then ops on top of it
    vt<Node> a(sz);
    vl v(sz);
    F0R (i, sz) v[i] = rnd(-1000, 1000), a[i] = {v[i], v[i]};
    ptr b = build(a);
    F0R (it, 2000) {
        int lo = rnd(0, sz - 1), hi = rnd(0, sz - 1);
        if (lo > hi) swap(lo, hi);
        hi++;
        if (it % 2) {
            ll x = rnd(-1000, 1000);
            bool inc = rng() % 2;
            b = b->upd(lo, hi, {x, inc});
            FOR (k, lo, hi) v[k] = inc ? v[k] + x : x;
        } else {
            Node q = b->query(lo, hi);
            ll mx = -INF, s = 0;
            FOR (k, lo, hi) mx = max(mx, v[k]), s += v[k];
            assert(q.mx == mx && q.sum == s);
        }
    }
    cout << "max nodes per upd: " << worst << " (4 log2 sz = " << 4 * lg << ")\n";
    cout << "Tests passed!" << endl;
}
