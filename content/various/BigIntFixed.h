/**
 * Author: caterpillow, Claude
 * Date: 2026-08-30
 * License: CC0
 * Source: folklore
 * Description: Unsigned bigint of exactly $N$ 64-bit limbs, stored most
 * significant first: \texttt{array}'s lexicographic comparison is then
 * numeric comparison, so all six operators come from the base class and
 * only $+$ and $-$ are written here. Every operation is mod $2^{64N}$: a
 * subtraction that would go negative wraps instead of failing (add and
 * subtract also match two's complement, comparison does not).
 * \texttt{bit(i)} is the $2^i$ bit, $i$ in $[0, 64N)$; nothing is checked.
 * Reach for this when a bound on the width is known and speed matters --
 * the limbs sit in the object, nothing is allocated. Use BigInt.h when the
 * length has to grow. No multiplication, division or decimal I/O; print the
 * limbs in hex from the front.
 * Time: O(N) per operation.
 * Status: stress-tested against __uint128\_t and a bitset oracle
 */
#pragma once

using ull = unsigned long long;
using u128 = __uint128_t;
template <int N> struct BigF : array<ull, N> { // limb 0 is the top limb
    BigF(ull x = 0) : array<ull, N>{} { (*this)[N - 1] = x; }
    bool bit(int i) const { return (*this)[N-1 - i/64] >> (i%64) & 1; }
    void flip(int i) { (*this)[N-1 - i/64] ^= 1ULL << (i%64); }
    void set(int i, bool v = 1) { if (bit(i) != v) flip(i); }
    BigF& operator+=(const BigF& b) {
        ull c = 0; u128 t;
        ROF (i, 0, N) t = (u128) (*this)[i] + b[i] + c,
            (*this)[i] = (ull) t, c = (ull) (t >> 64);
        return *this;
    }
    BigF& operator-=(const BigF& b) { // wraps if *this < b
        ull c = 0; u128 t;
        ROF (i, 0, N) t = (u128) (*this)[i] - b[i] - c,
            (*this)[i] = (ull) t, c = (ull) (t >> 64) & 1;
        return *this;
    }
    friend BigF operator+(BigF a, BigF b) { return a += b; }
    friend BigF operator-(BigF a, BigF b) { return a -= b; }
};
