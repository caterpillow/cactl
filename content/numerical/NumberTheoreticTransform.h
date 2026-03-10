/**
 * Author: chilli
 * Date: 2019-04-16
 * License: CC0
 * Source: based on KACTL's FFT
 * Description: ntt(a) computes $\hat f(k) = \sum_x a[x] g^{xk}$ for all $k$, where $g=\text{root}^{(mod-1)/N}$.
 * N must be a power of 2.
 * Useful for convolution modulo specific nice primes of the form $2^a b+1$,
 * where the convolution result has size at most $2^a$. For arbitrary modulo, see FFTMod.
   \texttt{conv(a, b) = c}, where $c[x] = \sum a[i]b[x-i]$.
   For manual convolution: NTT the inputs, multiply
   pointwise, divide by n, reverse(start+1, end), NTT back.
 * Inputs must be in [0, mod).
 * Time: O(N \log N) with $N = |A|+|B|$ ($\tilde 0.2s$ for $N=2^{20}$)
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

const ll root = 62; // = 998244353
// For p < 2^30 there is also e.g. 5 << 25, 7 << 26, 479 << 21
// and 483 << 21 (same root). The last two are > 10^9.

template<class T>
void ntt(vt<T> &a) {
    int n = size(a), L = 31 - __builtin_clz(n);
    static vt<ll> rt(2, 1);
    for (static int k = 2, s = 2; k < n; k *= 2, s++) {
        rt.resize(n);
        ll z[] = {1, mpow(root, mod >> s)};
        FOR (i, k, 2 * k) rt[i] = rt[i / 2] * z[i & 1] % mod;
    }
    vi rev(n);
    F0R (i, n) rev[i] = (rev[i / 2] | (i & 1) << L) / 2;
    F0R (i, n) if (i < rev[i]) swap(a[i], a[rev[i]]);
    for (int k = 1; k < n; k *= 2)
        for (int i = 0; i < n; i += 2 * k) FOR (j, 0, k) {
            T z = (ll) rt[j + k] * a[i + j + k] % mod, &ai = a[i + j];
            a[i + j + k] = ai - z + (z > ai ? mod : 0);
            ai += (ai + z >= mod ? z - mod : z);
        }
}

template<class T>
vt<T> conv(const vt<T> &a, const vt<T> &b) {
    if (a.empty() || b.empty()) return {};
    int s = size(a) + size(b) - 1, B = 32 - __builtin_clz(s),
        n = 1 << B;
    int inv = mpow(n, mod - 2);
    vt<T> L(a), R(b), out(n);
    L.resize(n), R.resize(n);
    ntt(L), ntt(R);
    F0R (i, n) out[-i & (n - 1)] = (ll) L[i] * R[i] % mod * inv % mod;
    ntt(out);
    return {out.begin(), out.begin() + s};
}
