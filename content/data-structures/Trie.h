/**
 * Author: caterpillow
 * Date: 2025-10-25
 * Description: maintains a set of ints with queries related to xor
 * Time: O(lg)
 * Source: me
 */

const int lg = 30;
using ptr = struct Node *;
struct Node {
    int cnt;
    ptr c[2];
};

int ins(ptr &n, int x, int i = lg) {
    if (!n) n = new Node {}; //     prevents dupes v    
    return (i-- ? ins(n->c[1 & x >> i], x, i) : !n->cnt) ? ++n->cnt : 0;
}

void multi_ins(ptr &n, int x, int i = lg) {
    if (!n) n = new Node {};
    if (i--) multi_ins(n->c[1 & x >> i], x, i);
    n->cnt++;
}

int del(ptr n, int x, int i = lg) {
    return (n && (i-- ? del(n->c[1 & x >> i], x, i) : n->cnt)) ? n->cnt-- : 0;
}

int cnt(ptr n) { return n ? n->cnt : 0; }

// find y in n with minimum x ^ y
int qmin(ptr n, int x, int i = lg) {
    if (!i--) return 0;
    int b = 1 & x >> i; 
    return cnt(n->c[b]) ? qmin(n->c[b], x, i) :
        qmin(n->c[!b], x, i) | (1 << i);
}

// find y in n with maximum x ^ y
int qmax(ptr n, int x, int i = lg) {
    if (!i--) return 0;
    int b = 1 & ~x >> i; 
    return cnt(n->c[b]) ? qmax(n->c[b], x, i) | (1 << i) :
        qmax(n->c[!b], x, i);
}

// count y in n such that:
template<int sgn> // sgn = 0 for x ^ y < k, sgn = 1 for x ^ y > k
int count(ptr n, int x, int k, int i = lg) {
    if (!i-- || !cnt(n)) return 0;
    int b = 1 & (x ^ k) >> i;
    return ((1 & k >> i) ^ sgn ? cnt(n->c[!b]) : 0) 
        + count<sgn>(n->c[b], x, k, i);
}

inline void push(ptr n, int i) { // lazy xor
    if (i && 1 & n->lazy >> (i - 1)) swap(n->c[0], n->c[1]);
    for (int j = 2; j--; n->c[j] && (n->c[j]->lazy ^= n->lazy));
    n->lazy = 0;
} 

// you can directly add cnt if multiset - otherwise need to pull 
int merge(ptr &l, ptr r, int i = lg) {
    if (!l) swap(l, r);
    if (!l) return 0;
    if (!r || i == 1) return l->cnt;
    push(l, i), push(r, i);
    l->cnt = 0;
    for (int j = 2; j--; l->cnt += merge(l->c[j], r->c[j], i - 1));
    return l->cnt;
}
 
int mex(ptr n, int i = lg) {
    if (!n || !i) return 0;
    push(n, i--);
    int b = n->c[0] && (n->c[0]->cnt == (1 << i));
    return mex(n->c[b], i) + (b << i);
}