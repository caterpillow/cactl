/**
 * Author: Claude
 * Date: 2026-10-09
 * License: CC0
 * Source: folklore (read off $[x^{i-1}]$ of a first-order ODE)
 * Description: Power series mod $x^n$ for any prime \texttt{mod}, no
 *  FFT. Inputs: entries in $[0, \text{mod})$, any length. Outputs:
 *  exactly $n$ terms. \texttt{invSlow}: $1/a$, needs $a_0 \ne 0$;
 *  $b_0 = a_0^{-1}$, $b_i = -b_0 \sum_{j \ge 1} a_j b_{i-j}$. The
 *  rest need $n \le$ \texttt{mod} and read off $[x^{i-1}]$ of an ODE:
 *  \texttt{ode} sets $y_i = \frac{q}{i} \sum_{j=1}^{i} (Kj - Li)
 *  a_j z_{i-j}$ for $0 < i < |y|$; needs $q, K \in [0, \text{mod})$,
 *  $L \in \{0, 1\}$, $|z| \ge |y|$ ($z$ may be $y$).
 *  \texttt{expSlow}: $e^a$, needs $a_0 = 0$: $y_0 = 1$, $y' = a'y$,
 *  i.e.\ $i y_i = \sum_j j a_j y_{i-j}$. \texttt{logSlow}: $\ln a$,
 *  needs $a_0 = 1$: $y_0 = 0$, $y' = a'z$, $z = 1/a$.
 *  \texttt{powSlow}: $a^k$, $0 \le k \le 10^{18}$, leading zeros ok,
 *  $0^0 = 1$. With $a = x^t c$, $c_0 \ne 0$: $a^k = x^{tk} c^k$, and
 *  $c y' = k c' y$ gives $i c_0 y_i = \sum_j ((k+1)j - i) c_j
 *  y_{i-j}$, $y_0 = c_0^k$. If $a_0 = 1$, $k$ only matters modulo
 *  \texttt{mod}: $k = (\text{mod}+1)/2$ gives $\sqrt a$ (the root with
 *  $y_0 = 1$; else $\sqrt a = \sqrt{a_0} \sqrt{a/a_0}$, ModSqrt.h).
 * Usage: vl c = powSlow({1, 1, 1}, k, n); // (1+x+x^2)^k
 *  vl r = powSlow(a, (mod + 1) / 2, n); // sqrt(a), a[0] = 1
 * Time: O(nd), $d = \min(n, |a|)$; $n = d = 5000$: 0.03\,s
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

vl invSlow(const vl& a, int n) { // a b = 1
    vl b(n); ll q = mpow(a[0]);
    F0R (i, n) { ll x = 0;
        FOR (j, 1, min(i + 1, size(a))) x += a[j] * b[i - j] % mod;
        b[i] = (!i + mod - x % mod) * q % mod;
    }
    return b;
} // <hash>

void ode(const vl& a, vl& y, const vl& z, ll q, ll K, ll L) {
    vl v(size(y), 1); // v[i] = 1/i
    FOR (i, 2, size(y)) v[i] = mod - mod / i * v[mod % i] % mod;
    FOR (i, 1, size(y)) { ll x = 0;
        FOR (j, 1, min(i + 1, size(a))) x += (K * j - L * i + mod)
            % mod * a[j] % mod * z[i - j] % mod;
        y[i] = x % mod * q % mod * v[i] % mod;
    }
} // <hash>

vl expSlow(const vl& a, int n) { // y' = a' y
    vl y(n); if (n) y[0] = 1;
    ode(a, y, y, 1, 1, 0); return y;
} // <hash>

vl logSlow(const vl& a, int n) { // y' = a' / a
    vl y(n); ode(a, y, invSlow(a, n), 1, 1, 0); return y;
} // <hash>

vl powSlow(const vl& a, ll k, int n) { // a y' = k a' y
    int t = 0, d = size(a);
    while (t < d && !a[t]) t++;
    vl y(n); if (n) y[0] = !k;
    if (!k || t == d || min(k, (ll)n) * t >= n) return y;
    vl c(a.begin() + t, a.end()), r(n - k * t);
    r[0] = mpow(c[0], k);
    ode(c, r, r, mpow(c[0]), (k + 1) % mod, 1);
    copy(all(r), y.begin() + k * t); return y;
} // <hash>
