/**
 * Author: caterpillow, OpenAI
 * Date: 2026-10-09
 * License: CC0
 * Source: folklore
 * Description: $b^e\bmod M$ for fixed $b$, $0\le e<2^B$, via $\lceil B/L\rceil$
 *  table lookups. vs binary exponentiation: $\sim$17x for $M=2^{61}-1$, $\sim$35x
 *  for $M\approx10^9$ ($B=30$, ll \texttt{mul}). $L=16$: $\sim$2x faster, $\le$2 MiB.
 * Time: O(B/L \cdot 2^L) setup, O(B/L) query.
 * Status: stress-tested
 */
#pragma once

template<ll M, int L = 8, int B = 64> struct FixedModPow {
    static_assert(M > 1 && 1 <= L && L <= 16 && 1 <= B && B <= 64);
    static constexpr int N = 1 << L, R = (B+L-1)/L;
    ll pw[R][N]{};
    static constexpr ll mul(ll a, ll b) { // M <= 2^32: (ull)a * b % M
        return (ll)((__uint128_t)a * b % M);
    }
    constexpr FixedModPow(ll b) {
        for (auto &row : pw) {
            row[0] = 1;
            FOR (i, 1, N) row[i] = mul(row[i-1], b);
            b = mul(row[N-1], b);
        }
    }
    constexpr ll operator()(unsigned long long e) const {
        ll r = pw[0][e & (N-1)];
        FOR (i, 1, R) r = mul(r, pw[i][e>>(L*i) & (N-1)]);
        return r;
    }
};
