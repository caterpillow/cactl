/**
 * Author: Claude
 * Date: 2026-10-06
 * License: CC0
 * Source: folklore (ranked zeta transform)
 * Description: $c[S] = \sum_{T \subseteq S} a[T]\, b[S \setminus T]$,
 * arrays of size $2^n$, values mod \texttt{mod}. Zeta transform by
 * popcount rank, multiply the rank polynomials, invert, keep rank
 * = popcount. Unlike OR-convolution it forbids overlaps.
 * Usage: vl c = subset_conv(a, b, n);
 * Time: O(2^n n^2)
 * Memory: $24 (n+1) 2^n$ bytes ($n \le 18$)
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

vl subset_conv(vl a, vl b, int n) {
    int M = 1 << n;
    vt<vl> A(n + 1, vl(M)), B = A, C = A;
    F0R (i, M) {
        int k = __builtin_popcount(i);
        A[k][i] = a[i], B[k][i] = b[i];
    }
    F0R (k, n + 1) F0R (j, n) F0R (i, M) if (i >> j & 1) {
        A[k][i] = (A[k][i] + A[k][i ^ 1 << j]) % mod;
        B[k][i] = (B[k][i] + B[k][i ^ 1 << j]) % mod;
    }
    F0R (i, M) F0R (x, n + 1) F0R (y, n + 1 - x)
        C[x + y][i] = (C[x + y][i] + A[x][i] * B[y][i]) % mod;
    F0R (k, n + 1) F0R (j, n) F0R (i, M) if (i >> j & 1)
        C[k][i] = (C[k][i] - C[k][i ^ 1 << j] + mod) % mod;
    vl c(M);
    F0R (i, M) c[i] = C[__builtin_popcount(i)][i];
    return c;
}
