/**
 * Author: caterpillow
 * Date: 2016-8-24
 * License: CC0
 * Source: me
 * Description: Generic static range query for associative operations.
 * Queries are half-open: query(l, r) folds over $[l, r)$.
 * Time: O(N \log N) build, O(1) query.
 * Usage: Define the desired operation
 * Status: stress-tested
 */
#pragma once

template<class T> struct RangeQuery {
    #define comb(a, b) (a) + (b)
    #define id 0
    int lg, n;
    vt<vt<T>> stor;
    vt<T> a;
    void fill(int l, int r, int ind) {
        if (ind < 0) return;
        int m = (l + r) / 2;
        T prod = id;
        FOR (i, m, r) stor[i][ind] = prod = comb(prod, a[i]);
        prod = id;
        ROF (i, l, m) stor[i][ind] = prod = comb(a[i], prod);
        fill(l, m, ind - 1);
        fill(m, r, ind - 1);
    }
    template <typename It>
    void build(It l, It r) {
        lg = 1;
        while ((1 << lg) < r - l) lg++;
        n = 1 << lg;
        a.resize(n, id);
        for (It i = l; i != r; i++) a[i - l] = *i;
        stor.resize(n, vt<T>(lg + 1));
        fill(0, n, lg - 1);
    }
    T query(int l, int r) { // [l, r)
        if (r-- - l == 1) return a[l];
        int t = __lg(l ^ r);
        return comb(stor[l][t], stor[r][t]);
    }
    #undef id
    #undef comb
};