// Tests EulerTransform.h: mset/pset vs multiplying in one factor at a time
// (1/(1-x^k): stride-k prefix sums, 1+x^k: stride-k update, a[k] times)
// for small a[k] >= 0; vs products of binomial series for arbitrary
// residues a[k]; p(n) vs the pentagonal recurrence and p(100); distinct
// parts = odd parts; Euler's pentagonal theorem (all a[k] = -1); subset-sum
// counts vs 0/1 knapsack with items of value 0; n = 0, 1, 2, a[0] ignored,
// inputs shorter and longer than n; n up to 2^16 + 1.
// written by Claude
#include "../utilities/template.h"

namespace ignore { // NTT.h needs mod = 998244353, not ModPow's 1e9+7
#include "../../content/number-theory/ModPow.h"
}
const ll mod = 998244353;
ll mpow(ll b, ll e = mod - 2) {
    ll ans = 1;
    for (; e; b = b * b % mod, e /= 2) if (e & 1) ans = ans * b % mod;
    return ans;
}
// NTT.h's own `int inv = mpow(..)` warns under -Wconversion (pre-existing);
// silence it for that file only, the headers below are checked in full.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "../../content/numerical/NumberTheoreticTransform.h"
#pragma GCC diagnostic pop
#include "../../content/generating-functions/EulerTransform.h"

mt19937_64 rng(99);
int rndInt(int lo, int hi) { return lo + int(rng() % unsigned(hi - lo + 1)); }
vl one(int n) { vl r(n); if (n) r[0] = 1; return r; }

vl msetDP(const vl& a, int n) { // times 1/(1 - x^k), a[k] times
    vl c = one(n);
    FOR (k, 1, min(n, size(a))) F0R (r, a[k])
        FOR (i, k, n) c[i] = (c[i] + c[i - k]) % mod;
    return c;
}
vl psetDP(const vl& a, int n) { // times (1 + x^k), a[k] times
    vl c = one(n);
    FOR (k, 1, min(n, size(a))) F0R (r, a[k])
        ROF (i, k, n) c[i] = (c[i] + c[i - k]) % mod;
    return c;
}
vl mulN(const vl& a, const vl& b, int n) { // a * b mod x^n
    vl c(n);
    F0R (i, min(n, size(a))) F0R (j, min(n - i, size(b)))
        c[i + j] = (c[i + j] + a[i] * b[j]) % mod;
    return c;
}
// (1 - x^k)^{-e} = sum_j C(e + j - 1, j) x^{jk}, (1 + x^k)^e = sum_j C(e, j) x^{jk}
vl factor(ll e, int k, int n, bool p) {
    vl c(n);
    ll b = 1;
    for (int j = 0; j * k < n; j++) {
        c[j * k] = b;
        ll num = p ? (e - j + mod) % mod : (e + j) % mod;
        b = b * num % mod * mpow(j + 1) % mod;
    }
    return c;
}
vl binomN(const vl& a, int n, bool p) {
    vl c = one(n);
    FOR (k, 1, min(n, size(a))) c = mulN(c, factor(a[k], k, n, p), n);
    return c;
}
vl partPent(int n) { // p(i) by Euler's pentagonal recurrence
    vl p(n);
    F0R (i, n) {
        if (!i) { p[i] = 1; continue; }
        ll v = 0;
        for (int q = 1; q * (3 * q - 1) / 2 <= i; q++) {
            int g1 = q * (3 * q - 1) / 2, g2 = q * (3 * q + 1) / 2;
            ll term = (p[i - g1] + (g2 <= i ? p[i - g2] : 0)) % mod;
            v = (v + (q % 2 ? term : mod - term)) % mod;
        }
        p[i] = v;
    }
    return p;
}

