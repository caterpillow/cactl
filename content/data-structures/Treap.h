/**
 * Author: caterpillow
 * Date: 2025-08-24
 * Source: me
 * Description: Treap with too many operations 
 * Time: $O(\log N)$
 * Usage: ptr t = 0; every mutator returns the new root. Range
 *  update/query: split, set the piece's lazy / read its agg, merge.
 *  push(n) before reading a node's fields. order(x) = index of x,
 *  valid right after find/findi.
 * Status: stress-tested
 */
#pragma once

using K = ll;
random_device rd;
mt19937 mt(rd());

struct Lazy {
    ll v;
    bool inc, rev;
    void operator+=(const Lazy &b) {
        if (b.inc) v += b.v;
        else v = b.v, inc = false;
        rev ^= b.rev;
    }
};

struct Value {
    ll mx, sum;
    void upd(const Lazy &b, int sz) {
        if (!b.inc) mx = sum = 0;
        mx += b.v, sum += b.v * sz;
    }
    Value operator+(const Value &b) const {
        return {max(mx, b.mx), sum + b.sum};
    }
};

const Lazy LID = {0, true, false};
const Value VID = {-INF, 0};

using ptr = struct Node*;

struct Node {
    int pri;
    K key;
    ptr l, r, par;
    int sz;

    Value val, agg;
    Lazy lazy;

    Node(K key, Value val) : key(key), val(val), agg(val) {
        sz = 1;
        pri = mt();
        l = r = par = 0;
        lazy = LID;
    }

    ~Node() {
        delete l;
        delete r;
    }
};

int sz(ptr n) { return n ? n->sz : 0; }
Value val(ptr n) { return n ? n->val : VID; }
Value agg(ptr n) { return n ? n->agg : VID; }

ptr push(ptr n) {
    if (!n) return n;
    if (n->lazy.rev) swap(n->l, n->r);
    ptr l = n->l, r = n->r;
    n->val.upd(n->lazy, 1);
    n->agg.upd(n->lazy, n->sz);
    if (l) n->l->lazy += n->lazy;
    if (r) n->r->lazy += n->lazy;
    n->lazy = LID;
    return n;
}

ptr pull(ptr n) {
    ptr l = n->l, r = n->r;
    push(l), push(r);
    n->sz = sz(l) + 1 + sz(r);
    n->agg = agg(l) + n->val + agg(r);
    if (n->l) n->l->par = n;
    if (n->r) n->r->par = n;
    return n;
}

pair<ptr, ptr> split(ptr n, K k) {
    if (!n) return {n, n};
    push(n);
    n->par = 0;
    if (k <= n->key) {
        auto [l, r] = split(n->l, k);
        n->l = r;
        return {l, pull(n)};
    } else {
        auto [l, r] = split(n->r, k);
        n->r = l;
        return {pull(n), r};
    }
}

pair<ptr, ptr> spliti(ptr n, int i) {
    if (!n) return {n, n};
    push(n);
    n->par = 0;
    if (i <= sz(n->l)) {
        auto [l, r] = spliti(n->l, i);
        n->l = r;
        return {l, pull(n)};
    } else {
        auto [l, r] = spliti(n->r, i - sz(n->l) - 1);
        n->r = l;
        return {pull(n), r};
    }
}

ptr merge(ptr l, ptr r) {
    if (!l || !r) return l ? l : r;
    push(l), push(r);
    ptr t;
    if (l->pri > r->pri) l->r = merge(l->r, r), t = l;
    else r->l = merge(l, r->l), t = r;
    return pull(t);
}

ptr del(ptr n, K k) { // delete one copy of k, if present
    auto a = split(n, k), b = spliti(a.s, 1);
    if (b.f && b.f->key != k) // k was absent; reattach
        return merge(a.f, merge(b.f, b.s));
    return merge(a.f, b.s);
}

ptr find(ptr n, K k) {
    push(n);
    if (!n || n->key == k) return n;
    if (k < n->key) return find(n->l, k);
    else return find(n->r, k);
}

ptr findi(ptr n, int i) {
    push(n);
    if (!n || i == sz(n->l)) return n;
    if (i < sz(n->l)) return findi(n->l, i);
    else return findi(n->r, i - 1 - sz(n->l));
}

K mn(ptr n) {
    assert(n);
    push(n);
    if (n->l) return mn(n->l);
    else return n->key;
}

int order(ptr n, ptr from = nullptr) {
    if (!n) return -1;
    int res = order(n->par, n);
    if (from == n->r || !from) res += sz(n->l) + 1;
    return res;
}

ptr unite(ptr l, ptr r) {
    if (!l || !r) return l ? l : r;
    // l has the smallest key
    if (mn(l) > mn(r)) swap(l, r);
    ptr res = 0;
    while (r) {
        auto [lt, rt] = split(l, mn(r) + 1);
        res = merge(res, lt);
        tie(l, r) = make_pair(r, rt);
    }
    return merge(res, l);
}

void heapify(ptr n) {
    if (!n) return;
    ptr mx = n;
    if (n->l && n->l->pri > mx->pri) mx = n->l;
    if (n->r && n->r->pri > mx->pri) mx = n->r;
    if (mx != n) swap(n->pri, mx->pri), heapify(mx);
}

ptr build(int l, int r, vt<ptr>& ns) {
    if (l > r) return nullptr;
    if (l == r) return ns[l];
    int m = (r + l) / 2;
    ns[m]->l = build(l, m - 1, ns);
    ns[m]->r = build(m + 1, r, ns);
    heapify(ns[m]);
    return pull(ns[m]);
}

Node* tree;