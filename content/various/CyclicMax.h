/**
 * Author: caterpillow, Claude
 * Date: 2026-09-13
 * License: CC0
 * Source: extrVertex in LineHullIntersection.h
 * Description: Argmax of a cyclically unimodal sequence $f(0..n-1)$: read
 * from its minimum it strictly increases up to the maximum and then
 * strictly decreases back, wrapping around. The max and the min may each
 * be shared by two adjacent indices, never more, and no other neighbours
 * may be equal. Returns $(i, i+1 \bmod n)$ if $f(i) = f(i+1)$ is the max,
 * else $(i, -1)$. \texttt{cyc\_peak} takes a 3-way \texttt{comp(i, j)}
 * $=$ sign of $f(i) - f(j)$, exactly $-1/0/1$, instead of values (angles
 * via cross products, or negate for the argmin). At most $5 \log_2 n$
 * comparisons. The eps version needs consecutive values to differ by
 * more than eps except at the two ties.
 * Usage: cyc_max(n, [\&](int i) { return poly[i].dot(dir); })
 * Time: O(\log n)
 * Status: stress-tested
 */
#pragma once

template<class C> pi cyc_peak(int n, C comp) {
    auto c = [&](int i, int j) { return comp(i % n, j % n); };
    auto peak = [&](int i) {
        return c(i, i + 1) >= 0 && c(i - 1 + n, i) < 0; };
    int lo = 0, hi = n;
    if (!peak(0)) while (lo + 1 < hi) {
        int m = (lo + hi) / 2;
        if (peak(m)) { lo = m; break; }
        int ls = c(lo, lo + 1), ms = c(m, m + 1);
        (ls < ms || (ls == ms && ls == c(m, lo)) ? hi : lo) = m;
    }
    return {lo, c(lo, lo + 1) || n < 2 ? -1 : (lo + 1) % n};
}
template<class F> pi cyc_max(int n, F fn) { // anything with <
    return cyc_peak(n, [&](int i, int j) {
        auto a = fn(i), b = fn(j); return (b < a) - (a < b); });
} // <hash>
template<class F> pi cyc_max(int n, F fn, db eps) { // doubles
    return cyc_peak(n, [&](int i, int j) {
        db d = fn(i) - fn(j); return (d > eps) - (d < -eps); });
}
