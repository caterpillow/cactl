// Stress test for content/data-structures/Treap.h (key+index treap).
// Tests: del exact-key semantics (dups, absent keys), findi with keys != indices,
// upd/updi on empty ranges, and full randomized mixes vs brute force:
// key mode (ins/del/upd/query/find/findi/mn/unite) and index mode
// (insi/deli/updi with add/assign/rev, queryi/findi). written by Claude (audit)
#include "../utilities/template.h"
#undef sz // template.h shim clashes with Treap.h's sz(ptr) function
#include "../../content/data-structures/Treap.h"

mt19937 rng(12345);
ll rnd(ll a, ll b) { return uniform_int_distribution<ll>(a, b)(rng); }

using PKV = pair<K, ll>;
Value mkval(ll v) { return {v, v}; }

void collect(ptr n, vt<PKV>& out) {
    if (!n) return;
    push(n);
    collect(n->l, out);
    out.pb({n->key, n->val.sum});
    collect(n->r, out);
}
void check_eq(ptr n, const vt<PKV>& brute) {
    vt<PKV> got;
    collect(n, got);
    assert(got == brute);
}
// first index with key >= k (treap's ins puts new node before existing equals)
int lbk(const vt<PKV>& b, K k) {
    int i = 0;
    while (i < size(b) && b[i].f < k) i++;
    return i;
}

// mirrors unite(): l = tree with smaller min (tie: first arg); repeatedly move
// l's prefix with key <= mn(r) to result, then roles swap
vt<PKV> brute_unite(vt<PKV> a, vt<PKV> b) {
    if (a.empty() || b.empty()) return a.empty() ? b : a;
    if (a[0].f > b[0].f) swap(a, b);
    vt<PKV> res;
    while (!b.empty()) {
        K m = b[0].f;
        int i = 0;
        while (i < size(a) && a[i].f <= m) res.pb(a[i++]);
        vt<PKV> rem(a.begin() + i, a.end());
        a = b, b = rem;
    }
    for (auto& p : a) res.pb(p);
    return res;
}

void test_del() { // item 1: exact-match delete, duplicates, absent keys
    F0R(iter, 300) {
        ptr n = 0;
        vt<PKV> brute;
        F0R(op, 80) {
            if (brute.empty() || rnd(0, 2)) { // insert, keys [0,15] => many dups
                K k = rnd(0, 15);
                ll v = rnd(-100, 100);
                n = ins(n, k, mkval(v));
                brute.insert(brute.begin() + lbk(brute, k), {k, v});
            } else { // delete key in [-3,18]: present, absent-between, absent-outside
                K k = rnd(-3, 18);
                int i = lbk(brute, k);
                if (i < size(brute) && brute[i].f == k) brute.erase(brute.begin() + i);
                n = del(n, k);
            }
            check_eq(n, brute);
        }
        delete n;
    }
}

void test_findi() { // item 2: i-th node with arbitrary keys (keys != indices)
    F0R(iter, 200) {
        ptr n = 0;
        vt<PKV> brute;
        int m = rnd(1, 60);
        F0R(j, m) {
            int i = rnd(0, size(brute));
            K k = rnd(-1000000000LL, 1000000000LL);
            ll v = rnd(-50, 50);
            n = insi(n, i, k, mkval(v));
            brute.insert(brute.begin() + i, {k, v});
        }
        F0R(i, size(brute)) {
            ptr p = findi(n, i);
            assert(p && p->key == brute[i].f && p->val.sum == brute[i].s);
        }
        assert(!findi(n, size(brute)) && !findi(n, size(brute) + 7));
        delete n;
    }
}

void test_empty_ranges() { // item 3: upd/updi on empty ranges are no-ops
    Lazy add5 = {5, true, false}, asg7 = {7, false, false};
    ptr n = 0;
    n = upd(n, 1, 5, add5);        // empty treap
    n = updi(n, 0, 3, asg7);
    assert(!n);
    n = ins(ins(0, 0, mkval(1)), 10, mkval(2));
    vt<PKV> brute = {{0, 1}, {10, 2}};
    n = upd(n, 3, 7, add5);        // between keys
    n = upd(n, -9, -1, asg7);      // below all
    n = upd(n, 20, 30, add5);      // above all
    n = upd(n, 7, 3, asg7);        // lo > hi
    n = updi(n, 2, 5, add5);       // index range past end
    n = updi(n, 1, 0, asg7);       // lo > hi
    assert(agg(n).sum == 3 && agg(n).mx == 2);
    check_eq(n, brute);
    Value q = query(n, 3, 7);      // empty-range queries return VID
    assert(q.mx == -INF && q.sum == 0);
    q = queryi(n, 5, 2);
    assert(q.mx == -INF && q.sum == 0);
    delete n;
}

