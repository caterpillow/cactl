/**
 * Author: caterpillow, Claude
 * Date: 2026-08-29
 * License: CC0
 * Source: folklore
 * Description: Unsigned bigint of exactly $N$ 64-bit limbs, little endian,
 * so every operation is mod $2^{64N}$: a subtraction that would go negative
 * wraps instead of failing, and comparison is unsigned (add and subtract
 * also match two's complement, comparison does not).
 * \texttt{bit(i)} is the $2^i$ bit, $i$ in $[0, 64N)$; nothing is checked.
 * Reach for this when a bound on the width is known and speed matters --
 * the limbs sit in the object, nothing is allocated. Use BigInt.h when the
 * length has to grow. No multiplication, division or decimal I/O; print the
 * limbs in hex from the top down.
 * Time: O(N) per operation.
 * Status: stress-tested against __uint128_t and a bitset oracle
 */
#pragma once

using ull = unsigned long long;
using u128 = __uint128_t;
template <int N> struct BigF {
    array<ull, N> d{}; // limb i is bits [64i, 64i + 64)
    BigF(ull x = 0) { d[0] = x; }
    bool bit(int i) const { return d[i / 64] >> (i % 64) & 1; }
    void flip(int i) { d[i / 64] ^= 1ULL << (i % 64); }
    void set(int i, bool v = 1) { if (bit(i) != v) flip(i); }
    bool operator==(const BigF& b) const { return d == b.d; }
    bool operator<(const BigF& b) const {
        ROF (i, 0, N) if (d[i] != b.d[i]) return d[i] < b.d[i];
        return 0;
    }
    BigF& operator+=(const BigF& b) {
        ull c = 0; u128 t;
        F0R (i, N) t = (u128) d[i] + b.d[i] + c,
            d[i] = (ull) t, c = (ull) (t >> 64);
        return *this;
    }
    BigF& operator-=(const BigF& b) { // wraps if *this < b
        ull c = 0; u128 t;
        F0R (i, N) t = (u128) d[i] - b.d[i] - c,
            d[i] = (ull) t, c = (ull) (t >> 64) & 1;
        return *this;
    }
    friend BigF operator+(BigF a, BigF b) { return a += b; }
    friend BigF operator-(BigF a, BigF b) { return a -= b; }
};
