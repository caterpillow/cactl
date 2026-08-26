/**
 * Author: caterpillow
 * Date: 2025-10-25
 * Description: maintains a multiset of ints in [0, 2\ensuremath{^{lg}})
 * with queries related to xor. All lazy/push lines are optional:
 * delete them if you don't need global xor.
 * Usage: Node t{}; t.ins(x); t.qmin(x); t.lazy ^= v;
 * Time: O(lg)
 * Source: me
 */

const int lg = 30;
using ptr = struct Node *;
struct Node {
    int cnt = 0;
    ptr _c[2] = {};
    ptr &c(int i) { return _c[i] ?: _c[i] = new Node {}; }

    int ins(int x, int i = lg) {
        push(i);
        return (i-- ? c(1 & x >> i)->ins(x, i) : !cnt) ? ++cnt : 0;
    }

    void multi_ins(int x, int i = lg) {
        push(i);
        if (i--) c(1 & x >> i)->multi_ins(x, i);
        cnt++;
    }

    int del(int x, int i = lg) { // wastes a bit of mem
        push(i);
        return (i-- ? c(1 & x >> i)->del(x, i) : cnt) ? cnt-- : 0;
    }

    // find y in n with minimum x ^ y
    int qmin(int x, int i = lg) {
        push(i);
        if (!i--) return 0;
        int b = 1 & x >> i; 
        return c(b)->cnt ? c(b)->qmin(x, i) :
            c(!b)->qmin(x, i) | (1 << i);
    }

    // find y in n with maximum x ^ y
    int qmax(int x, int i = lg) {
        push(i);
        if (!i--) return 0;
        int b = 1 & ~x >> i; 
        return c(b)->cnt ? c(b)->qmax(x, i) | (1 << i) :
            c(!b)->qmax(x, i);
    }

    // count y in n such that:
    template<int sgn> // sgn = 0 for x ^ y < k, sgn = 1 for x ^ y > k
    int count(int x, int k, int i = lg) {
        push(i);
        if (!i-- || !cnt) return 0;
        int b = 1 & (x ^ k) >> i;
        return ((1 & k >> i) ^ sgn ? c(!b)->cnt : 0) 
            + c(b)->count<sgn>(x, k, i);
    }

    int lazy = 0; // root->lazy ^= v xors every element by v

    inline void push(int i) { // lazy xor
        if (i && 1 & lazy >> (i - 1)) swap(_c[0], _c[1]);
        for (int j = 2; j--; _c[j] && (_c[j]->lazy ^= lazy));
        lazy = 0;
    } 

    int mex(int i = lg) {
        if (!i) return 0;
        push(i);
        int b = c(0)->cnt == (1 << --i);
        return c(b)->mex(i) + (b << i);
    }
};

// multiset merge; for set semantics use l->cnt |= r->cnt at the leaf
int merge(ptr &l, ptr r, int i = lg) {
    if (!l) swap(l, r);
    if (!l) return 0;
    if (!r) return l->cnt;
    if (!i) return l->cnt += r->cnt;
    l->push(i), r->push(i);
    l->cnt = 0;
    F0R (j, 2) l->cnt += merge(l->_c[j], r->_c[j], i - 1);
    return l->cnt;
}
 
