/**
 * Author: Simon Lindholm
 * Date: 2015-03-15
 * License: CC0
 * Source: own work
 * Description: $2^{64} - 1$ mod int
 * Status: stress-tested
 */
#pragma once

// Arithmetic mod 2^64-1. 2x slower than mod 2^64 and more
// code, but works on evil test data (e.g. Thue-Morse, where
// ABBA... and BAAB... of length 2^10 hash the same mod 2^64).
// "typedef ull H;" instead if you think test data is random,
// or work mod 10^9+7 if the Birthday paradox is not a problem.
using ull = unsigned long long;
struct H {
    ull x; H(ull x = 0) : x(x) {}
    H operator+(H o) { return x + o.x + (x + o.x < x); }
    H operator-(H o) { return *this + ~o.x; }
    H operator*(H o) { auto m = (__uint128_t) x * o.x;
        return H((ull) m) + (ull)(m >> 64); }
    ull get() const { return x + !~x; }
    #define op(o) bool operator o (H oth) const { return get() o oth.get(); }
    op(==) op(<)
};

static const H C = (ll) 1e11 + 3; // (order ~ 3e9; random also ok)