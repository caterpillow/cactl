/**
 * Author: caterpillow, Claude
 * Date: 2026-09-13
 * License: CC0
 * Source: libstdc++ <ext/rope>, https://codeforces.com/blog/entry/94213
 * Description: Persistent sequence: insert, erase and cut out a block at
 * any index in $O(\log n)$ amortized, 1.5--4$\mu$s each (an implicit treap
 * is 3--5x faster, a vector insert is as fast below $n \approx 2\cdot10^5$).
 * A copy is $O(1)$, \texttt{substr} and $+$ are $O(\log n)$, all sharing
 * nodes, so old versions stay valid: a persistent array is copy, then
 * write, at 2--5KB per version (fine for $10^5$, not $10^6$). \texttt{r[i]}
 * is a value, $O(\log n)$, 0.2--0.6$\mu$s at $10^6$; a range-for is
 * 2--17ns per step, but any mutation frees \texttt{begin()} iterators
 * (\texttt{mutable\_begin()} ones survive). Landmines: include
 * \texttt{<ext/rope>} above the template (the size macro breaks it); T
 * must not be a std type (pair, string), wrap it in a struct; a literal 0
 * is ambiguous with the pointer overloads unless T is int, so
 * \texttt{Rope<ll>} needs \texttt{0LL}; never the one-argument
 * \texttt{erase(i)}, it erases $i+1$ elements up to gcc 11.2 and does not
 * compile after. Benchmarks: doc/rope.md
 * Time: O(\log n) amortized
 * Status: stress-tested, benchmarked
 */
#pragma once

#include <ext/rope> /** keep-include */

template<class T>
using Rope = __gnu_cxx::rope<T>;

void example() {
    Rope<int> r;                     // Rope<int>(a, n) is faster
    F0R (i, 5) r.push_back(i);       // 0 1 2 3 4
    r.insert(1, 9);                  // 0 9 1 2 3 4
    r.erase(2, 3);                   // 0 9 4 (3 elements from 2)
    Rope<int> t = r.substr(1, 2);    // 9 4
    r = t + r; r.insert(1, t);       // 9 9 4 4 0 9 4
    r.mutable_reference_at(0) = 7;   // r[0] = 7; t is unchanged
    r.replace(0, 7);                 // same but copies the path
    assert(r[0] == 7 && t[0] == 9 && size(r) == 7);
    for (int x : r) cout << x;       // O(1) amortized per step
    t.replace(0, 1, r.substr(3, 2)); // t = 4 0 4
}
