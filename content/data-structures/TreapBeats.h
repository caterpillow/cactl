/**
 * Author: caterpillow
 * Date: 2025-10-25
 * Source: me
 * Description: segtree beats but treap
 * Time: $O(N \log N)$
 * Status: taken off byot
 */
#pragma once

struct Lazy {
    int mn;
    int mx;
    int add;

    void operator+=(const Lazy oth) {
        if (oth.mn - add <= mx) mn = mx = oth.mn - add;
        else if (oth.mx - add >= mn) mn = mx = oth.mx - add;
        else {
            mn = min(mn, oth.mn - add);
            mx = max(mx, oth.mx - add);
        }
        add += oth.add;
    }
};

const Lazy lid = {1'000'000'000, -1'000'000'000, 0};

Lazy chmin_tag(ll x) { Lazy lazy = lid; lazy.mn = x; return lazy; }
Lazy chmax_tag(ll x) { Lazy lazy = lid; lazy.mx = x; return lazy; }
Lazy add_tag(ll x) { Lazy lazy = lid; lazy.add = x; return lazy; }

// You can implement your own monoid here for custom operations.
struct Value {
    int sum;
    int mx, mxcnt, mx2;
    int mn, mncnt, mn2;

    static Value make(ll x, ll len = 1) {
        return {x * len, x, len, -1'000'000'000, x, len, 1'000'000'000};
    }

    bool can_break(const Lazy& lazy) {
        return lazy.mn >= mx && lazy.mx <= mn && lazy.add == 0;
    }

    bool can_tag(const Lazy& lazy) {
        return mx2 < lazy.mn && mn2 > lazy.mx;
    }

    void upd(Lazy lazy, int sz) {
        if (mn == mx) {
            mn = mx = min(lazy.mn, mn);
            mn = mx = max(lazy.mx, mn);
            sum = mn * mncnt;
        } else if (mn == mx2) {
            if (lazy.mx > mn) mn = mx2 = lazy.mx;
            if (lazy.mn < mx) mx = mn2 = lazy.mn;
            sum = mn * mncnt + mx * mxcnt;
        } else {
            if (lazy.mn < mx) sum -= (mx - lazy.mn) * mxcnt, mx = lazy.mn;
            if (lazy.mx > mn) sum += (lazy.mx - mn) * mncnt, mn = lazy.mx;
        }
        sum += lazy.add * sz;
        mx += lazy.add, mx2 += lazy.add;
        mn += lazy.add, mn2 += lazy.add;
    }

    Value operator+(const Value& oth) const {
        Value res {};
        res.sum = sum + oth.sum;
        if (mx == oth.mx) res.mx = mx, res.mxcnt = mxcnt + oth.mxcnt, res.mx2 = max(mx2, oth.mx2);
        else if (mx > oth.mx) res.mx = mx, res.mxcnt = mxcnt, res.mx2 = max(mx2, oth.mx);
        else res.mx = oth.mx, res.mxcnt = oth.mxcnt, res.mx2 = max(mx, oth.mx2);
        if (mn == oth.mn) res.mn = mn, res.mncnt = mncnt + oth.mncnt, res.mn2 = min(mn2, oth.mn2);
        else if (mn < oth.mn) res.mn = mn, res.mncnt = mncnt, res.mn2 = min(mn2, oth.mn);
        else res.mn = oth.mn, res.mncnt = oth.mncnt, res.mn2 = min(mn, oth.mn2);
        return res;
    }
};

const Value vid = {0, -1'000'000'000, 0, -1'000'000'000, 1'000'000'000, 0, 1'000'000'000};

mt19937 mt(chrono::steady_clock::now().time_since_epoch().count());
using ptr = struct Node*;

struct Node {
    Value val;
    Value agg;
    Lazy lazy;

    int sz;
    int pri;
    ptr l, r;

    Node() {
        pri = mt();
        val = vid;
        agg = vid;
        lazy = lid;
        sz = 0;
        l = r = nullptr;
    }

    Node(Value val) : val(val), agg(val) {
        pri = mt();
        lazy = lid;
        sz = 1;
        l = r = nullptr;
    }

    ~Node() {
        delete l;
        delete r;
    }
};

int sz(ptr n) { return n ? n->sz : 0; };
Value agg(ptr n) { return n ? n->agg : vid; }

void push(ptr n) {
    n->val.upd(n->lazy, 1);
    n->agg.upd(n->lazy, sz(n));
    if (n->l) n->l->lazy += n->lazy;
    if (n->r) n->r->lazy += n->lazy;
    n->lazy = lid;
}

ptr pull(ptr n) {
    if (!n) return nullptr;
    if (n->l) push(n->l);
    if (n->r) push(n->r);
    n->sz = sz(n->l) + 1 + sz(n->r);
    n->agg = agg(n->l) + n->val + agg(n->r);
    return n;
}

void updi(ptr n, int lo, int hi, Lazy lazy) {
    if (!n) return;
    push(n);
    if (lo >= n->sz || hi <= 0 || n->agg.can_break(lazy)) return;
    if (lo <= 0 && n->sz <= hi && n->agg.can_tag(lazy)) {
        n->lazy += lazy;
        push(n);
        return;
    }
    if (lo <= sz(n->l) && sz(n->l) < hi) n->val.upd(lazy, 1);
    updi(n->l, lo, hi, lazy);
    updi(n->r, lo - 1 - sz(n->l), hi - 1 - sz(n->l), lazy);
    pull(n);
}

Value queryi(ptr n, int lo, int hi) {
    if (!n || lo >= sz(n) || hi <= 0) return vid;
    push(n);
    if (lo <= 0 && sz(n) <= hi) return n->agg;
    return queryi(n->l, lo, hi) + (lo <= sz(n->l) && sz(n->l) < hi ? n->val : vid) + queryi(n->r, lo - 1 - sz(n->l), hi - 1 - sz(n->l));
}