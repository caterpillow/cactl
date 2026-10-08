/**
 * Author: Claude
 * Date: 2026-10-06
 * License: CC0
 * Source: folklore (Gauss-Jordan)
 * Description: Solves $Ax = b \pmod{\texttt{mod}}$ (prime), $A$ is
 * $n \times m$. Returns the rank, or $-1$ if there is no solution;
 * free variables are 0. The solution is unique iff rank $= m$.
 * Usage: vl x; int r = solve_mod(A, b, x);
 * Time: O(n m \min(n, m))
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

int solve_mod(vt<vl> A, vl b, vl& x) {
    int n = size(A), m = size(A[0]), rk = 0;
    vi piv;
    F0R (c, m) {
        int r = rk;
        while (r < n && !A[r][c]) r++;
        if (r == n) continue;
        swap(A[rk], A[r]), swap(b[rk], b[r]);
        ll v = mpow(A[rk][c]);
        F0R (j, m) A[rk][j] = A[rk][j] * v % mod;
        b[rk] = b[rk] * v % mod;
        F0R (i, n) if (i != rk && A[i][c]) {
            ll f = A[i][c];
            F0R (j, m)
                A[i][j] = (A[i][j] - f * A[rk][j] % mod + mod) % mod;
            b[i] = (b[i] - f * b[rk] % mod + mod) % mod;
        }
        piv.pb(c), rk++;
    }
    FOR (i, rk, n) if (b[i]) return -1;
    x.assign(m, 0);
    F0R (i, rk) x[piv[i]] = b[i];
    return rk;
}
