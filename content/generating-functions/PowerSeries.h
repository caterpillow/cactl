/**
 * Author: Claude
 * Date: 2026-10-09
 * License: CC0
 * Source: folklore (Newton iteration on power series)
 * Description: Power series mod $x^n$. Inputs: entries in
 *  $[0, \text{mod})$, any length (missing terms are 0); outputs:
 *  exactly $n$ terms. \texttt{polyInv}: $1/a$, needs $a_0 \ne 0$;
 *  Newton $b \gets b(2 - ab)$ doubles the number of correct terms.
 *  \texttt{polyLog}: $\ln a = \int a'/a$, needs $a_0 = 1$.
 *  \texttt{polyExp}: $e^a$, needs $a_0 = 0$ ($a$ may be empty);
 *  Newton $b \gets b(1 - \ln b + a)$. \texttt{polyPow}: $a^k$ for any
 *  $a$ and any $k \ge 0$ ($0^0 = 1$): if $a = c x^t(1 + \ldots)$ then
 *  $a^k = c^k x^{tk} \exp(k \ln \frac{a}{c x^t})$, with $k$ taken mod
 *  \texttt{mod} inside the exp. \texttt{polySqrt}: the root with
 *  $b_0 = 1$, needs $a_0 = 1$ (else scale by $\sqrt{a_0}$, ModSqrt.h).
 *  Needs \texttt{mod} = 998244353 (edit ModPow.h) and $n \le 2^{22}$
 *  (else silently wrong). For another prime, include
 *  FastFourierTransformMod.h and replace \texttt{conv} by
 *  \texttt{convMod<mod>}; keep $n \le$ \texttt{mod}.
 * Usage: vl fib = polyInv({1, mod - 1, mod - 1}, n); // 1 1 2 3 5
 *  vl g = polySqrt({1, mod - 4}, n); // g[i>0] = -2 Catalan(i-1)
 * Time: O(n \log n); $n = 10^6$: inv 0.4\,s, exp 1.3\,s, pow 2\,s
 * Status: stress-tested
 */
#pragma once

#include "../numerical/NumberTheoreticTransform.h"

vl cut(vl a, int n) { a.resize(n); return a; } // <hash>

vl polyInv(const vl& a, int n) {
    assert(a[0]);
    vl b{mpow(a[0])};
    for (int m = 1; m < n; m *= 2) {
        vl c = cut(conv(cut(a, 2 * m), conv(b, b)), 2 * m);
        b.resize(2 * m); // b = 2b - ab^2
        F0R (i, 2 * m) b[i] = (2 * b[i] - c[i] + mod) % mod;
    }
    return cut(b, n);
} // <hash>

vl polyLog(const vl& a, int n) {
    assert(a[0] == 1);
    vl d(n), r(n), iv(n, 1);
    FOR (i, 1, min(n, size(a))) d[i - 1] = a[i] * i % mod; // a'
    d = conv(d, polyInv(a, n));
    FOR (i, 2, n) iv[i] = (mod - mod / i) * iv[mod % i] % mod;
    FOR (i, 1, n) r[i] = d[i - 1] * iv[i] % mod;
    return r;
} // <hash>

vl polyExp(const vl& a, int n) {
    assert(a.empty() || !a[0]);
    vl b{1};
    for (int m = 1; m < n; m *= 2) {
        vl c = cut(a, 2 * m), l = polyLog(b, 2 * m);
        F0R (i, 2 * m) c[i] = (c[i] - l[i] + mod) % mod;
        c[0]++; // c = 1 + a - ln b
        b = cut(conv(b, c), 2 * m);
    }
    return cut(b, n);
} // <hash>

vl polyPow(vl a, ll k, int n) {
    if (!k) a = {1};
    int t = 0; while (t < size(a) && !a[t]) t++;
    if (t == size(a) || (t && k > (n - 1) / t)) return vl(n);
    int m = n - int(t * k); // t * k < n: no overflow
    ll c = a[t], ic = mpow(c), ck = mpow(c, k);
    vl b(a.begin() + t, a.end()), r(n);
    for (ll& x : b) x = x * ic % mod;
    b = polyLog(b, m); for (ll& x : b) x = x * (k % mod) % mod;
    b = polyExp(b, m);
    F0R (i, m) r[n - m + i] = b[i] * ck % mod;
    return r;
} // <hash>

vl polySqrt(const vl& a, int n) {
    assert(a[0] == 1); return polyPow(a, (mod + 1) / 2, n);
} // <hash>
