/**
 * Author: caterpillow
 * Date: 2025-09-12
 * License: CC0
 * Source: some yosupo
 * Description: Given $p/q$ with $p \ge 0$, $q > 0$, find the shorter of its
 * two unique continued fraction representations. (Negative $p$ breaks:
 * C++ division truncates instead of flooring.) 
 *  The other representation is given by \texttt{a.back()--, a.pb(1)};
 * Time: O(\log N)
 * Status: true
 */

vl cont_frac(ll p, ll q) {
    vl a;
    while (q) {
        a.pb(p / q);
        tie(p, q) = make_pair(q, p % q);
    }
    return a;
}
