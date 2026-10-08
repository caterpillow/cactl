/**
 * Author: caterpillow
 * Date: 2016-8-24
 * License: CC0
 * Source: me
 * Description: Generic-ish sparse segment tree (point update, range query).
 * Time: O(\log N).
 * Usage: ptr t = new Node{INF}; t->upd(i, v); t->query(l, r); upd assigns,
 *  query is [l, r), 0 <= i < sz; the root value is ID.
 * Memory: $\log_2 sz$ nodes per upd.
 * Status: stress-tested
 */
#pragma once

using ptr = struct Node*;
const int sz = 1 << 30;
struct Node {
    #define func(a, b) min(a, b)
    #define ID INF
    ll val;
    ptr lc, rc;

    ptr get(ptr& p) { return p ? p : p = new Node {ID}; }

    ll query(int lo, int hi, int l = 0, int r = sz) {
        if (lo >= r || hi <= l) return ID;
        if (lo <= l && r <= hi) return val;
        int m = (l + r) / 2;
        return func(lc ? lc->query(lo, hi, l, m) : ID,
            rc ? rc->query(lo, hi, m, r) : ID);
    }

    ll upd(int i, ll nval, int l = 0, int r = sz) {
        if (r - l == 1) return val = nval;
        int m = (l + r) / 2;
        if (i < m) get(lc)->upd(i, nval, l, m);
        else get(rc)->upd(i, nval, m, r);
        return val = func(lc ? lc->val : ID, rc ? rc->val : ID);
    }
    #undef ID
    #undef func
};