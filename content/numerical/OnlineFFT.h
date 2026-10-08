/**
 * Author: caterpillow
 * Date: 2026-10-05
 * License: CC0
 * Source: folklore (relaxed multiplication, one side known)
 * Description: Online convolution with a fixed kernel $b$: the $t$-th call
 * \texttt{push(a[t])} returns $c[t] = \sum_{i=0}^{t} a[i]\,b[t-i]$, so $a[t]$ may
 * depend on $c[0..t-1]$. E.g. for $f_{t+1} = \sum_{i \le t} f_i g_{t+1-i}$, use kernel
 * $g_1, g_2, \dots$; then \texttt{push}($f_t$) returns $f_{t+1}$.
 * At most $n$ pushes; the kernel is cut/padded to length $n$.
 * Any linear \texttt{conv} mod \texttt{mod} works in place of NTT's.
 * Values must be in $[0, \text{mod})$.
 * Usage: OnlineFFT o(n, kernel); o.push(x);
 * Time: O(N \log^2 N)
 * Status: stress-tested
 */
#pragma once

#include "NumberTheoreticTransform.h"

struct OnlineFFT {
    vl a, b, c;
    OnlineFFT(int n, vl kernel) : b(move(kernel)), c(n) {
        b.resize(n);
        a.reserve(n);
    }
    ll push(ll x) {
        int t = size(a), w = (t + 1) & -(t + 1);
        int len = min(w, size(c) - t);
        a.pb(x);
        auto d = conv(vl(a.end() - w, a.end()),
                      vl(b.begin(), b.begin() + w + len - 1));
        F0R (j, len) (c[t + j] += d[w - 1 + j]) %= mod;
        return c[t];
    }
};
