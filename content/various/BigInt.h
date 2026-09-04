/**
 * Author: caterpillow, Claude
 * Date: 2026-08-30
 * License: CC0
 * Source: folklore
 * Description: Unsigned bigint of exactly $N$ 64-bit limbs, stored most
 * significant first
 * Time: O(N) per operation.
 * Status: stress-tested against __uint128\_t and a bitset oracle
 */
#pragma once

using ull = unsigned long long;
using u128 = __uint128_t;
template <int N> struct Big : array<ull, N> {
    Big(ull x = 0) : array<ull, N>{} { (*this)[N - 1] = x; }
    Big& operator+=(const Big& b) {
        ull c = 0; u128 t;
        ROF (i, 0, N) t = (u128) (*this)[i] + b[i] + c,
            (*this)[i] = (ull) t, c = (ull) (t >> 64);
        return *this;
    }
    Big operator-() const {
        auto r = *this;
        for (ull &x : r) x = ~x;
        return r += 1;
    }
    Big& operator-=(const Big &b) { return (*this) += -b; }
    friend Big operator+(Big a, const Big& b) { return a += b; }
    friend Big operator-(Big a, const Big& b) { return a -= b; }
    bool bit(int i) const { return (*this)[N-1 - i/64] >> (i%64) & 1; }
    void flip(int i) { (*this)[N-1 - i/64] ^= 1ULL << (i%64); }
    void set(int i, bool v = 1) { if (bit(i) != v) flip(i); }
};