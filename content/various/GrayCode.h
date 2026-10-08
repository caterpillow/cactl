/**
 * Author: caterpillow
 * License: CC0
 * Source: cp algorithms
 * Description: \texttt{g(i)} is the $i$-th Gray code, \texttt{rev\_g} its inverse. \texttt{g(i)} and \texttt{g(i+1)} differ in bit $\mathrm{ctz}(i+1)$ only.
 */
#pragma once
int g(int n) { return n ^ (n >> 1); }

int rev_g (int g) {
    int n = 0;
    for (; g; g >>= 1)
        n ^= g;
    return n;
}