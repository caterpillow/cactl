/**
 * Author: caterpillow
 * Date: 2025-09-12
 * License: CC0
 * Source: some yosupo
 * Description: Given $p/q$, find the shorter of its two unique continued fraction representations. 
 *  The other representation is given by \texttt{vec.back()--, vec.pb(1)};
 * Time: O(\log N)
 * Status: true
 */

vi cont_frac(int p, int q) {
    vi a;
    while (q) {
        a.push_back(p / q);
        tie(p, q) = make_pair(q, p % q);
    }
    return a;
}
