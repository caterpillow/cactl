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
    if (!n) n = new Node {};
    if (i-- ? ins(n->c[1 & x >> i], x, i) : !n->cnt) 
        return n->cnt++, 1; //     prevents dupes ^ 
    return 0;
}

int del(ptr n, int x, int i = lg) {
    if (n && (i-- ? del(n->c[1 & x >> i], x, i) : n->cnt)) 
        return n->cnt--, 1;
    return 0;
}

int cnt(ptr n) { return n ? n->cnt : 0; }

// find y in n with minimum x ^ y
int qmin(ptr n, int x, int i = lg) {
    if (!i--) return 0;
    int b = 1 & x >> i; 
    if (cnt(n->c[b])) return qmin(n->c[b], x, i);
    else return qmin(n->c[!b], x, i) | (1 << i);
}

// find y in n with maximum x ^ y
int qmax(ptr n, int x, int i = lg) {
    if (!i--) return 0;
    int b = 1 & ~x >> i; 
    if (cnt(n->c[b])) return qmax(n->c[b], x, i) | (1 << i);
    else return qmax(n->c[!b], x, i);
}

// count y in n such that:
template<int sgn> // sgn = 0 for x ^ y < k, sgn = 1 for x ^ y > k
int count(ptr n, int x, int k, int i = lg) {
    if (!i-- || !cnt(n)) return 0;
    int b = 1 & (x ^ k) >> i;
    return ((1 & k >> i) ^ sgn ? cnt(n->c[!b]) : 0) 
        + count<sgn>(n->c[b], x, k, i);
}