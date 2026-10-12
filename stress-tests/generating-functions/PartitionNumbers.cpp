// Tests PartitionNumbers.h: partitions(n) vs the O(n^2) coin-change DP
// (parts 1, 2, ...) for every n <= 130, around powers of two and up to
// 3000, under prime, composite, tiny and ~2^61 moduli; vs exact values
// (__int128 DP) reduced mod each modulus; p(0..10) and p(100).
// mod is a runtime variable here so one binary covers every modulus.
// written by Claude
#include "../utilities/template.h"

namespace unused { // ModPow.h's const mod; the header sees ::mod below
#include "../../content/number-theory/ModPow.h"
}
ll mod = 1000000007;
#include "../../content/generating-functions/PartitionNumbers.h"

vl coinDP(int n) { // number of multisets of parts summing to i
    vl dp(n);
    if (n) dp[0] = 1 % mod;
    FOR (k, 1, n) FOR (j, k, n) dp[j] = (dp[j] + dp[j - k]) % mod;
    return dp;
}

int main() {
    // exact small values under 1e9+7
    assert(partitions(0).empty());
    assert(partitions(1) == vl{1});
    assert(partitions(2) == vl({1, 1}));
    assert(partitions(11) == vl({1, 1, 2, 3, 5, 7, 11, 15, 22, 30, 42}));
    assert(partitions(101)[100] == 190569292);

    // exact p(i) for i < 1000 (p(999) ~ 2.4e31 < 2^127)
    const int E = 1000;
    vt<__int128> ex(E);
    ex[0] = 1;
    FOR (k, 1, E) FOR (j, k, E) ex[j] += ex[j - k];
    assert(ex[100] == 190569292);
    assert(ex[200] == (__int128)3972999029388LL);

    vi sizes;
    F0R (n, 131) sizes.pb(n);
    for (int b = 8; b <= 2048; b *= 2) for (int d : {-1, 0, 1}) sizes.pb(b + d);
    mt19937_64 rng(7);
    F0R (it, 20) sizes.pb(int(rng() % 3001));
    sizes.pb(3000);

    vl mods = {1000000007, 998244353, 1000000000, 2, 3, 4, 6, 7, 12,
               97, 1LL << 31, 3000000000LL, (1LL << 61) - 1,
               1LL << 60};
    for (ll m : mods) {
        mod = m;
        vl want = coinDP(3001);
        F0R (i, E) assert(want[i] == ll(ex[i] % (__int128)mod));
        for (int n : sizes) {
            vl got = partitions(n);
            assert(size(got) == n);
            assert(got == vl(want.begin(), want.begin() + n));
        }
    }
    cout << "Tests passed!" << endl;
}
