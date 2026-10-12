/**
 * Author: Claude
 * Date: 2026-10-09
 * License: CC0
 * Source: Flajolet, Sedgewick, Analytic Combinatorics, I.2
 * Description: Euler transform. $a_k$ = number of objects of size $k$
 *  ($a_0$ ignored; any residues: \texttt{mod}$-a_k$ flips the sign of
 *  the factor's exponent). \texttt{mset}: first $n$ terms of
 *  $\prod_{k \ge 1} (1 - x^k)^{-a_k}$ (multisets of objects, by total
 *  size); \texttt{pset}: same for $\prod_{k \ge 1} (1 + x^k)^{a_k}$
 *  (sets of distinct objects). The log is the harmonic sum
 *  $\sum_k a_k \sum_{j \ge 1} x^{jk}/j$ (sign $(-1)^{j+1}$ for
 *  \texttt{pset}), then \texttt{polyExp}, so \texttt{mod} = 998244353
 *  and $n \le 2^{22}$ (PowerSeries.h). Partitions $p(i)$:
 *  \texttt{mset} of all ones; into distinct parts: \texttt{pset} of
 *  all ones. Subsets of items with values $v_i \ge 0$ summing to $s$:
 *  \texttt{pset(cnt, n)}$[s] \cdot 2^{cnt_0}$, where $cnt_v$ = number
 *  of items of value $v$.
 * Usage: vl p = mset(vl(n, 1), n); // partition numbers
 *  vl q = pset(vl(n, 1), n); // partitions into distinct parts
 * Time: O(n \log n)
 * Status: stress-tested
 */
#pragma once

#include "PowerSeries.h"

vl mset(const vl& a, int n, bool p = 0) { // p: pset instead
    vl L(n), iv(n, 1);
    FOR (i, 2, n) iv[i] = (mod - mod / i) * iv[mod % i] % mod;
    if (p) for (int j = 2; j < n; j += 2) iv[j] = mod - iv[j];
    FOR (k, 1, min(n, size(a))) for (int j = 1; j * k < n; j++)
        L[j * k] = (L[j * k] + a[k] * iv[j]) % mod;
    return polyExp(L, n);
} // <hash>

vl pset(const vl& a, int n) { return mset(a, n, 1); } // <hash>
