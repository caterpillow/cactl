/**
 * Author: caterpillow
 * Date: 2026-10-11
 * License: CC0
 * Source: me
 * Description: Persistent lazy segment tree. Lazy/Node as in LazySegtree;
 *  Node::upd may only use r - l. Query allocates nothing.
 * Time: O(\log N).
 * Usage: ptr t = new Seg{{0, 0}, lid}; t->lc = t->rc = t; // all-0
 *  t = t->upd(l, r, {x, true}); t->query(l, r) on [l, r).
 * Memory: $\le 4\log_2 sz$ nodes per upd, never freed.
 * Status: stress-tested
 */
#pragma once

// use Node and Lazy semantics from LazySegtree.h

using ptr = struct Seg*;
const int sz = 1 << 18;
struct Seg {
    Node v;
    Lazy lz;
    ptr lc, rc;

    ptr apply(const Lazy &t, int l, int r) { // copy, then tag
        ptr p = new Seg(*this);
        p->v.upd(t, l, r), p->lz += t;
        return p;
    }

    // t: tag pushed from the parent, older than u
    ptr upd(int lo, int hi, const Lazy &u, Lazy t = lid,
            int l = 0, int r = sz) {
        if (r <= lo || l >= hi) return apply(t, l, r);
        if (lo <= l && r <= hi) return t += u, apply(t, l, r);
        int m = (l + r) / 2;
        Lazy p = lz; p += t;
        ptr a = lc->upd(lo, hi, u, p, l, m);
        ptr b = rc->upd(lo, hi, u, p, m, r);
        return new Seg{a->v + b->v, lid, a, b};
    }

    Node query(int lo, int hi, int l = 0, int r = sz) {
        if (r <= lo || l >= hi) return nid;
        if (lo <= l && r <= hi) return v;
        int m = (l + r) / 2;
        Node res = lc->query(lo, hi, l, m)
            + rc->query(lo, hi, m, r);
        res.upd(lz, max(l, lo), min(r, hi)); // lz is newest
        return res;
    }

    // leaves a[0..sz), 2 sz nodes, no self-loop root needed
    ptr build(vt<Node> &a, int l = 0, int r = sz) {
        if (r - l == 1) return new Seg{a[l], lid};
        int m = (l + r) / 2;
        ptr x = build(a, l, m), y = build(a, m, r);
        return new Seg{x->v + y->v, lid, x, y};
    }
};
