/**
 * Author: wery0
 * Date: 2025-10-24
 * Source: wery0
 * Description: Wildcard pattern matching. wc is wildcard character.
 * res[i] = '1' iff pattern matches text starting at i. Idea: give every
 * character a random value $r$; over the non-wildcard pairs a match means
 * $\sum_j r(p_j)(r(p_j) - r(t_{i+j})) = 0$, which two convolutions
 * evaluate for all $i$. A mismatch survives with probability $\le 2/mod$
 * per alignment, so \tilde{}$2N/mod$ overall; to square that, run again
 * with fresh random values (re-seed rng) or another wpm\_mod and AND.
 * Time: $O(N \log N)$
 */

#pragma once

#include "../numerical/FastFourierTransformMod.h"

const int wpm_mod = (119 << 23) + 1;
template<class T_arr, class T>
string wildcard_pattern_matching(const T_arr& text, const T_arr& pattern, T wc) {
    static mt19937_64 rng;
    int N = size(text), M = size(pattern);
    if (N == 0 || M > N) return "";
    assert(M);
    map<T, ll> char_to_rnd;
    auto f = [&] (T x) { ll& r = char_to_rnd[x]; 
        if (!r) r = rng() % (wpm_mod - 1) + 1; return r; };
    vl mt(N), mp(M);
    F0R (i, N) mt[i] = text[i] == wc ? 0 : f(text[i]);
    F0R (i, M) mp[M - 1 - i] = pattern[i] == wc ? 0 : f(pattern[i]);
    vl sc1 = convMod<wpm_mod>(mp, mt);
    F0R (i, N) mt[i] = text[i] == wc;
    ll must = 0;
    for (ll& x : mp) x = x * x % wpm_mod, must = (must + x) % wpm_mod;
    vl sc2 = convMod<wpm_mod>(mp, mt);
    string res(N - M + 1, '0');
    FOR (i, M - 1, N) res[i + 1 - M] += (sc1[i] + sc2[i]) % wpm_mod == must;
    return res;
}