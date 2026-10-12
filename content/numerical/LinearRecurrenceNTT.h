/**
 * Author: Claude
 * Date: 2026-10-09
 * License: CC0
 * Source: Bostan, Mori, A Simple and Fast Algorithm for Computing
 *  the N-th Term of a Linearly Recurrent Sequence (2020)
 * Description: Same contract as linearRec: $k$'th term of
 *  $S[i] = \sum_j S[i-j-1]tr[j]$, given $S[0 \ldots \ge n-1]$ and $tr$.
 *  $s[k] = [x^k] P/Q$ with $Q = 1 - \sum_j tr[j] x^{j+1}$; each step
 *  multiplies by $Q(-x)$ and keeps the half of $P$ matching $k$'s parity.
 *  NTT mod (998244353); for other mods use convMod from FFTMod.
 * Usage: linearRecNTT({0, 1}, {1, 1}, k) // k'th Fibonacci number
 * Time: O(n \log n \log k)
 * Status: stress-tested
 */
#pragma once

#include "NumberTheoreticTransform.h"

ll linearRecNTT(vl S, vl tr, ll k) {
    int n = size(tr);
    assert(size(S) >= n);
    vl Q(n + 1); Q[0] = 1;
    F0R (i, n) Q[i + 1] = (mod - tr[i] % mod) % mod;
    S.resize(n);
    vl P = conv(S, Q); P.resize(n);
    for (; k; k >>= 1) {
        vl R = Q;
        for (int i = 1; i < size(R); i += 2)
            R[i] = (mod - R[i]) % mod;
        vl A = conv(P, R), B = conv(Q, R);
        P.clear(), Q.clear();
        for (int i = k & 1; i < size(A); i += 2) P.pb(A[i]);
        for (int i = 0; i < size(B); i += 2) Q.pb(B[i]);
    }
    return P.empty() ? 0 : P[0]; // Q[0] stays 1
}
