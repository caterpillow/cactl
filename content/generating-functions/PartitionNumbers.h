/**
 * Author: Claude
 * Date: 2026-10-09
 * License: CC0
 * Source: Euler's pentagonal number theorem
 * Description: $p(0), \ldots, p(n-1)$ mod \texttt{mod}, where $p(i)$
 *  is the number of ways to write $i$ as an unordered sum of positive
 *  integers, i.e.\ $\sum_i p(i) x^i = \prod_{k \ge 1} 1/{(1 - x^k)}$.
 *  No products or inverses (all values $< 3 \cdot$\texttt{mod}), so
 *  any \texttt{mod} $< 2^{61}$, prime or not.
 *  Euler's pentagonal theorem $\prod_{k \ge 1} {(1 - x^k)} =
 *  \sum_{k \in \mathbb{Z}} (-1)^k x^{k(3k-1)/2}$ gives
 *  $p(i) = \sum_{k \ge 1} (-1)^{k+1} [p(i - g_k) + p(i - g_k - k)]$,
 *  where $g_k = k(3k-1)/2$, $g_k + k = k(3k+1)/2$, $p(<0) = 0$.
 * Usage: vl p = partitions(101); // p[100] = 190569292
 * Time: O(n \sqrt n)
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

vl partitions(int n) {
    vl p(n);
    F0R (i, n) {
        p[i] = !i;
        for (int k = 1, g = 1; g <= i; g += 3 * k + 1, k++) {
            ll v = p[i - g] + (g + k <= i ? p[i - g - k] : 0);
            p[i] = (p[i] + (k & 1 ? v : 2 * mod - v)) % mod;
        }
    }
    return p;
} // <hash>
