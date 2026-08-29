/**
 * Author: caterpillow, Claude
 * Date: 2026-08-29
 * License: CC0
 * Source: folklore
 * Description: Unsigned bigint of as many 64-bit limbs as it needs, little
 * endian and always trimmed, so no leading zero limb and $0$ is the empty
 * vector. \texttt{a -= b} requires $a \ge b$; comparison is unsigned.
 * \texttt{bit(i)} is the $2^i$ bit and reads $0$ past the end;
 * \texttt{set}/\texttt{flip} grow as needed, clearing past the end is a
 * no-op. Reach for this when the width is not bounded in advance; use
 * BigIntFixed.h when it is and speed matters. No multiplication, division
 * or decimal I/O; print the limbs in hex from the top down.
 * Time: O(L) per operation, $L$ = limbs.
 * Status: stress-tested against __uint128_t and a bitset oracle
 */
#pragma once

using ull = unsigned long long;
using u128 = __uint128_t;
struct Big {
    vt<ull> d; // limb i is bits [64i, 64i + 64)
    Big(ull x = 0) { if (x) d.pb(x); }
    void trim() { while (size(d) && !d.back()) d.pop_back(); }
    ull at(int i) const { return i < size(d) ? d[i] : 0; }
    bool bit(int i) const { return at(i / 64) >> (i % 64) & 1; }
    void flip(int i) {
        d.resize(max(size(d), i / 64 + 1));
        d[i / 64] ^= 1ULL << (i % 64);
        trim();
    }
    void set(int i, bool v = 1) { if (bit(i) != v) flip(i); }
    bool operator==(const Big& b) const { return d == b.d; }
    bool operator<(const Big& b) const {
        if (size(d) != size(b.d)) return size(d) < size(b.d);
        ROF (i, 0, size(d))
            if (d[i] != b.d[i]) return d[i] < b.d[i];
        return 0;
    }
    Big& operator+=(const Big& b) {
        ull c = 0; u128 t;
        d.resize(max(size(d), size(b.d)));
        F0R (i, size(d)) t = (u128) d[i] + b.at(i) + c,
            d[i] = (ull) t, c = (ull) (t >> 64);
        if (c) d.pb(1);
        return *this;
    }
    Big& operator-=(const Big& b) { // needs *this >= b
        ull c = 0; u128 t;
        F0R (i, size(d)) t = (u128) d[i] - b.at(i) - c,
            d[i] = (ull) t, c = (ull) (t >> 64) & 1;
        trim();
        return *this;
    }
    friend Big operator+(Big a, Big b) { return a += b; }
    friend Big operator-(Big a, Big b) { return a -= b; }
};
