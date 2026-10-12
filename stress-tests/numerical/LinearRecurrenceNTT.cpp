// Tests LinearRecurrenceNTT.h (Bostan-Mori): vs direct iteration for k < 3000,
// vs the O(n^2 log k) Kitamasa of LinearRecurrence.h (copied, same mod) for
// k up to 1e18, n = 0..60 plus n = 2000, and S longer than tr.
// written by Claude (audit)
#include "../utilities/template.h"

namespace ignore { // NTT.h needs mod = 998244353, not ModPow's 1e9+7
#include "../../content/number-theory/ModPow.h"
}
const ll mod = 998244353;
ll mpow(ll b, ll e) {
    ll ans = 1;
    for (; e; b = b * b % mod, e /= 2) if (e & 1) ans = ans * b % mod;
    return ans;
}
#include "../../content/numerical/LinearRecurrenceNTT.h"

ll kitamasa(vl S, vl tr, ll k) { // LinearRecurrence.h
    int n = size(tr);
    if (!n) return 0;
    auto combine = [&](vl a, vl b) {
        vl res(n * 2 + 1);
        F0R (i, n + 1) F0R (j, n + 1) res[i + j] = (res[i + j] + a[i] * b[j]) % mod;
        for (int i = 2 * n; i > n; --i) F0R (j, n) res[i - 1 - j] = (res[i - 1 - j] + res[i] * tr[j]) % mod;
        res.resize(n + 1);
        return res;
    };
    vl pol(n + 1), e(pol);
    pol[0] = e[1] = 1;
    for (++k; k; k /= 2) {
        if (k % 2) pol = combine(pol, e);
        e = combine(e, e);
    }
    ll res = 0;
    F0R (i, n) res = (res + pol[i + 1] * S[i]) % mod;
    return res;
}

int main() {
    mt19937_64 rng(5);
    auto rnd = [&](int n) { vl v(n); for (ll& x : v) x = ll(rng() % mod); return v; };
    F0R (it, 300) {
        int n = int(rng() % 12);
        vl tr = rnd(n), S = rnd(n + int(rng() % 3));
        vl s(S.begin(), S.begin() + n);
        F0R (k, 3000) {
            if (k >= n) {
                ll v = 0;
                F0R (j, n) v = (v + tr[j] * s[k - j - 1]) % mod;
                s.pb(v);
            }
            if (k < n) s.resize(max(size(s), k + 1)), s[k] = S[k];
            if (k % 97 == 0 || k < 20) assert(linearRecNTT(S, tr, k) == s[k]);
        }
    }
    F0R (it, 200) {
        int n = int(rng() % 61);
        vl tr = rnd(n), S = rnd(n);
        ll k = ll(rng() >> 4);
        assert(linearRecNTT(S, tr, k) == kitamasa(S, tr, k));
    }
    vl tr = rnd(2000), S = rnd(2000);
    ll k = ll(1e18);
    assert(linearRecNTT(S, tr, k) == kitamasa(S, tr, k));
    assert(linearRecNTT({0, 1}, {1, 1}, 10) == 55);
    cout << "Tests passed!" << endl;
}
