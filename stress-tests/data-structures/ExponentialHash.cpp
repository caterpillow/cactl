#include "../utilities/template.h"
#include "../../content/data-structures/ExponentialHash.h"

using H = ExpHash<>;
using H2 = ExpHash<ll(1e11)+7>;
const ll P = H::P, M = P - 1;

ll norm(ll x, ll m) { return (x % m + m) % m; }
ll refmul(ll a, ll b, ll m = P) {
    return (ll)((__int128)a * b % m);
}
ll refpow(ll a, ull e) {
    ll r = 1;
    for (; e; e >>= 1, a = refmul(a, a))
        if (e & 1) r = refmul(r, a);
    return r;
}
template<class T> ll refpoint(ll z, ll c = 1) {
    return refpow(T::B, refmul(norm(z, M), norm(c, M), M));
}

template<class T> void test() {
    mt19937_64 rng(719);
    vl edge{LLONG_MIN, LLONG_MAX, -P, -M, -1, 0, 1, M, P};
    for (ll x : edge) for (ll c : edge) {
        assert(T(x).x == norm(x, P));
        assert(T::point(x, c).x == refpoint<T>(x, c));
    }
    F0R (it, 30000) {
        ll x = (ll)(rng() % (ull)P), y = (ll)(rng() % (ull)P);
        assert((T(x) + T(y)).x == (x + y) % P);
        assert((T(x) - T(y)).x == norm(x - y, P));
        assert((T(x) * T(y)).x == refmul(x, y));
        assert(T::mul(x, y) == refmul(x, y));
        ll z = (ll)(rng() % (ull)M), c = (ll)(rng() % (ull)M);
        assert(T::point(-z, c).x == refpoint<T>(-z, c));
        assert((T::point(z) * T::point(-z)) == T(1));
    }
    for (ll x : vl{0, 1, 2, P-2, P-1, 1LL<<60, (1LL<<61)-2})
        for (ll y : vl{0, 1, 2, P-2, P-1, 1LL<<60, (1LL<<61)-2})
            assert(T::mul(x, y) == refmul(x, y));
    F0R (it, 300) {
        vl a(rng() % 40);
        for (ll &v : a) v = (ll)rng() % 50 - 25;
        vt<T> pre(size(a) + 1);
        F0R (i, size(a)) pre[i+1] = pre[i] + T::point(a[i]);
        F0R (l, size(a) + 1) FOR (r, l, size(a) + 1) {
            T want;
            FOR (i, l, r) want = want + T::point(a[i]);
            assert(pre[r] - pre[l] == want);
        }
    }
    F0R (it, 10000) {
        ll a = (ll)(rng() % 2001) - 1000;
        ll d = (ll)(rng() % 201) - 100;
        ll n = (ll)(rng() % 30), k = (ll)(rng() % (ull)(n + 1));
        ll want = 0;
        F0R (i, n) want = (want + refpoint<T>(a + i * d)) % P;
        assert(T::range(a, d, n).x == want);
        assert(T::range(a, d, n) ==
            T::range(a, d, k) + T::range(a + k * d, d, n - k));
    }
    for (ll n : vl{0, 1, P-1, P, P+1, LLONG_MAX}) {
        assert(T::range(-5, 0, n) == T::point(-5) * T(n));
        assert(T::range(-5, M, n) == T::point(-5) * T(n));
        assert(T::range(0, 1, M) == T(0));
    }
    F0R (it, 1000) {
        vt<pl> s, t;
        F0R (i, 8) s.pb({(ll)(rng()%101)-50, (ll)(rng()%101)-50});
        F0R (i, 6) t.pb({(ll)(rng()%101)-50, (ll)(rng()%101)-50});
        ll c = ll(1e11)+13;
        auto point = [&](pl v) { return T::point(v.f) * T::point(v.s, c); };
        T hs, ht, sums, shifted;
        for (auto v : s) {
            hs = hs + point(v);
            shifted = shifted + point({v.f+17, v.s-29});
            for (auto w : t) sums = sums + point({v.f+w.f, v.s+w.s});
        }
        for (auto v : t) ht = ht + point(v);
        assert(hs * ht == sums);
        assert(hs * point({17, -29}) == shifted);
        assert(hs - point(s[0]) + point(s[0]) == hs);
        assert(hs * T(3) == hs + hs + hs);
        shuffle(all(s), rng);
        T permuted;
        for (auto v : s) permuted = permuted + point(v);
        assert(permuted == hs);
    }
}

int main() {
    ll rest = M;
    for (ll q : vl{2,3,5,7,11,13,31,41,61,151,331,1321}) {
        assert(rest % q == 0);
        while (rest % q == 0) rest /= q;
        assert(refpow(37, M/q) != 1);
        assert(refpow(H::B, M/q) != 1);
        assert(refpow(H2::B, M/q) != 1);
    }
    assert(rest == 1);
    for (ll c : vl{ll(1e11)+3,ll(1e11)+7,
            ll(1e11)+13,ll(1e11)+19,
            ll(1e11)+21,ll(1e11)+27}) assert(gcd(c, M) == 1);
    assert(H::B == refpow(37, ll(1e11)+3));
    assert(H2::B == refpow(37, ll(1e11)+7));
    assert(sizeof(H) == sizeof(ll));
    test<H>();
    test<H2>();
    assert(!(H::point(0)+H::point(4)+H::point(5) ==
        H::point(1)+H::point(2)+H::point(6)));
    cout << "Tests passed!\n";
}
