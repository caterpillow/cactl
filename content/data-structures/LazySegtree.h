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

struct LazySeg { 
    int n;
    vt<Node> seg;
    vt<Lazy> lazy;
    void init(int _n) {
        for (n = 1; n < _n; n *= 2);
        seg.resize(2 * n, nid);
        lazy.resize(2 * n, lid);
    }
    void pull(int i) {
        seg[i] = seg[2 * i] + seg[2 * i + 1];
    }
    void push(int i, int l, int r) {
        seg[i].upd(lazy[i], l, r);
        if (r - l > 1) F0R (j, 2) lazy[2 * i + j] += lazy[i];
        lazy[i] = lid;
    }
    void build() {
        for (int i = n - 1; i > 0; i--) pull(i);
    }
    void upd(int lo, int hi, Lazy val) { upd(lo, hi, val, 1, 0, n); }
    void upd(int lo, int hi, Lazy val, int i, int l, int r) {
        if (r == -1) r = n;
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
        pull(i);
    }
    Node query() { return query(0, n, 1, 0, n); }
    Node query(int lo, int hi) { return query(lo, hi, 1, 0, n); }
    Node query(int lo, int hi, int i, int l, int r) {
        push(i, l, r);
        if (r <= lo || l >= hi) return nid;
        if (lo <= l && r <= hi) return seg[i];
        int m = (l + r) / 2;
        return query(lo, hi, 2 * i, l, m) 
            + query(lo, hi, 2 * i + 1, m, r);
    }
    Node& operator[](int i) {
        return seg[i + n];
    }
};