// Tests NumberTheoreticTransform.h: conv() vs O(n*m) brute force (random
// sizes 0..300 plus one 2000x2000, so the static root table grows and shrinks),
// ntt() vs the DFT definition sum a[x] g^{xk} with g = root^((mod-1)/n), and a
// hand-checked value. Written by Claude (audit).
#include "../utilities/template.h"

// NTT.h #includes ModPow.h (mod = 1e9+7) but its root = 62 is for 998244353:
// consume ModPow.h's #pragma once in a dummy namespace, then provide mod/mpow
// for the NTT prime at global scope before including the header.
namespace ignore {
#include "../../content/number-theory/ModPow.h"
}
const ll mod = 998244353;
ll mpow(ll b, ll e) {
    ll ans = 1;
    for (; e; b = b * b % mod, e /= 2) if (e & 1) ans = ans * b % mod;
    return ans;
}
#include "../../content/numerical/NumberTheoreticTransform.h"

vl brute(const vl &a, const vl &b) {
    if (a.empty() || b.empty()) return {};
    vl c(size(a) + size(b) - 1);
    F0R (i, size(a)) F0R (j, size(b)) c[i + j] = (c[i + j] + a[i] * b[j]) % mod;
    return c;
}

int main() {
    mt19937 rng(12345);
    auto rnd = [&](int lo, int hi) { return (int) (rng() % (hi - lo + 1)) + lo; };
    auto randVec = [&](int n) {
        vl v(n);
        for (ll &x : v) x = (ll) (rng() % mod);
        return v;
    };

    assert(conv(vl{1, 2, 3}, vl{4, 5, 6}) == (vl{4, 13, 28, 27, 18}));
    assert(conv(vl{}, vl{1}).empty() && conv(vl{7}, vl{}).empty());
    assert(conv(vl{mod - 1}, vl{mod - 1}) == vl{1});

    F0R (it, 400) {
        vl a = randVec(rnd(0, 300)), b = randVec(rnd(0, 300));
        assert(conv(a, b) == brute(a, b));
    }
    {
        vl a = randVec(2000), b = randVec(2000);
        assert(conv(a, b) == brute(a, b));
    }
    assert(conv(vl{1, 2, 3}, vl{4, 5, 6}) == (vl{4, 13, 28, 27, 18})); // after growth

    // forward transform against the definition
    for (int n = 1; n <= 64; n *= 2) F0R (it, 3) {
        vl a = randVec(n), f = a;
        ntt(f);
        ll g = mpow(root, (mod - 1) / n);
        F0R (k, n) {
            ll sum = 0;
            F0R (x, n) sum = (sum + a[x] * mpow(g, (ll) x * k % n)) % mod;
            assert(sum == f[k]);
        }
    }
    cout << "Tests passed!" << endl;
}
