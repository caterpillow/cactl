// Tests OnlineFFT.h: push() outputs vs O(n^2) brute force for random n in
// 1..300 (kernel shorter/longer than n), and an online recurrence where a[t]
// depends on c[t-1]. Written by Claude (audit).
#include "../utilities/template.h"

namespace ignore {
#include "../../content/number-theory/ModPow.h"
}
const ll mod = 998244353;
ll mpow(ll b, ll e) {
    ll ans = 1;
    for (; e; b = b * b % mod, e /= 2) if (e & 1) ans = ans * b % mod;
    return ans;
}
#include "../../content/numerical/OnlineFFT.h"

int main() {
    mt19937 rng(7);
    F0R (it, 500) {
        int n = rng() % 300 + 1, m = rng() % 400;
        vl a(n), b(m);
        for (ll &x : a) x = rng() % mod;
        for (ll &x : b) x = rng() % mod;
        OnlineFFT o(n, b);
        F0R (t, n) {
            ll want = 0;
            F0R (i, t + 1) if (t - i < m) want = (want + a[i] * b[t - i]) % mod;
            assert(o.push(a[t]) == want);
        }
    }
    // f_0 = 1, f_{t+1} = sum_{i<=t} f_i g_{t+1-i}
    int n = 2000;
    vl g(n + 1), f(n + 1);
    for (ll &x : g) x = rng() % mod;
    f[0] = 1;
    FOR (t, 1, n + 1) F0R (i, t) f[t] = (f[t] + f[i] * g[t - i]) % mod;
    OnlineFFT o(n, vl(g.begin() + 1, g.end()));
    ll cur = 1;
    F0R (t, n) assert((cur = o.push(cur)) == f[t + 1]);
    cout << "Tests passed!" << endl;
}
