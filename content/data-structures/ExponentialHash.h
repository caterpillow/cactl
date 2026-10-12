/**
 * Author: caterpillow, OpenAI
 * Date: 2026-10-09
 * License: CC0
 * Source: folklore
 * Description: Multiset hash $H(S)=\sum_{x\in S}B^x\bmod p$, $p=2^{61}-1$,
 *  $B=37^K$, a primitive root iff $\gcd(K,p-1)=1$. $\pm$ adds/removes elements,
 *  $H(S)H(T)$ hashes all pairwise sums, $H\cdot$\texttt{point(t)} translates by $t$.
 *  \texttt{range(a,d,n)}: $\{a,a+d,\ldots,a+(n-1)d\}$. Can do prefix hashes. For vectors,
 *  dot against fixed vector of random coprimes: $\prod_i$\texttt{point}$(x_i,c_i)$
 *  with $c_i$ coprime to $p-1$, e.g. $10^{11}+\{3,7,13,19,21,27\}$.
 *  Exponents wrap mod $p-1$. For uniformly random $B,c_i$: collision prob.
 *  $\le D/\varphi(p-1)$, $\varphi(p-1)\approx4\cdot10^{17}$, $D=$ sum of coordinate
 *  spans. Double hash: another $K$ with different $c_i$.
 * Usage:
 *  using H = ExpHash<>;
 *  H h = H::point(2) + H::point(5); // multiset {2,5}
 *  h = h * H::point(-3); // translate to {-1,2}
 *  H xy = H::point(x) * H::point(y, ll(1e11)+13);
 * Time: O(1) arithmetic/point; O(\log p+\log(n+1)) range.
 * Memory: O(1) per hash; shared compile-time power table.
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModMulLL.h"
#include "../number-theory/FixedModPow.h"

template<ll K = ll(1e11)+3>
struct ExpHash {
    static constexpr ll P = (1LL << 61) - 1;
    static_assert(0 < K && K < P-1 && gcd(K, P-1) == 1);
    static constexpr ll B = FixedModPow<P,1>(37)(K);
    static constexpr FixedModPow<P> pw{B};
    ll x;
    ExpHash(ll v = 0) : x(v % P) { if (x < 0) x += P; }
    static ExpHash point(ll z, ll c = 1) {
        constexpr ll m = P-1;
        z %= m; if (z < 0) z += m;
        c %= m; if (c < 0) c += m;
        ExpHash r;
        r.x = pw((ull)((__uint128_t)z * c % m)); return r;
    } // <hash>
    // faster member operation impls
    ExpHash operator+(ExpHash o) const {
        o.x += x; if (o.x >= P) o.x -= P; return o;
    }
    ExpHash operator-(ExpHash o) const {
        o.x = x-o.x; o.x += o.x >> 63 & P; return o;
    }
    ExpHash operator*(ExpHash o) const {
        o.x = mul(x, o.x); return o;
    }
    bool operator==(ExpHash o) const { return x == o.x; } // <hash>
    static ExpHash range(ll a, ll d, ll n) {
        ExpHash q = point(d);
        return point(a) * (q.x == 1 ? ExpHash(n) :
            ExpHash(mpow(q.x, n, P)-1) *
            ExpHash(mpow(q.x-1, P-2, P)));
    }
    static ll mul(ll a, ll b) { // 0 <= a, b < P; faster 
        auto z = (__uint128_t)a * b;
        ll r = ll(z & P) + ll(z >> 61);
        return r >= P ? r - P : r;
    }
};
