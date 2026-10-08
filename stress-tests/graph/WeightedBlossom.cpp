// Tests WeightedBlossom.h: maximum weight matching vs a bitmask-DP brute force on
// n <= 12 (self-loops, parallel edges, negative weights, weights up to 1e9,
// one object reused via init()), mate[] validity and its weight, and
// agreement with the other weighted general matching on n <= 150.
// written by Claude (audit)
#include <bits/extc++.h>
#include "../utilities/template.h"
#include "../../content/graph/WeightedBlossom.h"
#include "../../content/graph/GabowWeighted.h"

mt19937 rng(15);
ll R(ll k) { return (ll) (rng() % (unsigned long long) k); }

ll brute(int n, vt<array<ll, 3>>& es) {
    vt<vl> w(n, vl(n, 0));
    for (auto [u, v, c] : es) if (u != v) w[u][v] = w[v][u] = max(w[u][v], c);
    vl dp(1 << n);
    FOR(m, 1, 1 << n) {
        int i = __builtin_ctz(m), r = m ^ 1 << i;
        dp[m] = dp[r];
        FOR(j, i + 1, n) if (r >> j & 1) dp[m] = max(dp[m], w[i][j] + dp[r ^ 1 << j]);
    }
    return dp[(1 << n) - 1];
}

WeightedBlossom G;
template<class M> ll run(M& g, int n, vt<array<ll, 3>>& es) {
    g.init(n);
    for (auto [u, v, c] : es) g.ae(int(u) + 1, int(v) + 1, c);
    ll got = g.solve(), sum = 0;
    FOR(v, 1, n + 1) if (g.mate[v]) {
        int w = g.mate[v];
        assert(w != v && g.mate[w] == v);
        if (w < v) continue;
        ll best = LLONG_MIN;
        for (auto [a, b, c] : es)
            if ((a + 1 == v && b + 1 == w) || (a + 1 == w && b + 1 == v)) best = max(best, c);
        assert(best != LLONG_MIN);
        sum += best;
    }
    assert(sum == got);
    return got;
}

int main() {
    F0R(it, 40000) {
        int n = int(R(13)), m = n ? int(R(n * n + 2)) : 0;
        ll W = it % 4 == 0 ? 3 : it % 4 == 1 ? 50 : 1000000000;
        vt<array<ll, 3>> es;
        F0R(i, m) es.pb({R(n), R(n), 1 + R(W) - (it % 5 == 0 ? R(W) : 0)});
        assert(run(G, n, es) == brute(n, es));
    }
    WeightedBlossom A; GabowWeighted B;
    F0R(it, 300) {
        int n = 2 + int(R(150)), m = it % 2 ? 3 * n : n * (n - 1) / 4;
        ll W = it % 3 == 0 ? 2 : 1000000000;
        vt<array<ll, 3>> es;
        F0R(i, m) es.pb({R(n), R(n), 1 + R(W)});
        assert(run(A, n, es) == run(B, n, es));
    }
    cout << "Tests passed!" << endl;
}
