/**
 * Author: Simon Lindholm
 * Date: 2018-07-06
 * License: CC0
 * Description: Permutation of $0..n-1$ ($n \le 12$) -> integer in $[0,n!)$. (Not order preserving.)
 * Integer -> permutation can use a lookup table.
 * Time: O(n)
 */
#pragma once

int permToInt(vi &v) {
    int use = 0, i = 0, r = 0;
    for (int x : v) r = r * ++i + __builtin_popcount(use & -(1 << x)),
        use |= 1 << x; // (note: minus, not ~!)
    return r;
}
vi intToPerm(int r, int n) {
    vi a(n), v(n); iota(all(a), 0);
    for (int i = n; i; i--) {
        int c = r % i; r /= i;
        v[i - 1] = a[i - 1 - c];
        a.erase(begin(a) + i - 1 - c);
    }
    return v;
}
