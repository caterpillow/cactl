/**
 * Author: Unknown
 * Date: 2014-11-27
 * Source: somewhere on github
 * Description: Calculates determinant using modular arithmetics.
 * Modulos can also be removed to get a pure-integer version.
 * Pass entries already reduced into $(-mod, mod)$ and keep
 * $mod < \tilde{}3e9$: intermediates reach $mod^2$.
 * Time: $O(N^3)$
 * Status: bruteforce-tested for N <= 3, mod <= 7
 */
#pragma once

// needs a global ll mod, e.g. from "../number-theory/ModPow.h"
ll det(vt<vl> &a) {
    int n = size(a); ll ans = 1;
    F0R (i, n) {
        FOR (j, i + 1, n) {
            while (a[j][i] != 0) { // gcd step
                ll t = a[i][i] / a[j][i];
                if (t) FOR (k, i, n)
                    a[i][k] = (a[i][k] - a[j][k] * t) % mod;
                swap(a[i], a[j]);
                ans *= -1;
            }
        }
        ans = ans * a[i][i] % mod;
        if (!ans) return 0;
    }
    return (ans + mod) % mod;
}
