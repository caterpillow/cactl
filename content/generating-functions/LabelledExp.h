/**
 * Author: Claude
 * Date: 2026-10-09
 * License: CC0
 * Source: folklore (exponential formula)
 * Description: Labelled exp/log on plain counts, not EGF
 *  coefficients, so no inverses: any \texttt{mod} $\le 3 \cdot 10^9$,
 *  prime or not, even $n \ge$ \texttt{mod}. Entries in [0, \texttt{mod}).
 *  $a_k$ = number of connected structures on $k$ labels ($a_0$
 *  ignored); \mbox{\texttt{labelledExp}} returns $b_0, \ldots, b_{n-1}$,
 *  $b_i$ = number of sets of them partitioning $i$ labels
 *  ($\sum b_i x^i/i! = \exp \sum a_k x^k/k!$). Choose the other
 *  labels of the component containing label 1: $b_0 = 1$,
 *  $b_i = \sum_{k=1}^{i} \binom{i-1}{k-1} a_k b_{i-k}$.
 *  \mbox{\texttt{labelledLog}} inverts it ($g_0 = 1$ assumed, $c_0 = 0$):
 *  $c_i = g_i - \sum_{k=1}^{i-1} \binom{i-1}{k-1} c_k g_{i-k}$.
 *  E.g.\ connected graphs are \mbox{\texttt{labelledLog}} of
 *  $g_i = 2^{i(i-1)/2}$: 0, 1, 1, 4, 38, 728, 26704.
 *  Both return exactly $n$ terms.
 * Usage: vl bell = labelledExp(vl(n, 1), n);
 *  vl conn = labelledLog(g, n); // g[i] = 2^C(i,2)
 * Time: O(n^2)
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

vl labelledExp(vl a, int n) {
    vl b(n), R(n + 1); a.resize(n); R[0] = 1;
    F0R (i, n) { // R[j] = C(i-1, j), one Pascal row at a time
        ROF (j, 1, i) R[j] = (R[j] + R[j - 1]) % mod;
        b[i] = !i;
        FOR (k, 1, i + 1)
            b[i] = (b[i] + R[k - 1] * a[k] % mod * b[i - k]) % mod;
    }
    return b;
} // <hash>

vl labelledLog(vl g, int n) {
    vl c(n), R(n + 1); g.resize(n); R[0] = 1;
    FOR (i, 1, n) {
        ROF (j, 1, i) R[j] = (R[j] + R[j - 1]) % mod;
        ll v = 0;
        FOR (k, 1, i)
            v = (v + R[k - 1] * c[k] % mod * g[i - k]) % mod;
        c[i] = (g[i] - v + mod) % mod;
    }
    return c;
} // <hash>