int main() {
    // tiny cases
    assert(mset({}, 0).empty() && pset({5, 6}, 0).empty());
    assert(mset({}, 1) == vl{1} && pset({7, 3}, 1) == vl{1});
    assert(mset({7, 3}, 2) == vl({1, 3}) && pset({7, 3}, 2) == vl({1, 3}));
    assert(mset({0, 1}, 4) == vl(4, 1)); // 1 / (1 - x)
    assert(pset({0, 1}, 4) == vl({1, 1, 0, 0})); // 1 + x
    assert(pset({0, 2}, 4) == vl({1, 2, 1, 0})); // (1 + x)^2

    // small nonnegative a[k] vs multiplying factor by factor
    vi sizes = {0, 1, 2, 3, 4, 5, 7, 8, 9, 16, 17};
    for (int b = 32; b <= 256; b *= 2) for (int d : {-1, 0, 1}) sizes.pb(b + d);
    sizes.pb(100);
    for (int n : sizes) F0R (it, n <= 20 ? 30 : 5) {
        int len = rng() % 4 ? rndInt(0, n + 2) : rndInt(0, 2 * n + 20);
        vl a(len);
        for (ll& x : a) x = rng() % 3 ? ll(rng() % 4) : 0;
        if (len) a[0] = ll(rng() % mod); // ignored
        vl m = mset(a, n), p = pset(a, n);
        assert(size(m) == n && size(p) == n);
        assert(m == msetDP(a, n) && p == psetDP(a, n));
    }

    // arbitrary residues a[k] (e.g. mod - 1 = exponent -1) vs binomial series
    F0R (it, 300) {
        int n = rndInt(0, 40), len = rndInt(0, n + 5);
        vl a(len);
        for (ll& x : a) {
            int r = int(rng() % 4);
            x = r == 0 ? mod - 1 : r == 1 ? mod - ll(rng() % 4) - 1 : ll(rng() % mod);
        }
        assert(mset(a, n) == binomN(a, n, 0));
        assert(pset(a, n) == binomN(a, n, 1));
    }

    // partitions: p(100), all p(i) < 2^16 + 1 vs the pentagonal recurrence
    {
        const int N = (1 << 16) + 1;
        vl p = mset(vl(N, 1), N), want = partPent(N);
        assert(size(p) == N && p == want);
        assert(p[100] == 190569292);
        assert(p[1000] == want[1000]);
        vl q = pset(vl(N, 1), N), odd(N);
        for (int k = 1; k < N; k += 2) odd[k] = 1;
        assert(q == mset(odd, N)); // distinct parts = odd parts
        assert(q[100] == 444793);
        assert(vl(q.begin(), q.begin() + 2001) == psetDP(vl(2001, 1), 2001));
        // Euler: prod (1 - x^k) = sum_{j in Z} (-1)^j x^{j(3j-1)/2}
        vl e(N);
        for (int j = -300; j <= 300; j++) {
            ll g = ll(j) * (3 * j - 1) / 2;
            if (g < N) e[g] = j % 2 ? mod - 1 : 1;
        }
        assert(mset(vl(N, mod - 1), N) == e);
        // inverses: mset(a) mset(-a) = 1, pset(a) pset(-a) = 1
        vl a(N + 7), na(N + 7);
        F0R (i, N + 7) a[i] = ll(rng() % mod), na[i] = (mod - a[i]) % mod;
        assert(cut(conv(mset(a, N), mset(na, N)), N) == one(N));
        assert(cut(conv(pset(a, N), pset(na, N)), N) == one(N));
    }

    // subsets of items with values v_i >= 0 summing to t vs 0/1 knapsack
    F0R (it, 400) {
        int n = rndInt(0, 80), m = rndInt(0, 60);
        vi vals(m);
        for (int& v : vals) v = rng() % 3 ? rndInt(0, 6) : rndInt(0, n + 10);
        vl dp = one(n), cnt(1);
        for (int v : vals) {
            ROF (t, v, n) dp[t] = (dp[t] + dp[t - v]) % mod;
            if (v >= size(cnt)) cnt.resize(v + 1);
            cnt[v]++;
        }
        vl ps = pset(cnt, n);
        ll z = mpow(2, cnt[0]);
        F0R (t, n) assert(ps[t] * z % mod == dp[t]);
    }
    cout << "Tests passed!" << endl;
}
