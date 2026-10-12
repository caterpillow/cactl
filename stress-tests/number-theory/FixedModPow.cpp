#include "../utilities/template.h"
#include "../../content/number-theory/FixedModPow.h"

using ull = unsigned long long;
template<ll M> ll reference(ll b, ull e) {
    ll r = 1;
    for (; e; e >>= 1, b = (ll)((__int128)b*b % M))
        if (e&1) r = (ll)((__int128)r*b % M);
    return r;
}
template<ll M, int L, int B = 64> void test() {
    mt19937_64 rng(812);
    ull mask = B == 64 ? ULLONG_MAX : (1ULL<<B) - 1;
    for (ll b : vl{0, 1, M-1, (ll)(rng() % M)}) {
        auto pw = make_unique<FixedModPow<M,L,B>>(b);
        for (ull e : {0ULL, 1ULL, 255ULL, 256ULL, 65535ULL,
                65536ULL, 1ULL<<63, ULLONG_MAX})
            assert((*pw)(e & mask) == reference<M>(b, e & mask));
        F0R (it, 5000) {
            ull e = rng() & mask;
            assert((*pw)(e) == reference<M>(b, e));
        }
    }
}
int main() {
    test<(1LL<<61)-1,8>();
    test<(1LL<<61)-1,16>();
    test<1000000007,8>();
    test<1000000007,7>();
    test<2,1>();
    test<100,16>();
    test<LLONG_MAX,8>();
    test<1000000007,8,30>();
    test<1000000007,16,30>();
    test<998244353,10,20>();
    test<(1LL<<61)-1,8,61>();
    test<(1LL<<61)-1,7,1>();
    test<(1LL<<61)-1,16,63>();
    cout << "Tests passed!\n";
}
