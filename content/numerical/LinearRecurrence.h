/**
 * Author: Lucian Bicsi
 * Date: 2018-02-14
 * License: CC0
 * Source: Chinese material
 * Description: Generates the $k$'th term of an $n$-order
 * linear recurrence $S[i] = \sum_j S[i-j-1]tr[j]$,
 * given $S[0 \ldots \ge n-1]$ and $tr[0 \ldots n-1]$.
 * Faster than matrix multiplication.
 * Useful together with Berlekamp--Massey.
 * Usage: linearRec({0, 1}, {1, 1}, k) // k'th Fibonacci number
 *  vl s; F0R (i, 40) s.pb(brute(i) % mod); // >= 2n terms
 *  vl c = berlekampMassey(s); // check 2 size(c) < size(s)
 *  ll x = linearRec(s, c, k); // s[k], k up to 1e18
 * Time: O(n^2 \log k)
 * Status: bruteforce-tested mod 5 for n <= 5
 */
#pragma once

const ll mod = 5; /** exclude-line */

using Poly = vt<ll>;
ll linearRec(Poly S, Poly tr, ll k) {
    int n = size(tr);
    assert(size(S) >= n);
    if (!n) return 0;

    auto combine = [&] (Poly a, Poly b) {
        Poly res(n * 2 + 1);
        F0R (i, n + 1) F0R (j, n + 1)
            res[i + j] = (res[i + j] + a[i] * b[j]) % mod;
        for (int i = 2 * n; i > n; --i) F0R (j, n)
            res[i - 1 - j] = (res[i - 1 - j] + res[i] * tr[j]) % mod;
        res.resize(n + 1);
        return res;
    };

    Poly pol(n + 1), e(pol);
    pol[0] = e[1] = 1;

    for (++k; k; k /= 2) {
        if (k % 2) pol = combine(pol, e);
        e = combine(e, e);
    }

    ll res = 0;
    F0R (i, n) res = (res + pol[i + 1] * S[i]) % mod;
    return (res + mod) % mod;
}
