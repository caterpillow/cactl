// Tests content/data-structures/Trie.h (binary xor trie): ins/multi_ins/del/
// qmin/qmax/count<0|1>/mex vs brute multiset; same ops interleaved with global
// lazy xors (root->lazy ^= v == xor every element); multiset merge() incl. lazy
// on both sides, with queries + more lazy/updates on the merged trie.
// Build with -DTRIE_HEADER='"<path>"' -DTRIE_NOLAZY to test a lazy-stripped copy.
// written by Claude (audit)
#include "../../utilities/template.h"
#ifdef TRIE_HEADER
#include TRIE_HEADER
#else
#include "../../../content/data-structures/Trie.h"
#endif

mt19937 rng(20260827);
int rnd(int n) { return (int)(rng() % n); }
const int M = (1 << 30) - 1;

int bmin(multiset<int>& ms, int x) { int r = INT_MAX; for (int y : ms) r = min(r, x ^ y); return r; }
int bmax(multiset<int>& ms, int x) { int r = -1; for (int y : ms) r = max(r, x ^ y); return r; }
int bcnt(multiset<int>& ms, int x, int k, int sgn) {
    int r = 0; for (int y : ms) r += sgn ? (x ^ y) > k : (x ^ y) < k; return r;
}
int bmex(multiset<int>& ms) { int m = 0; while (ms.count(m)) m++; return m; }
void bxor(multiset<int>& ms, int v) { multiset<int> t; for (int y : ms) t.insert(y ^ v); ms = t; }

// items (1)+(2): random ops vs brute multiset; setmode = no multi_ins, mex checked
void run(bool lazyMode, bool setmode, int mask, int iters, int ops) {
    F0R (it, iters) {
        ptr t = new Node {};
        multiset<int> ms;
        F0R (op, ops) {
            int ty = rnd(10), x = (int)rng() & mask;
            if (ty < 3) {
                if (setmode || rnd(2)) {
                    bool had = ms.count(x);
                    assert(!!t->ins(x) == !had);
                    if (!had) ms.insert(x);
                } else { t->multi_ins(x); ms.insert(x); }
            } else if (ty == 3) {
                auto jt = ms.find(x);
                assert(!!t->del(x) == (jt != end(ms)));
                if (jt != end(ms)) ms.erase(jt);
            } else if (ty == 4 && size(ms)) assert(t->qmin(x) == bmin(ms, x));
            else if (ty == 5 && size(ms)) assert(t->qmax(x) == bmax(ms, x));
            else if (ty == 6) { int k = (int)rng() & (rnd(2) ? M : mask); assert(t->count<0>(x, k) == bcnt(ms, x, k, 0)); }
            else if (ty == 7) { int k = (int)rng() & (rnd(2) ? M : mask); assert(t->count<1>(x, k) == bcnt(ms, x, k, 1)); }
            else if (ty == 8 && setmode) assert(t->mex() == bmex(ms));
            else if (ty == 9 && lazyMode) {
#ifndef TRIE_NOLAZY
                int v = (int)rng() & mask; t->lazy ^= v; bxor(ms, v);
#endif
            }
            assert(t->cnt == size(ms));
        }
    }
}

// item (3): merge two random tries (multiset semantics), optionally after lazy
// xors on both sides, then query/update the merged trie vs the merged brute set
void mergeTest(bool lazyMode, int mask, int iters) {
    F0R (it, iters) {
        ptr a = new Node {}, b = new Node {};
        multiset<int> A, B;
        auto fill = [&](ptr t, multiset<int>& ms, int n) {
            F0R (j, n) {
                int x = (int)rng() & mask;
                if (rnd(4)) { t->multi_ins(x); ms.insert(x); }
                else if (t->ins(x)) ms.insert(x);
                if (!rnd(8)) { int y = (int)rng() & mask; if (t->del(y)) ms.erase(ms.find(y)); }
#ifndef TRIE_NOLAZY
                if (lazyMode && !rnd(8)) { int v = (int)rng() & mask; t->lazy ^= v; bxor(ms, v); }
#endif
            }
        };
        fill(a, A, rnd(50)); fill(b, B, rnd(50));
        assert(merge(a, b) == size(A) + size(B));
        for (int y : B) A.insert(y);
        assert(a && a->cnt == size(A));
        if (!rnd(4)) { ptr z = nullptr; assert(merge(z, a) == size(A)); a = z; } // null-l case
        F0R (q, 30) {
            int x = (int)rng() & mask;
            if (size(A)) { assert(a->qmin(x) == bmin(A, x)); assert(a->qmax(x) == bmax(A, x)); }
            int k = (int)rng() & (rnd(2) ? M : mask);
            assert(a->count<0>(x, k) == bcnt(A, x, k, 0));
            assert(a->count<1>(x, k) == bcnt(A, x, k, 1));
#ifndef TRIE_NOLAZY
            if (lazyMode && !rnd(4)) { int v = (int)rng() & mask; a->lazy ^= v; bxor(A, v); }
#endif
            if (!rnd(4)) { int y = (int)rng() & mask; a->multi_ins(y); A.insert(y); }
            if (!rnd(6)) { int y = (int)rng() & mask; if (a->del(y)) A.erase(A.find(y)); }
            assert(a->cnt == size(A));
        }
    }
}

signed main() {
    // (1) no lazy
    run(false, true, 63, 60, 150);
    run(false, true, M, 60, 150);
    run(false, false, 63, 60, 150);
    run(false, false, 1023, 60, 150);
    run(false, false, M, 60, 150);
#ifndef TRIE_NOLAZY
    // (2) with random global xors
    run(true, true, 63, 60, 150);
    run(true, true, M, 60, 150);
    run(true, false, 63, 60, 150);
    run(true, false, M, 60, 150);
#endif
    // (3) merge
    mergeTest(false, 63, 150);
    mergeTest(false, M, 150);
#ifndef TRIE_NOLAZY
    mergeTest(true, 63, 150);
    mergeTest(true, M, 150);
#endif
    cout << "Tests passed!" << endl;
}