void test_mix_key() { // item 4a: key-mode mix incl. unite, add/assign lazies
    ptr n = 0;
    vt<PKV> brute;
    F0R(op, 6000) {
        int t = rnd(0, 9);
        if (size(brute) > 600) t = 2; // cap size
        if (t <= 1) {
            K k = rnd(0, 50);
            ll v = rnd(-1000, 1000);
            n = ins(n, k, mkval(v));
            brute.insert(brute.begin() + lbk(brute, k), {k, v});
        } else if (t == 2) {
            K k = rnd(-2, 52);
            int i = lbk(brute, k);
            if (i < size(brute) && brute[i].f == k) brute.erase(brute.begin() + i);
            n = del(n, k);
        } else if (t == 3) {
            K lo = rnd(-2, 52), hi = rnd(-2, 52); // sometimes lo > hi
            ll v = rnd(-100, 100);
            bool asg = rnd(0, 1);
            n = upd(n, lo, hi, {v, !asg, false});
            if (lo <= hi) for (auto& p : brute) if (lo <= p.f && p.f <= hi) p.s = asg ? v : p.s + v;
        } else if (t == 4) {
            K lo = rnd(-2, 52), hi = rnd(-2, 52);
            Value q = query(n, lo, hi);
            ll mx = -INF, sm = 0;
            for (auto& p : brute) if (lo <= p.f && p.f <= hi) mx = max(mx, p.s), sm += p.s;
            assert(q.mx == mx && q.sum == sm);
        } else if (t == 5) {
            K k = rnd(-2, 52);
            ptr p = find(n, k);
            bool have = lbk(brute, k) < size(brute) && brute[lbk(brute, k)].f == k;
            assert(!!p == have && (!p || p->key == k));
        } else if (t == 6 && !brute.empty()) {
            int i = rnd(0, size(brute) - 1);
            ptr p = findi(n, i);
            assert(p && p->key == brute[i].f && p->val.sum == brute[i].s);
        } else if (t == 7 && !brute.empty()) {
            assert(mn(n) == brute[0].f);
        } else if (t == 8) {
            ptr t2 = 0;
            vt<PKV> b2;
            int m = rnd(1, 8);
            F0R(j, m) {
                K k = rnd(0, 50);
                ll v = rnd(-1000, 1000);
                t2 = ins(t2, k, mkval(v));
                b2.insert(b2.begin() + lbk(b2, k), {k, v});
            }
            n = unite(n, t2);
            brute = brute_unite(brute, b2);
        }
        if (op % 25 == 0) check_eq(n, brute);
    }
    check_eq(n, brute);
    delete n;
}

void test_mix_index() { // item 4b: index-mode mix with add/assign/rev lazies
    ptr n = 0;
    vt<PKV> brute;
    F0R(op, 6000) {
        int t = rnd(0, 5);
        int N = size(brute);
        if (N > 400) t = 1;
        if (t == 0 || N == 0) {
            int i = rnd(0, N);
            K k = rnd(-1000000, 1000000);
            ll v = rnd(-1000, 1000);
            n = insi(n, i, k, mkval(v));
            brute.insert(brute.begin() + i, {k, v});
        } else if (t == 1) {
            int i = rnd(0, N - 1);
            n = deli(n, i);
            brute.erase(brute.begin() + i);
        } else if (t == 2) {
            int lo = rnd(0, N - 1), hi = rnd(0, N - 1); // sometimes lo > hi
            int kind = rnd(0, 2);
            ll v = rnd(-100, 100);
            Lazy lz = kind == 0 ? Lazy{v, true, false}
                    : kind == 1 ? Lazy{v, false, false} : Lazy{0, true, true};
            n = updi(n, lo, hi, lz);
            if (lo <= hi) {
                if (kind == 0) FOR(i, lo, hi + 1) brute[i].s += v;
                else if (kind == 1) FOR(i, lo, hi + 1) brute[i].s = v;
                else reverse(brute.begin() + lo, brute.begin() + hi + 1);
            }
        } else if (t == 3) {
            int lo = rnd(0, N - 1), hi = rnd(0, N - 1);
            Value q = queryi(n, lo, hi);
            ll mx = -INF, sm = 0;
            FOR(i, lo, hi + 1) mx = max(mx, brute[i].s), sm += brute[i].s;
            assert(q.mx == mx && q.sum == sm);
        } else {
            int i = rnd(0, N - 1);
            ptr p = findi(n, i);
            assert(p && p->key == brute[i].f && p->val.sum == brute[i].s);
        }
        if (op % 25 == 0) check_eq(n, brute);
    }
    check_eq(n, brute);
    delete n;
}

int main() {
    test_del();
    test_findi();
    test_empty_ranges();
    test_mix_key();
    test_mix_index();
    cout << "Tests passed!" << endl;
}
