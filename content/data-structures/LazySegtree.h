/**
 * Author: caterpillow
 * Date: 2016-8-24
 * License: CC0
 * Source: me
 * Description: Generic lazy segment tree
 * Time: O(\log N).
 * Usage: Implement +, upd and id for Node object; += and id for Lazy object.
 * Status: migrated from code-library - untested
 */
#pragma once

struct Lazy {
    ll v;
    bool inc;
    void operator+=(const Lazy &b) {
        if (b.inc) v += b.v;
        else v = b.v, inc = false;
    }
};
 
struct Node {
    ll mx, sum;
    Node operator+(const Node &b) {
        return {max(mx, b.mx), sum + b.sum};
    }
    void upd(const Lazy &u, int l, int r) {
        if (!u.inc) mx = sum = 0;
        mx += u.v, sum += u.v * (r - l);
    }
};

const Lazy lid = {0, true};
const Node nid = {-INF, 0};

const int sz = 1 << 18;
struct Lazyseg { 
    vt<Node> seg;
    vt<Lazy> lazy;
    // careful: max is initialised to -1e18; perform range set or write build function
    Lazyseg() : seg(2 * sz, nid), lazy(2 * sz, lid) {}
    void push(int i, int l, int r) {
        seg[i].upd(lazy[i], l, r);
        if (r - l > 1) F0R (j, 2) lazy[2 * i + j] += lazy[i];
        lazy[i] = lid;
    }
    void upd(int lo, int hi, Lazy val, int i = 1, int l = 0, int r = sz) {
        push(i, l, r);
        if (r <= lo || l >= hi) return;
        if (lo <= l && r <= hi) {
            lazy[i] += val;
            push(i, l, r);
            return;
        }
        int m = (l + r) / 2;
        upd(lo, hi, val, 2 * i, l, m);
        upd(lo, hi, val, 2 * i + 1, m, r);
        seg[i] = seg[2 * i] + seg[2 * i + 1];
    }
    Node query(int lo, int hi, int i = 1, int l = 0, int r = sz) {
        push(i, l, r);
        if (r <= lo || l >= hi) return nid;
        if (lo <= l && r <= hi) return seg[i];
        int m = (l + r) / 2;
        return query(lo, hi, 2 * i, l, m) 
            + query(lo, hi, 2 * i + 1, m, r);
    }
    Node& operator[](int i) {
        return seg[i + sz];
    }
    // void pull(int i) {
    //     seg[i] = seg[2 * i] + seg[2 * i + 1];
    // }
    // void build() {
    //     for (int i = n - 1; i > 0; i--) pull(i);
    // }
};